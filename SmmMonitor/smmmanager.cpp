#include "smmmanager.h"
#include <QDebug>
#include <QDateTime>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>

SmmManager::SmmManager(QObject *parent)
    : QObject{parent}, m_serialPort(new QSerialPort(this)),m_saturation(0), m_pulseRate(0), m_isSignalWeak(false),m_beepVoice(false)
{
    connect(m_serialPort, &QSerialPort::readyRead, this, &SmmManager::readData);

    m_handshakeTimer = new QTimer(this);
    m_handshakeTimer->setSingleShot(true);
    connect(m_handshakeTimer, &QTimer::timeout, this, &SmmManager::sendNextHandshakeByte);

    //Watchdog zamanlayıcısının kurulumu
    m_watchdogTimer = new QTimer(this);
    m_watchdogTimer->setSingleShot(true);
    connect(m_watchdogTimer, &QTimer::timeout, this, &SmmManager::onWatchdogTimeout);

    m_reconnectTimer = new QTimer(this);
    connect(m_reconnectTimer, &QTimer::timeout, this, &SmmManager::tryReconnect);

    connect(m_serialPort, &QSerialPort::errorOccurred, this, &SmmManager::handlePortError);

    initDatabase();

}
SmmManager::~SmmManager()
{
    if(m_serialPort->isOpen()){
        m_serialPort->close();
    }
}

void SmmManager::connectToModule(const QString &portName) {

    m_lastPortName = portName;
    m_serialPort->setPortName(portName);
    m_serialPort->setBaudRate(375000); // smm modülüne göre ayarlan
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setParity(QSerialPort::OddParity);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

    if (m_serialPort->open(QIODevice::ReadWrite)) {
        qDebug() << "The SMM modul has been successfully connected : " << portName;

        if(!m_isPortConnected){
            m_isPortConnected = true;
            emit isPortConnectedChanged(m_isPortConnected);
        }
        m_reconnectTimer->stop();
        m_watchdogTimer->start(1000); // port açıldığında 1 saniyelik geri sayımı başlat
    } else {
        qDebug() << "The Port could not be opened!" << m_serialPort->errorString();
        if(!m_reconnectTimer->isActive()){
            m_reconnectTimer->start(2000);
        }
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
        qDebug() << " The handshake has been completed, now waiting for the packetized data.";
        sendBiolightSpo2Setting(0xB2);
        return;
    }

    char byte = static_cast<char>(toSend);
    m_serialPort->write(&byte, 1);
    m_serialPort->flush();
    qDebug() << "handshake byte sent -> 0x" + QString::number(toSend,16).toUpper();

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

    qDebug().noquote() << "Biolight setting package has been sent (Hex):" << packet.toHex(' ').toUpper();
}

quint8 SmmManager::calcChecksum(quint8 len, quint8 code, const QByteArray &data){
    quint32 sum = len + code;
    for (char b : data){
        sum += static_cast<quint8>(b);
    }
    return static_cast<quint8>(sum & 0xFF);
}

void SmmManager::readData(){
    QByteArray rawData = m_serialPort->readAll();
    // Ham veriyi yazdıran qDebug kapatıldı. Böylece arayüz kilitlemeyecek.
    m_buffer.append(rawData);
    parseBuffer();
}

//3 saniye boyunca veri gelmezse çalışacak kurtarma fonksiyonu
void SmmManager::onWatchdogTimeout() {
    if(m_saturation != 0 || m_pulseRate != 0 || m_isSignalWeak){
        m_saturation = 0;
        m_pulseRate = 0;
        m_isSignalWeak = false;
        m_beepVoice = false;
        m_pulseSearch = false;

        emit saturationChanged(m_saturation); // Arayüzü "--" durumuna açık
        emit pulseRateChanged(m_pulseRate);
        emit isSignalWeakChanged(m_isSignalWeak);
        emit beepVoiceChanged(m_beepVoice);
        emit pulseSearchChanged(m_pulseSearch);
    }
    qDebug() << "[WARNING] Data flow interrupted! Module is being awakened.";
    initializeBiolightModule(); //el sıkışma komutlarını baştan gönder
}

void SmmManager::initDatabase(){
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);

    QString dbPath = dataDir + "/SmmData.db";
    db.setDatabaseName("SmmData.db");
    qDebug() << "DB Path:" << dbPath;

    if(!db.open()){
        qDebug() << "The database could not be opened:" << db.lastError().text();
        return;
    }
    QSqlQuery query;
    QString createTable = "CREATE TABLE IF NOT EXISTS measurements ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "timestamp TEXT,"
                          "spo2 INTEGER,"
                          "pulse_rate INTEGER)";
    if(!query.exec(createTable)){
        qDebug() << "The table could not be opened: " << query.lastError().text();
    }
}

