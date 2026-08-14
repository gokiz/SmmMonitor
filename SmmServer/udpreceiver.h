#ifndef UDPRECEIVER_H
#define UDPRECEIVER_H

#include <QObject>
#include <QUdpSocket>

class UdpReceiver : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int spo2 READ spo2 NOTIFY dataReceived)
    Q_PROPERTY(int pulseRate READ pulseRate NOTIFY dataReceived)
    Q_PROPERTY(int waveform READ waveform NOTIFY dataReceived)

public:
    explicit UdpReceiver(QObject *parent = nullptr);

    int spo2() const;
    int pulseRate() const;
    int waveform() const;

signals:
    void dataReceived();

private slots:
    void readPendingDatagrams();

private:
    QUdpSocket *m_udpSocket;
    int m_spo2 = 0;
    int m_pulseRate = 0;
    int m_waveform = 0;
};

#endif // UDPRECEIVER_H
