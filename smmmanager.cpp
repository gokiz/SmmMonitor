#include "smmmanager.h"
#include <QDebug>
SmmManager::SmmManager(QObject *parent)
    : QObject{parent}, m_serialPort(new QSerialPort(this)),m_saturation(0)
{
    connect(m_serialPort, &QSerialPort::readyRead, this, &SmmManager::readData);
}
    SmmManager::~SmmManager()
{
        if(m_serialPort->isOpen()){
        m_serialPort->close();
        }
}
void SmmManager::connectToModule(const QString &portName) {
    m_serialPort->setPortName(portName);
    m_serialPort->setBaudRate(QSerialPort::Baud115200); //smm modülüne göre ayarlan
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

    if (m_serialPort->open(QIODevice::ReadWrite)){
        qDebug() << "SMM Modülüne başarıyla bağlanıldı: " << portName;
    } else {
        qDebug() << "Port açılamadı!" << m_serialPort->errorString();
    }
}
void SmmManager::readData(){
    m_buffer.append(m_serialPort->readAll());
    parseBuffer();
}

void SmmManager::parseBuffer(){
    // ÖRNEK ÇÖZÜMLEME:
    // Varsayalım ki kullandığınız SMM SpO2 paketi 10 bayt uzunluğunda
    // ve her paket 0xAA 0xBB başlığıyla (header) başlıyor olsun.
    // Dokümandaki Noringmed veya Biolight paket yapılarına göre bu sabitleri değiştirebilirsiniz.
    const int PACKET_SIZE = 10;
    while(m_buffer.size() >= PACKET_SIZE){
        int headerIndex=m_buffer.indexOf("\xAAxBB");
        if(headerIndex == -1){
            //başlangıç bulunmadıysa tamponu temizle(senkranizasyon kaybı)
            m_buffer.clear();
            break;
        }
        if(headerIndex > 0){
            //başlangıçta önceki gereksiz verileri at
            m_buffer.remove(0, headerIndex);
        }
        if(m_buffer.size() < PACKET_SIZE){
            break; //tam paket henüz gelmemiş,bekklenecek
        }


        //Paket bütünlüğünü aldık
        QByteArray packet = m_buffer.left(PACKET_SIZE);
        m_buffer.remove(0, PACKET_SIZE);


        //SpO2 verisi byte 3'tedir (indeks 3)
        int parsedSpo2 = static_cast<unsigned char>(packet.at(3));
        //limit kontrolü (geçerli SpO2 aralığı 0-100, geçersiz 127)

        if(parsedSpo2 >= 0 && parsedSpo2 <= 100) {
            if(m_saturation != parsedSpo2){
                m_saturation = parsedSpo2;
                emit saturationChanged(m_saturation); //QML arayüzü haderdar edilmeli

            }
        }
    }

}