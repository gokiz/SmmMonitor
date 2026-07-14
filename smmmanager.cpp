#include "smmmanager.h"
#include <QDebug>

SmmManager::SmmManager(QObject *parent)
    : QObject{parent}, m_serialPort(new QSerialPort(this)),m_saturation(0)
{
    connect(m_serialPort, &QSerialPort::readyRead, this, &SmmManager::readData);

    m_handshakeTimer = new QTimer(this);
    m_handshakeTimer->setSingleShot(true);
    connect(m_handshakeTimer, &QTimer::timeout, this, &SmmManager::sendNextHandshakeByte);

}
SmmManager::~SmmManager()
{
        if(m_serialPort->isOpen()){
            m_serialPort->close();
        }
}
void SmmManager::connectToModule(const QString &portName) {
    m_serialPort->setPortName(portName);
    m_serialPort->setBaudRate(375000); //smm modülüne göre ayarlan
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setParity(QSerialPort::OddParity);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

    if (m_serialPort->open(QIODevice::ReadWrite)){
        qDebug() << "SMM Modülüne başarıyla bağlanıldı: " << portName;
    } else {
        qDebug() << "Port açılamadı!" << m_serialPort->errorString();
    }
}

//Modüle "Biolight modunda çalış" talimatı gönderen komut
void SmmManager::initializeBiolightModule(){
    if(!m_serialPort || !m_serialPort->isOpen()){
        return;
    }
    m_handshakeStep = 0;
    sendNextHandshakeByte();
}
void SmmManager::sendNextHandshakeByte(){
    if(!m_serialPort->isOpen())
        return;

    quint8 toSend = 0;
    switch(m_handshakeStep) {
    case 0: toSend = 0xBF; break;
    case 1: toSend = 0x5F; break;
    case 2: toSend = 0xFF; break;
    default:
        qDebug() << "El sıkışma tamamlandı, artık paketli veri bekleniyor.";
        sendBiolightSpo2Setting(0xB2);
        return;
    }

    char byte = static_cast<char>(toSend);
    m_serialPort->write(&byte, 1);
    m_serialPort->flush();
    qDebug() << "handshake byte gönderildi -> 0x" + QString::number(toSend,16).toUpper();

    m_handshakeStep++;
    m_handshakeTimer->start(50); // her byte arasında 50 ms bekle
}
void SmmManager::sendBiolightSpo2Setting(quint8 configByte) {
    if (!m_serialPort->isOpen())
        return;

    const quint8 len = 2;   // CODE(1) + DATA(1)
    const quint8 code = 6;
    QByteArray data;
    data.append(static_cast<char>(configByte));

    QByteArray packet;
    packet.append(static_cast<char>(0xAA));
    packet.append(static_cast<char>(0x55));
    packet.append(static_cast<char>(len));
    packet.append(static_cast<char>(code));
    packet.append(data);
    packet.append(static_cast<char>(calcChecksum(len, code, data)));

    m_serialPort->write(packet);
    m_serialPort->flush();

    qDebug().noquote() << "Biolight setting paketi gönderildi (Hex):" << packet.toHex(' ').toUpper();
}
void SmmManager::readData(){
    QByteArray rawData = m_serialPort->readAll();

    //debug kolaylığı: modülden veri akıp akmadığı konsolda gösterilir
    if(!rawData.isEmpty()){
        qDebug() << "Gelen Ham Veri(Hex): " << rawData.toHex(' ').toUpper();
    }
    m_buffer.append(rawData);
    parseBuffer();
}
quint8 SmmManager::calcChecksum(quint8 len, quint8 code, const QByteArray &data){
    quint32 sum = len + code;
    for (char b : data){
        sum += static_cast<quint8>(b);
    }
    return static_cast<quint8>(sum & 0xFF);
}

void SmmManager::parseBuffer(){
    const quint8 BIOLIGHT_CODE = 21; //0x15
    while(true) {
        // Gerçek çerçeve başlığını (0xAA, 0x55) ara- 21 değil
        //21 zaten code alanı, header değil
        int headerIndex = -1;
        for(int i = 0; i + 1 < m_buffer.size(); ++i){
            if(static_cast<quint8>(m_buffer[i] )== 0xAA &&
                static_cast<quint8>(m_buffer[i + 1]) == 0x55){
                headerIndex = i;
                break;
            }
        }
        if (headerIndex < 0) {
            if (m_buffer.size() > 1)
                m_buffer = m_buffer.right(1); // olası yarım 0xAA'yı sakla
            return;
        }
        if(headerIndex > 0)
            m_buffer.remove(0, headerIndex);
        if(m_buffer.size() < 3)
            return; // LEn byte'ı henüz gelmedi
        const quint8 len = static_cast<quint8>(m_buffer[2]); // code + data uzunluğu
        const int totalPacketSize = 2 + 1 + len +1 ; // header+len+(code+data) + checksum

        if(m_buffer.size() < totalPacketSize)
            return; //paket tam gelmedi,bekle

        const quint8 code = static_cast<quint8>(m_buffer[3]);
        const QByteArray data = m_buffer.mid(4, len - 1);
        const quint8 receivedChecksum = static_cast<quint8>(m_buffer[totalPacketSize - 1]);
        const quint8 expectedChecksum = calcChecksum(len, code, data);

        if(receivedChecksum != expectedChecksum){
            qDebug() << "Checksum hatası, paket atlandı. code= " << code;
            m_buffer.remove(0, totalPacketSize);
            continue;
        }
        if(code == BIOLIGHT_CODE && data.size() >= 9){
            //DATA Byte0: bit6= sensör off/on
            const quint8 data0 = static_cast<quint8>(data[0]);
            const bool inSensorOff = (data0 & 0x40) != 0;

            //DATA Byte3: Sp02 (0-100, 127 = geçersiz)
            const quint8 rawSpo2 = static_cast<quint8>(data[3]);

            //terminalde hem hex hem de decimal göster
            qDebug().noquote() << QString("SpO2 ham bayt -> Hex: 0x%1 Decimal: %2")
                                      .arg(rawSpo2, 2, 16,QChar('0')).toUpper()
                                      .arg(rawSpo2);
            if(inSensorOff || rawSpo2 == 127){
                if(m_saturation != 0) {
                    m_saturation = 0;
                    emit saturationChanged(m_saturation);
                    qDebug() << "Yeni SpO2 değeri :" << m_saturation;
                }
            }
        }
        m_buffer.remove(0, totalPacketSize);
    }


}