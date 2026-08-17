#ifndef UDPRECEIVER_H
#define UDPRECEIVER_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>

class UdpReceiver : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int spo2 READ spo2 NOTIFY dataReceived)
    Q_PROPERTY(int pulseRate READ pulseRate NOTIFY dataReceived)
    Q_PROPERTY(int waveform READ waveform NOTIFY dataReceived)
    Q_PROPERTY(int waveformSpeed READ waveformSpeed NOTIFY waveformSpeedChanged)

    Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectionStatusChanged)

public:
    explicit UdpReceiver(QObject *parent = nullptr);

    int spo2() const;
    int pulseRate() const;
    int waveform() const;

    bool isConnected() const;
    int waveformSpeed() const;

signals:
    void dataReceived();
    void connectionStatusChanged(bool status);
    void waveformSpeedChanged(int speed);

private slots:
    void readPendingDatagrams();
    void connectionLost();

private:
    QUdpSocket *m_udpSocket;
    QTimer *m_watchdogTimer;

    int m_spo2 = 0;
    int m_pulseRate = 0;
    int m_waveform = 0;
    bool m_isConnected = false;
    int m_waveformSpeed = 25;
};

#endif // UDPRECEIVER_H
