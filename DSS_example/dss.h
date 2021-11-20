#ifndef DSS_H
#define DSS_H

#include <QtGlobal>
#include <QTime>

#include <QCryptographicHash>
#include <QByteArray>

class DSS {
    uint64_t
        _G {}, // open prime integer
        _P {}, //   too
        _q {}, //   too

        _X {}; // secret integer key

    uint64_t _Y {}; // open integer key

    QCryptographicHash *_sha1;

public:
    DSS(uint64_t G, uint64_t P, uint64_t q, uint64_t X);

    const size_t L = 64; // uint64_t length in bits
    const size_t BYTE_SIZE = 8; // length in bits

    QByteArray sign(QByteArray M);
    bool verify(QByteArray M);

    // tools
    uint64_t power(uint64_t x, uint64_t y);
    uint64_t rand64();

    uint64_t Y() const;

private /* process */:
    void calculateY();
    void sign(QString M);
};

#endif // DSS_H
