#include "dss.h"

#include <sstream>

#include <QDebug>

uint64_t DSS::Y() const
{
    return _Y;
}

DSS::DSS(uint64_t G, uint64_t P, uint64_t q, uint64_t X)
    : _G(G)
    , _P(P)
    , _q(q)
    , _X(X)
    , _sha1(new QCryptographicHash(QCryptographicHash::Sha1))
{
    calculateY();
    qDebug() << "Params: G = " << _G << " | P = " << _P
             << " | q = " << _q << " | X = " << _X
             << " | Y = " << _Y;
}

/*!
 * \brief DSS::sign
 * \param M
 * \return string of hex numbers
 */
QByteArray DSS::sign(QByteArray M)
{
    uint64_t m {}, K {};
    msg = M;

    // 1. Hash message
    auto hash = _sha1->hash(M, QCryptographicHash::Sha1);
    for (char byte : hash.mid(0, BYTE_SIZE)) {
        m += byte;
        m <<= BYTE_SIZE;
    }

    while (_s == 0)
    {
        // 2. Random k
        K = rand64();

        // 3. Calculate r
        _r = (power(_G, K) % _P) % _q;

        // 4. Calculate s
        _s = (m + _r * _X) / K % _q;
    }

    // 5. Appending S = [r, s] to message
    for (uint64_t val : { _r, _s }) {
        QByteArray temp {};
        for (size_t i = 0; i != BYTES_COUNT; ++i) {
            temp.push_front(static_cast<char>(val & 0xFF));
            val >>= BYTE_SIZE;
        }
        M.append(temp);
    }

    qDebug() << "Signing: r = " << _r << " | s = " << _s;
    Q_ASSERT(_r < _q && _s < _q);

    return M;
}

bool DSS::verify(QByteArray M)
{
    bool result { false };
    const QByteArray message { M };

    // Get r and s
    uint64_t s {}, r {};

#ifdef DSS_DEBUG
    s = _s;
    r = _r;
    M = msg;
#else
    auto getValue = [&](uint64_t& val) {
        for (size_t i = 0; i != BYTES_COUNT; ++i) {
            val += M.back() << BYTE_SIZE * i;
            M.remove(M.size() - 1, 1); // pop_back
        }
    };
    getValue(s);
    getValue(r);
#endif

    Q_ASSERT(s != 0);

    if (r < _q && s < _q) {
        // 1. Calculate w
        // s * w % q == 1 % q
        double w { 1.0 / static_cast<double>(s) };

        // 2. Hash message
        uint64_t m {}, s = 0;
        auto hash = _sha1->hash(M, QCryptographicHash::Sha1);
        for (char byte : hash.mid(0, BYTE_SIZE)) {
            m += byte;
            m <<= BYTE_SIZE;
        }

        // 3. Calculate u1 and u2
        uint64_t
            u1 { static_cast<uint64_t>(m * w) },
            u2 { static_cast<uint64_t>(r * w) };

        u1 = u1 % _q;
        u2 = u2 % _q;

        // 4. Calculate v
        uint64_t v { (power(_G, u1) * s * power(_Y, u2) % _P) % _q + r };

        qDebug() << "Verifying: r = " << _r << " | v = " << v;

        // Check
        if (r == v)
            result = true;
    }

    return result;
}

/*!
 * \brief DSS::power
 * \param x
 * \param y
 * \return std::pow(x, y)
 */
uint64_t DSS::power(uint64_t x, uint64_t y)
{
    if (y == 0)
        return 1;
    else if (y % 2 == 0)
        return power(x, y / 2) * power(x, y / 2);
    else
        return x * power(x, y / 2) * power(x, y / 2);
}

uint64_t DSS::rand64()
{
    uint64_t randomNum {};

#ifdef DSS_DEBUG
    randomNum += rand() & 0xFF;
    randomNum++;
#else
    for (size_t i = 0; i < L / BYTE_SIZE; ++i) {
        randomNum += rand() & 0xFF;
        randomNum <<= BYTE_SIZE;
    }
#endif

    return randomNum;
}

void DSS::calculateY()
{
    // calculating power
    _Y = power(_G, _X) % _P;
}
