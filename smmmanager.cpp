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

//Modüle "Biolight modunda çalış" talimatı gönderen komut
void SmmManager::initializeBiolightModule(){
    if(m_serialPort && m_serialPort->isOpen()){
        QByteArray initPacket;
        //dokümana göre hazırlanan konfigürasyon baytı(Adult mod, 50Hz,8s ort)
        char configByte = static_cast<char>(0xB2);
        initPacket.append(configByte);
        m_serialPort->write(initPacket);
        m_serialPort->flush();
        qDebug() << "SpO2 modülüne başlatma komutu gönderildi (0xB2) ";

    }
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

void SmmManager::parseBuffer(){
    const int BIOLIGHT_PACKET_SIZE = 10; //Biolight paket uzunluğu
    const int BIOLOGHT_HEADER_CODE = 21; //paket Kodu(ondalık 21)

    while(m_buffer.size() >= BIOLIGHT_PACKET_SIZE){
        //tampon içerisin paket başlığı(21) araması yapılıyor
        int headerIndex = m_buffer.indexOf(BIOLOGHT_HEADER_CODE);
        if(headerIndex == -1){
            //başlık bulunmadıysa tamponu aşırı dolmaması için temizleyip döngüden çıkıyoruz
            if(m_buffer.size() > 100){
                m_buffer.clear();
            }
            break;
        }
        if (headerIndex > 0){
            //başlığa kadar olan ilgisiz/kaymış baytlar temizlensin
            m_buffer.remove(0, headerIndex);
        }
        if (m_buffer.size() < BIOLIGHT_PACKET_SIZE){
            break; //paket tamamlanmadıysa bir sonraki okumayı bekle
        }

        //10 baytlık tam paketi alıp tampon yapıyoruz
        QByteArray packet = m_buffer.left(BIOLIGHT_PACKET_SIZE);
        m_buffer.remove(0, BIOLIGHT_PACKET_SIZE);
        //BİYOLOJİK VERİ AYRIŞTIRMA ADIMI
        //Byte 1: sensör durum kontrolü(bit 6 = sensor off/on)
        unsigned char byte1 = static_cast<unsigned char>(packet.at(1));
        bool isSensorOff = (byte1 & 0x40); //6.bit kontrolü
        //Byte 4: Gerçek SpO2 verisi
        unsigned char rawSpo2 = static_cast<unsigned char>(packet.at(4));

        if(isSensorOff || rawSpo2 == 127){
            //sensör takılı değilse veya okuma geçersiz değer 0 (ölçüm yok)
            if(m_saturation != 0){
                m_saturation = 0;
                emit saturationChanged(m_saturation);
                qDebug() << "Sensör Çıktı veya Değer Geçersiz";
            }
        }
        else if(rawSpo2 <= 100) {
            //geçerli SpO2 değeri (0-100) geldiyse arayüzü güncelle
            if(m_saturation != rawSpo2){
                m_saturation =rawSpo2;
                emit saturationChanged(m_saturation);
                qDebug() << "Yeni SpO2 Değeri Ölçüldü: " << m_saturation;
            }
        }
    }

}