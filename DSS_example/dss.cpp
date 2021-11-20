#include "dss.h"

#include <sstream>

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
}

/*!
 * \brief DSS::sign
 * \param M
 * \return string of hex numbers
 */
QByteArray DSS::sign(QByteArray M)
{
    uint64_t m {}, K {}, r {}, s {};

    // 1. Hash message
    m = _sha1->hash(M, QCryptographicHash::Sha1).toULongLong();

    // 2. Random k
    K = rand64();

    // 3. Calculate r
    r = (power(_G, K) % _P) % _q;

    // 4. Calculate s
    s = (m + r * _X) / K % _q;

    // 5. Appending S = [r, s] to message
    for (uint64_t val : { r, s })
    {
        QByteArray temp {};
        for (size_t i = 0; i != L / BYTE_SIZE; ++i)
        {
            temp.push_front(static_cast<char>(val & 0xFF));
            val <<= BYTE_SIZE;
        }
        M.append(temp);
    }

    return M;
}

bool DSS::verify(QByteArray M)
{
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
    for (size_t i = 0; i < L / BYTE_SIZE; ++i) {
        randomNum += rand() & 0xFF;
        randomNum <<= BYTE_SIZE;
    }
    return randomNum;
}

void DSS::calculateY()
{
    // calculating power
    _Y = power(_G, _X) % _P;
}
