#include "aesgcmcrypto.h"
#include <QDebug>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <QProcessEnvironment>

QByteArray AesGcmCrypto::encrypt(const QByteArray &plaintext, const QByteArray &key) {
    if(key.size() != KeySize) {
        qWarning() << "AesGcmCrypto::encrypt - anahtar uzunlugu tam 16 byte olmali!";
        return QByteArray();
    }

    QByteArray iv(IvSize, '\0');
    if(RAND_bytes(reinterpret_cast<unsigned char *>(iv.data()), IvSize) != 1) {
        qWarning() << "AesGcmCrypto::encrypt - IV uretilemedi!";
        return QByteArray();
    }

    // DÜZELTME 1: Buradaki 'packet.left...' satırları decrypt'e aitti, silindi!

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return QByteArray();

    QByteArray ciphertext(plaintext.size(), '\0');
    int len = 0;
    int ciphertextLen = 0;
    bool ok = true;

    ok = ok && EVP_EncryptInit_ex(ctx, EVP_aes_128_gcm(), nullptr, nullptr, nullptr) == 1;
    ok = ok && EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, IvSize, nullptr) == 1;

    // DÜZELTME 2: 'const' dönüştürme hatalarını engellemek için mutable kopyalar kullanıldı.
    QByteArray mutableKey = key;
    ok = ok && EVP_EncryptInit_ex(ctx, nullptr, nullptr,
                                  reinterpret_cast<unsigned char *>(mutableKey.data()),
                                  reinterpret_cast<unsigned char *>(iv.data())) == 1;

    if (ok && plaintext.size() > 0) {
        QByteArray mutablePlaintext = plaintext;
        ok = EVP_EncryptUpdate(ctx,
                               reinterpret_cast<unsigned char *>(ciphertext.data()),
                               &len,
                               reinterpret_cast<unsigned char *>(mutablePlaintext.data()),
                               mutablePlaintext.size()) == 1;
        ciphertextLen = len;
    }

    ok = ok && EVP_EncryptFinal_ex(ctx,
                                   reinterpret_cast<unsigned char *>(ciphertext.data()) + ciphertextLen,
                                   &len) == 1;
    ciphertextLen += len;

    QByteArray tag(TagSize, '\0');
    ok = ok && EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, TagSize,
                                   reinterpret_cast<unsigned char *>(tag.data())) == 1;

    EVP_CIPHER_CTX_free(ctx);

    if (!ok) return QByteArray();

    ciphertext.resize(ciphertextLen);

    QByteArray packet;
    packet.reserve(IvSize + ciphertextLen + TagSize);
    packet.append(iv);
    packet.append(ciphertext);
    packet.append(tag);
    return packet;
}

QByteArray AesGcmCrypto::decrypt(const QByteArray &packet, const QByteArray &key)
{
    if (key.size() != KeySize) return QByteArray();
    if (packet.size() < IvSize + TagSize) return QByteArray();

    const QByteArray iv         = packet.left(IvSize);
    const QByteArray tag        = packet.right(TagSize);
    const QByteArray ciphertext = packet.mid(IvSize, packet.size() - IvSize - TagSize);

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return QByteArray();

    QByteArray plaintext(ciphertext.size(), '\0');
    int len = 0;
    int plaintextLen = 0;
    bool ok = true;

    ok = ok && EVP_DecryptInit_ex(ctx, EVP_aes_128_gcm(), nullptr, nullptr, nullptr) == 1;
    ok = ok && EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, IvSize, nullptr) == 1;

    QByteArray mutableKey = key;
    QByteArray mutableIv = iv;
    ok = ok && EVP_DecryptInit_ex(ctx, nullptr, nullptr,
                                  reinterpret_cast<unsigned char *>(mutableKey.data()),
                                  reinterpret_cast<unsigned char *>(mutableIv.data())) == 1;

    if (ok && ciphertext.size() > 0) {
        QByteArray mutableCiphertext = ciphertext;
        ok = EVP_DecryptUpdate(ctx,
                               reinterpret_cast<unsigned char *>(plaintext.data()),
                               &len,
                               reinterpret_cast<unsigned char *>(mutableCiphertext.data()),
                               mutableCiphertext.size()) == 1;
        plaintextLen = len;
    }

    QByteArray mutableTag = tag;
    ok = ok && EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, TagSize,
                                   reinterpret_cast<unsigned char *>(mutableTag.data())) == 1;

    int finalizeResult = 0;
    if (ok) {
        finalizeResult = EVP_DecryptFinal_ex(ctx,
                                             reinterpret_cast<unsigned char *>(plaintext.data()) + plaintextLen,
                                             &len);
    }

    EVP_CIPHER_CTX_free(ctx);

    if (!ok || finalizeResult <= 0) return QByteArray();

    plaintextLen += len;
    plaintext.resize(plaintextLen);
    return plaintext;
}

QByteArray AesGcmCrypto::loadKeyFromEnv(const char *envVarName) {
    const QString hexKey = QProcessEnvironment::systemEnvironment().value(envVarName);
    if (hexKey.isEmpty()) {
        qWarning() << "AES anahtari ortafam degiskeninde bulunamadi: " << envVarName;
        return QByteArray();
    }
    QByteArray key = QByteArray::fromHex(hexKey.toUtf8());
    if(key.size() != KeySize) {
        qWarning() << "AES anahtari yanlis uzunlukta!";
        return QByteArray();
    }
    return key;
}