#ifndef AESGCMCRYPTO_H
#define AESGCMCRYPTO_H

#include <QByteArray>

class AesGcmCrypto
{
public:
    // AES-128-GCM Standart Boyutları
    static constexpr int KeySize = 16;
    static constexpr int IvSize = 12;
    static constexpr int TagSize = 16;

    // Fonksiyonlar static olmalı ki obje üretmeden çağrılabilsin
    static QByteArray encrypt(const QByteArray &plaintext, const QByteArray &key, bool logDetails = false);
    static QByteArray decrypt(const QByteArray &packet, const QByteArray &key);
    static QByteArray loadKeyFromEnv(const char *envVarName = "SMM_AES_KEY");
};

#endif // AESGCMCRYPTO_H