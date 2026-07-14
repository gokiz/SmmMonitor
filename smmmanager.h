#ifndef SMMMANAGER_H
#define SMMMANAGER_H

#include <QObject>
#include <QSerialPort>
#include <QByteArray>


class SmmManager : public QObject
{
    Q_OBJECT
    // QML tarafında bu property'i okuyup Rectangle içinde göstereceğiz
    Q_PROPERTY(int saturation READ saturation NOTIFY saturationChanged);

public:
    explicit SmmManager(QObject *parent = nullptr);
    ~SmmManager();

    int saturation() const { return m_saturation; }
    //UART bağlantısını başlatma fonksiyonu
    Q_INVOKABLE void connectToModule(const QString &portName);
    Q_INVOKABLE void initializeBiolightModule(); // başlatma komutu fonksiyonu

signals:
    void saturationChanged(int newSaturation);
private slots:
    void readData(); //UART'a veri geldikçe tetiklenir

private:
    QSerialPort *m_serialPort;
    QByteArray m_buffer;
    int m_saturation;

    void parseBuffer(); //gelen baytları SMM protokolüne göre işler
};

#endif // SMMMANAGER_H