void SmmManager::refreshHistoryModel(){
    if (!m_historyModel) return;

    QString queryString;

    //filtre aktifse yenilemeyi filtreye göre yap
    if(m_isFilterActive){
        QString startStr = m_filterStartDate + " T00:00:00";
        startStr.replace(" ", "T");
        startStr += ":00";

        QString endStr = m_filterEndDate + "T23:59:59";
        endStr.replace(" ","T");
        endStr += ":59";

        queryString = QString("SELECT timestamp, spo2, pulse_rate AS pulseRate "
                              "FROM measurements "
                              "WHERE timestamp >= '%1' AND timestamp <= '%2' "
                              "ORDER BY id DESC").arg(startStr, endStr);
    }
    //filtre aktfi değilse tüm lüsteyi getir
    else {
        queryString = "SELECT timestamp, spo2, pulse_rate AS pulseRate "
                      "FROM measurements "
                      "ORDER BY id DESC";
    }

    m_historyModel->setQuery(queryString);
    if(m_historyModel->lastError().isValid()){
        qDebug() << "Model Refresh Error: " << m_historyModel->lastError().text();
    }
}
void SmmManager::insertMeasurement(int spo2, int pulseRate) {
    QSqlQuery query;

    query.prepare("INSERT INTO measurements (timestamp, spo2, pulse_rate) VALUES (:timestamp, :spo2, :pulse_rate)");


    query.bindValue(":timestamp", QDateTime::currentDateTime().toString(Qt::ISODate));
    query.bindValue(":spo2", spo2);
    query.bindValue(":pulse_rate", pulseRate);

    if (!query.exec()) {
        qDebug() << "Could not be saved to the database:" << query.lastError().text();
    } else {
        emit refreshHistoryModel();
    }
}

QSqlQueryModel* SmmManager::getHistoryModel() {
    if(!m_historyModel){

        struct RoleEnableModel : public QSqlQueryModel{
            QHash<int, QByteArray> roleNames() const override{
                QHash<int, QByteArray> roles;
                roles[Qt::UserRole + 1] = "timestamp";
                roles[Qt::UserRole + 2] = "spo2";
                roles[Qt::UserRole + 3] = "pulseRate";
                return roles;
            }
            QVariant data (const QModelIndex &index, int role) const override{
                if(!index.isValid())
                    return QVariant();
                int column = -1;
                switch (role) {
                case Qt::UserRole + 1: column = 0; break;
                case Qt::UserRole + 2: column = 1; break;
                case Qt::UserRole + 3: column = 2; break;
                }
                QModelIndex sourceIndex = this->index(index.row(),column);
                return QSqlQueryModel::data(sourceIndex, Qt::DisplayRole);
            }
        };
        m_historyModel = new RoleEnableModel();
    }
    refreshHistoryModel();
    return m_historyModel;
}

void SmmManager::handlePortError(QSerialPort::SerialPortError error){
    if(error == QSerialPort::ResourceError){
        qDebug() << "Port connection lost (Cable disconnected)!";
        m_serialPort->close();

        if(m_isPortConnected){
            m_isPortConnected = false;
            emit isPortConnectedChanged(m_isPortConnected);
        }
        onWatchdogTimeout();

        m_reconnectTimer->start(2000);
    }
}
void SmmManager::tryReconnect(){
    if(!m_lastPortName.isEmpty()){
        qDebug() << "The port is being rescanning: " << m_lastPortName;
        connectToModule(m_lastPortName);
    }
}
void SmmManager::filterHistoryByDate(const QString &startDate, const QString &endDate) {

    //filtreyi hafızaya al ve aktif et
    m_isFilterActive = true;
    m_filterStartDate = startDate;
    m_filterEndDate = endDate;

    refreshHistoryModel();
}
void SmmManager::clearFilter(){

    //filtreyi kapat ve hafızayı temizle
    m_isFilterActive = false;
    m_filterStartDate.clear();
    m_filterEndDate.clear();

    refreshHistoryModel();
}

