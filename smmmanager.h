#ifndef SMMMANAGER_H
#define SMMMANAGER_H

#include <QObject>
#include <QSerialPort>
#include <QByteArray>
#include <QTimer>

class SmmManager : public QObject
{
    Q_OBJECT
    // QML tarafında bu property'i okuyup Rectangle içinde göstereceğiz
    Q_PROPERTY(int saturation READ saturation NOTIFY saturationChanged)

public:
    explicit SmmManager(QObject *parent = nullptr);
    ~SmmManager();

    int saturation() const { return m_saturation; }
    //UART bağlantısını başlatma fonksiyonu
    Q_INVOKABLE void connectToModule(const QString &portName);
    // İsim aynı kaldı ama artık gerçek "eski protokolden yeniye geçiş"
    // el sıkışmasını yapıyor (0xBF,0x5F,0xFF), 0xB2 gibi dokümanda
    // olmayan bir komut değil.
    Q_INVOKABLE void initializeBiolightModule(); // başlatma komutu fonksiyonu

signals:
    void saturationChanged(int newSaturation);

private slots:
    void readData(); //UART'a veri geldikçe tetiklenir
    void sendNextHandshakeByte();
    void onWatchdogTimeout(); // Modül uyuduğunda tetiklenecek
private:
    void parseBuffer(); //gelen baytları SMM protokolüne göre işler
    static quint8 calcChecksum(quint8 len, quint8 code, const QByteArray &data);
    void sendBiolightSpo2Setting(quint8 configByte = 0xB2);

    QSerialPort *m_serialPort;
    QByteArray m_buffer;
    int m_saturation;

    QTimer *m_handshakeTimer;
    int m_handshakeStep = 0;

    QTimer *m_watchdogTimer; // veri akışını denetleyecek zamanlayıcı
};

#endif // SMMMANAGER_H