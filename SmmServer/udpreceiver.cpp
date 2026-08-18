#include "udpreceiver.h"
#include <QDataStream>
#include <QDebug>
#include "aesgcmcrypto.h"


UdpReceiver::UdpReceiver(QObject *parent)
    : QObject(parent), m_udpSocket(new QUdpSocket(this))
{
    //Client(istemci) veri gönderdiği portu her adresten dinlemeye başlıyoruz
    if(m_udpSocket->bind(QHostAddress::Any, 45454)) {
        qDebug() << "UDP Server dinlemeye basladi. Port: 45454";
    } else {
        qWarning() << "UDP Portu dinlemedi!";
    }

    connect(m_udpSocket, &QUdpSocket::readyRead, this, &UdpReceiver::readPendingDatagrams);

    m_watchdogTimer = new QTimer(this);
    connect(m_watchdogTimer, &QTimer::timeout, this, &UdpReceiver::connectionLost);
    m_watchdogTimer->start(3000);
}

int UdpReceiver::spo2() const {return m_spo2;}
int UdpReceiver::pulseRate() const {return m_pulseRate;}
int UdpReceiver::waveform() const {return m_waveform;}
bool UdpReceiver::isConnected() const {return m_isConnected;}
int UdpReceiver::waveformSpeed() const {return m_waveformSpeed;}

void UdpReceiver::readPendingDatagrams() {
    while (m_udpSocket->hasPendingDatagrams()) {

        QByteArray encryptedDatagram;
        encryptedDatagram.resize(m_udpSocket->pendingDatagramSize());
        m_udpSocket->readDatagram(encryptedDatagram.data(), encryptedDatagram.size());

        // AES-128 anahtari: SmmManager (istemci) tarafindakiyle BIREBIR AYNI olmali.
        static const QByteArray SECRET_KEY = QByteArray::fromHex("4A9F2B8D1C7E3A6F91D4C2B8AABBCCDD");

        QByteArray decryptedDatagram = AesGcmCrypto::decrypt(encryptedDatagram, SECRET_KEY);
        if (decryptedDatagram.isEmpty()) {
            // Anahtar yanlis, paket bozuk ya da baskasi tarafindan degistirilmis/uydurulmus olabilir.
            qWarning() << "Gelen UDP paketi dogrulanamadi/cozulemedi, atlaniyor.";
            continue;
        }

        QDataStream in(&decryptedDatagram, QIODevice::ReadOnly);
        in.setVersion(QDataStream::Qt_6_0);

        int incomingSpeed;
        bool inSpo2Alarm, inPulseAlarm, inAlarmMuted;
        int inSpo2Prio, inPulsePrio;

        in >> m_spo2 >> m_pulseRate >> m_waveform >> incomingSpeed
            >> inSpo2Alarm >> inSpo2Prio >> inPulseAlarm >> inPulsePrio
            >> inAlarmMuted;

        if(m_isAlarmMuted != inAlarmMuted) {
            m_isAlarmMuted = inAlarmMuted;
            emit isAlarmMutedChanged();
        }

        if(m_waveformSpeed != incomingSpeed && (incomingSpeed == 25 || incomingSpeed == 50)){
            m_waveformSpeed = incomingSpeed;
            emit waveformSpeedChanged(m_waveformSpeed);
        }

        if(m_isSpo2AlarmActive != inSpo2Alarm || m_spo2AlarmPriority != inSpo2Prio ||
            m_isPulseAlarmActive != inPulseAlarm || m_pulseAlarmPriority != inPulsePrio){

            m_isSpo2AlarmActive = inSpo2Alarm;
            m_spo2AlarmPriority = inSpo2Prio;
            m_isPulseAlarmActive = inPulseAlarm;
            m_pulseAlarmPriority = inPulsePrio;

            emit alarmStatusChanged();
        }

        if(!m_isConnected){
            m_isConnected = true;
            emit connectionStatusChanged(m_isConnected);
        }

        m_watchdogTimer->start();

        emit dataReceived();
    }
}

void UdpReceiver::connectionLost(){
    qWarning() << "Uyari: Istemci ile baglanti koptu! Veriler sifirlaniyor...";

    m_spo2 = 0;
    m_pulseRate = 0;
    m_waveform = 0;

    if(m_isSpo2AlarmActive || m_isPulseAlarmActive) {
        m_isSpo2AlarmActive = false;
        m_isPulseAlarmActive = false;
        m_spo2AlarmPriority = 0;
        m_pulseAlarmPriority = 0;
        emit alarmStatusChanged();
    }

    if(m_isConnected) {
        m_isConnected = false;
        emit connectionStatusChanged(m_isConnected);
    }

    emit dataReceived();
}