#ifndef DSS_H
#define DSS_H

#include <QTime>
#include <QtGlobal>

#include <QByteArray>
#include <QCryptographicHash>

#define DSS_DEBUG

struct EncodedMessage {
    QByteArray hash {};
    QByteArray r {};
    QByteArray s {};
};

class DSS {
    uint64_t
        _G {}, // open prime integer
        _P {}, //   too
        _q {}, //   too

        _X {}; // secret integer key

    uint64_t
        _Y {}; // open integer key

    QCryptographicHash* _sha1;

public:
    DSS(uint64_t G, uint64_t P, uint64_t q, uint64_t X);

    static const size_t L = 64; // uint64_t length in bits
    static const size_t BYTE_SIZE = 8; // length in bits
    static const size_t BYTES_COUNT = L / BYTE_SIZE;

    EncodedMessage sign(const QByteArray &M);
    bool verify(const EncodedMessage &encodedMsg);

    // tools
    uint64_t power(uint64_t x, uint64_t y);
    uint64_t rand64();

    uint64_t Y() const;

    static uint64_t fromByteArray(const QByteArray &bytes);
    static QByteArray toByteArray(const uint64_t &data);

private /* process */:
    void calculateY();
    void sign(QString M);
};

#endif // DSS_H
