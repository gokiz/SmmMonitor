#include "udpreceiver.h"
#include <QDataStream>
#include <QDebug>


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

void UdpReceiver::readPendingDatagrams() {
    while (m_udpSocket->hasPendingDatagrams()) {

        qDebug() << "UDP Paketi Yakalandi!";

        QByteArray datagram;
        datagram.resize(m_udpSocket->pendingDatagramSize());
        m_udpSocket->readDatagram(datagram.data(), datagram.size());

        QDataStream in(&datagram, QIODevice::ReadOnly);
        in.setVersion(QDataStream::Qt_6_0);

        in >> m_spo2 >> m_pulseRate >> m_waveform;

        m_watchdogTimer->start();

        emit dataReceived();
    }
}

void UdpReceiver::connectionLost(){
    qWarning() << "Uyari: Istemci ile baglanti koptu! Veriler sifirlaniyor...";

    m_spo2 = 0;
    m_pulseRate = 0;
    m_waveform = 0;

    emit dataReceived();
}