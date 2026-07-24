#include <QDebug>
#include "smmsimulator.h"

SmmSimulator::SmmSimulator(QObject *parent) : QObject(parent), m_simState(0), m_currentSpo2(98), m_currentPulse(75)
{
    m_simTimer = new QTimer(this);
    connect(m_simTimer, &QTimer::timeout, this, &SmmSimulator::simulateNextState);

}
void SmmSimulator::startSimulation(){
    m_simState = 0;
    m_simTimer->start(2000); //her 2 saniyede bir ekran guncellenecek
    qDebug() << "Görsel simülasyon başlıyor";

}
void SmmSimulator::stopSimulation(){
    m_simTimer->stop();
    qDebug() << "Görsel simülasyon durduruldu";
}
quint8 SmmSimulator::calcChecksum(quint8 len, quint8 code, const QByteArray &data){
    quint32 sum = len + code;
    for (char b : data) sum += static_cast<quint8>(b);
    return static_cast<quint8>(sum & 0xFF);
}

void SmmSimulator::simulateNextState(){
    quint8 data0 = 0x00;

    switch (m_simState) {
    case 0:
        qDebug() << "[SIMULASYON]: Normal Değerler";
        m_currentSpo2 = static_cast<quint8>(QRandomGenerator::global()->bounded(95,101));
        m_currentPulse = static_cast<quint8>(QRandomGenerator::global()->bounded(60,91));
        break;
    case 1:
        qDebug() << "[SIMULASYON]: Kritik Oksijen ve Nabız";
        m_currentSpo2 = static_cast<quint8>(QRandomGenerator::global()->bounded(70, 90));
        m_currentPulse = static_cast<quint8>(QRandomGenerator::global()->bounded(110, 151));
        break;
    case 2: // Zayıf Sinyal
        qDebug() << "[SİMÜLASYON]: Zayıf Sinyal Uyarısı";
        m_currentSpo2 = static_cast<quint8>(QRandomGenerator::global()->bounded(80, 95));
        m_currentPulse = static_cast<quint8>(QRandomGenerator::global()->bounded(40, 120));
        data0 |= 0x10;
        break;
    case 3: // Sensör Kapalı
        qDebug() << "[SİMÜLASYON]: Sensör Çıkarıldı";
        m_currentSpo2 = 0;
        m_currentPulse = 0;
        data0 |= 0x40;
        break;
    }
    const quint8 len = 10;
    const quint8 code = 21;

    QByteArray dataPayload;
    dataPayload.append(static_cast<char>(data0));
    dataPayload.append(static_cast<char>(0x00));
    dataPayload.append(static_cast<char>(0x00));
    dataPayload.append(static_cast<char>(m_currentSpo2));
    dataPayload.append(static_cast<char>(0x00));
    dataPayload.append(static_cast<char>(m_currentPulse));
    dataPayload.append(static_cast<char>(0x00));
    dataPayload.append(static_cast<char>(0x00));
    dataPayload.append(static_cast<char>(0x00));

    QByteArray fullPacket;
    fullPacket.append(static_cast<char>(0xAA));
    fullPacket.append(static_cast<char>(0x55));
    fullPacket.append(static_cast<char>(len));
    fullPacket.append(static_cast<char>(code));
    fullPacket.append(dataPayload);
    fullPacket.append(static_cast<char>(calcChecksum(len, code, dataPayload)));

    emit mockDataReady(fullPacket);

    m_simState = (m_simState + 1) % 4 ;

}