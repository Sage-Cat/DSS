#include "dss.h"

#include <sstream>

#include <QDebug>
#include <QRandomGenerator>

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
EncodedMessage DSS::sign(const QByteArray &M)
{
    uint64_t m {}, K {}, r {}, s {};

    // 1. Hash message
    auto hash = _sha1->hash(M, QCryptographicHash::Sha1);
    for (char byte : hash.mid(0, BYTE_SIZE)) {
        m += byte;
        m <<= BYTE_SIZE;
    }

    do
    {
        // 2. Random k
        K = rand64();

        // 3. Calculate r
        r = (power(_G, K) % _P) % _q;

        // 4. Calculate s
        s = (m + r * _X) / K % _q;
    } while (s == 0);

    // 5. Appending S = [r, s] to message
    EncodedMessage res {};
    res.hash = hash;
    res.r = toByteArray(r);
    res.s = toByteArray(s);

    qDebug() << "Signing: r = " << r << " | s = " << s;
    Q_ASSERT(r < _q && s < _q);

    return res;
}

uint64_t DSS::fromByteArray(const QByteArray &bytes)
{
    uint64_t temp {};

    for (size_t i = 0; i < BYTE_SIZE; ++i)
        temp += bytes.at(bytes.size() - i - 1) & (0xFF << i);

    return temp;
}

QByteArray DSS::toByteArray(const uint64_t &data)
{
    QByteArray temp {};

    for (int i = BYTE_SIZE - 1; i >= 0; --i)
        temp.push_back(static_cast<char>(data & (0xFF << i)));

    return temp;
}

bool DSS::verify(const EncodedMessage &encodedMsg)
{
    bool result { false };
    QByteArray M { encodedMsg.hash };

    // Get r and s
    uint64_t s { fromByteArray(encodedMsg.s) }, r { fromByteArray(encodedMsg.r) };

    Q_ASSERT(s != 0);

    if (r < _q && s < _q) {
        // 1. Calculate w
        // s * w % q == 1 % q
        double w { 1.0 / static_cast<double>(s) };

        // 2. Hash message
        uint64_t m {};
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
        uint64_t v { (power(_G, u1) * s * power(_Y, u2) % _P) % _q};

        qDebug() << "Verifying: r = " << r << " | v = " << v;

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
    QRandomGenerator gen;
    uint64_t random64 { gen.generate64() };

#ifdef DSS_DEBUG
    random64 = random64 & 0xFFFF;
#endif

    return random64;
}

void DSS::calculateY()
{
    // calculating power
    _Y = power(_G, _X) % _P;
}