void SmmManager::clearHistory(){
    QSqlQuery query;

    //measurements tablosundaki tüm verileri siler
    if(!query.exec("DELETE FROM measurements")) {
        qDebug() << "Database Delection Error: " << query.lastError().text();
    }else {
        query.exec("DELETE FROM sqlite_sequence WHERE  name = 'measurements' ");
        qDebug() << "The database history has been successfully deleted.";

        refreshHistoryModel();
    }
}
void SmmManager::deleteHistoryByDateRange(const QString &startDate, const QString &endDate){
    if(startDate.isEmpty() || endDate.isEmpty() || startDate == "--" || endDate == "--"){
        qDebug() << "No valid date range was selected for deletion.";
        return;
    }
    QString startStr = startDate;
    startStr.replace(" ", "T");
    startStr += ":00";

    QString endStr = endDate;
    endStr.replace(" ", "T");
    endStr += ":59";

    QSqlQuery query;
    query.prepare("DELETE FROM measurements WHERE timestamp >= :start AND timestamp <= :end");
    query.bindValue(":start", startStr);
    query.bindValue(":end", endStr);

    if(!query.exec()){
        qDebug() << "Error occured while deleting the date range:" << query.lastError().text();
    }else {
        qDebug() << "Data between" << startStr << " and " << endStr << "has been successfully deleted.";
    }
    refreshHistoryModel();

}

void SmmManager::parseBuffer(){
    const quint8 BIOLIGHT_CODE = 21; //0x15
    while(true) {
        int headerIndex = -1;
        for(int i = 0; i + 1 < m_buffer.size(); ++i){
            if(static_cast<quint8>(m_buffer[i]) == 0xAA &&
                static_cast<quint8>(m_buffer[i + 1]) == 0x55){
                headerIndex = i;
                break;
            }
        }

        if (headerIndex < 0) {
            if (m_buffer.size() > 1)
                m_buffer = m_buffer.right(1);
            return;
        }

        if(headerIndex > 0)
            m_buffer.remove(0, headerIndex);

        if(m_buffer.size() < 3)
            return;

        const quint8 len = static_cast<quint8>(m_buffer[2]);
        const int totalPacketSize = 2 + 1 + len + 1;

        if(m_buffer.size() < totalPacketSize)
            return;

        const quint8 code = static_cast<quint8>(m_buffer[3]);
        const QByteArray data = m_buffer.mid(4, len - 1);
        const quint8 receivedChecksum = static_cast<quint8>(m_buffer[totalPacketSize - 1]);
        const quint8 expectedChecksum = calcChecksum(len, code, data);

        if(receivedChecksum != expectedChecksum){
            // ÇOK ÖNEMLİ DEĞİŞİKLİK: Checksum hatası varsa sadece ilk 0xAA baytını siliyoruz.
            // Böylece gerçek paketi yanlışlıkla silme riskini (sonsuz hata döngüsünü) yok ediyoruz.
            m_buffer.remove(0, 1);
            continue;
        }

        if(code == BIOLIGHT_CODE && data.size() >= 9){
            //başarılı paket aldık, bekçi zamanlayıcısı sıfırlansın ki modül yeniden başlatılmasın
            m_watchdogTimer->start(1000);
            const quint8 data0 = static_cast<quint8>(data[0]);
            const bool inSensorOff = (data0 & 0x40) != 0;
            const bool isWeak = (data0 & (1 << 4)) != 0;
            const bool isBeepVoice = (data0 & (1 << 5) ) != 0;
            const bool currentPulseSearch = (data0 & (1 << 7)) != 0;

            if(m_pulseSearch != currentPulseSearch){
                m_pulseSearch = currentPulseSearch;
                emit pulseSearchChanged(m_pulseSearch);
            }

            if(m_beepVoice != isBeepVoice){
                m_beepVoice = isBeepVoice;
                emit beepVoiceChanged(m_beepVoice);
            }

            if(m_isSignalWeak != isWeak){
                m_isSignalWeak = isWeak;
                qDebug() << ">>> Signal Status Changed -> Weak Signal:" << m_isSignalWeak;
                emit isSignalWeakChanged(m_isSignalWeak);
            }
            const quint8 rawSpo2 = static_cast<quint8>(data[3]);
            const quint8 prMsb = static_cast<quint8>(data[4]);
            const quint8 prLsb = static_cast<quint8>(data[5]);
            int rawPulseRate = (prMsb << 8 | prLsb);

            if(inSensorOff || rawSpo2 == 127 || rawPulseRate == 255){
                if(m_saturation != 0) {
                    m_saturation = 0;
                    emit saturationChanged(m_saturation);
                }
                if(m_pulseRate != 0){
                    m_pulseRate = 0;
                    emit pulseRateChanged(m_pulseRate);
                }
            } else {
                if(m_saturation != rawSpo2 || m_pulseRate != rawPulseRate) {
                    m_saturation = rawSpo2;
                    m_pulseRate = rawPulseRate;

                    emit saturationChanged(m_saturation);
                    emit pulseRateChanged(m_pulseRate);

                    insertMeasurement(rawSpo2, rawPulseRate);
                }

            }
        }
        m_buffer.remove(0, totalPacketSize);
    }
}