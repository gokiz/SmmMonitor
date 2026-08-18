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

    Q_PROPERTY(bool isSpo2AlarmActive READ isSpo2AlarmActive NOTIFY alarmStatusChanged)
    Q_PROPERTY(int spo2AlarmPriority READ spo2AlarmPriority NOTIFY alarmStatusChanged)
    Q_PROPERTY(bool isPulseAlarmActive READ isPulseAlarmActive NOTIFY alarmStatusChanged)
    Q_PROPERTY(int pulseAlarmPriority READ pulseAlarmPriority NOTIFY alarmStatusChanged)
    Q_PROPERTY(bool isAlarmMuted READ isAlarmMuted NOTIFY isAlarmMutedChanged )

public:
    explicit UdpReceiver(QObject *parent = nullptr);

    int spo2() const;
    int pulseRate() const;
    int waveform() const;

    bool isConnected() const;
    int waveformSpeed() const;

    bool isSpo2AlarmActive() const {return m_isSpo2AlarmActive;}
    int spo2AlarmPriority() const {return m_spo2AlarmPriority;}
    bool isPulseAlarmActive() const { return m_isPulseAlarmActive; }
    int pulseAlarmPriority() const { return m_pulseAlarmPriority; }
    bool isAlarmMuted() const {return m_isAlarmMuted;}

signals:
    void dataReceived();
    void connectionStatusChanged(bool status);
    void waveformSpeedChanged(int speed);
    void alarmStatusChanged();
    void isAlarmMutedChanged();

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

    bool m_isSpo2AlarmActive = false;
    int m_spo2AlarmPriority = 0;
    bool m_isPulseAlarmActive = false;
    int m_pulseAlarmPriority = 0;
    bool m_isAlarmMuted = false;

    quint64 m_lastSequence = 0;
};

#endif // UDPRECEIVER_H
