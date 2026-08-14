#include "smmmanager.h"
#include <QDebug>
#include <QDateTime>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QSerialPortInfo>
#include <QPdfWriter>
#include <QPageSize>
#include <QPainter>
#include <QFile>
#include <QTextStream>
#include <QFont>
#include <cmath>

namespace {
int generateRealisticPpgSample(double phase) {
    const double baseline = 18.0;
    const double amplitude = 78.0;

    double shape = 0.0;
    if(phase < 0.12) {
        const double t = phase / 0.12;
        shape = std::pow(t, 0.6);
    } else if (phase < 0.30) {
        // Sistolik zirveden hafif inis
        const double t = (phase - 0.12) / 0.18;
        shape = 1.0 - 0.35 * t;
    } else if (phase < 0.45) {
        // Dikrotik centik (kisa bir cokus)
        const double t = (phase - 0.30) / 0.15;
        shape = 0.65 - 0.20 * std::sin(t * 3.14159265358979323846);
    } else if (phase < 0.62) {
        // Dikrotik dalga (kucuk ikincil kabarma)
        const double t = (phase - 0.45) / 0.17;
        shape = 0.50 + 0.12 * std::sin(t * 3.14159265358979323846);
    } else {
        // Yavas diastolik dusus, baseline'a donus
        const double t = (phase - 0.62) / 0.38;
        shape = 0.50 * (1.0 - t);
    }

    if (shape < 0.0) shape = 0.0;
    return static_cast<int>(baseline + amplitude * shape);
}
} // namespace


SmmManager::SmmManager(QObject *parent)
    : QObject{parent}, m_serialPort(new QSerialPort(this)),m_saturation(0), m_pulseRate(0), m_isSignalWeak(false),m_beepVoice(false),m_averageSecond(AveragingSeconds::sec4)
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

    m_soundPlayer = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_soundPlayer->setAudioOutput(m_audioOutput);

    m_soundPlayer->setSource(QUrl("qrc:/sounds2/alarmSound2.wav"));
    m_soundPlayer->setLoops(QMediaPlayer::Infinite);
    m_audioOutput->setVolume(1.0f);

    m_muteTimer = new QTimer(this);
    m_muteTimer->setSingleShot(true);
    connect(m_muteTimer, &QTimer::timeout, this, &SmmManager::onMuteTimeout);

    m_demoWaveformTimer = new QTimer(this);
    connect(m_demoWaveformTimer, &QTimer::timeout, this, &SmmManager::updateDemoWaveform);

    m_udpSocket = new QUdpSocket(this); //memory leak olmaması için this veriyoruz

}
SmmManager::~SmmManager()
{
    if(m_serialPort->isOpen()){
        m_serialPort->close();
    }
}
QStringList SmmManager::availablePorts() const {
    QStringList ports;
    const QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos){
        ports.append(info.portName());
    }
    return ports;
}
void SmmManager::refreshPorts() {
    emit availablePortsChanged();
}

void SmmManager::connectToModule(const QString &portName) {

    if(m_serialPort->isOpen()) {
        m_serialPort->close();
    }

    m_buffer.clear();
    m_watchdogTimer->stop();
    m_reconnectTimer->stop();

    m_saturation = 0;
    m_pulseRate = 0;
    m_waveform = 0;
    emit saturationChanged(m_saturation);
    emit pulseRateChanged(m_pulseRate);
    emit waveformChanged(m_waveform);

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

        m_watchdogTimer->start(1000); // port açıldığında 1 saniyelik geri sayımı başlat
    } else {
        qDebug() << "The Port could not be opened!" << m_serialPort->errorString();

        if(m_isPortConnected) {
            m_isPortConnected = false;
            emit isPortConnectedChanged(m_isPortConnected);
        }
        if(!m_hasConnectionError) {
            m_hasConnectionError = true;
            emit hasConnectionErrorChanged(m_hasConnectionError);
        }
        if(!m_reconnectTimer->isActive()){
            m_reconnectTimer->start(2000);
        }
    }
    if(m_hasConnectionError) {
        m_hasConnectionError = false;
        emit hasConnectionErrorChanged(m_hasConnectionError);
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
        sendBiolightSpo2Setting(m_currentConfigByte);
        return;
    }

    char byte = static_cast<char>(toSend);
    m_serialPort->write(&byte, 1);
    m_serialPort->flush();
    qDebug() << "handshake byte sent -> 0x" + QString::number(toSend,16).toUpper();

    m_handshakeStep++;
    m_handshakeTimer->start(50); // her byte arasında 50 ms bekle
}

void SmmManager::disconnectPort() {
    if(m_serialPort->isOpen()) {
        m_serialPort->close();
    }
    m_watchdogTimer->stop();
    m_reconnectTimer->stop();
    m_buffer.clear();

    m_saturation = 0;
    m_pulseRate = 0;
    m_waveform = 0;

    m_beepVoice = false;
    m_isSignalWeak = false;
    m_pulseSearch = false;

    stopSoundEffect();

    emit saturationChanged(m_saturation);
    emit pulseRateChanged(m_pulseRate);
    emit waveformChanged(m_waveform);

    emit beepVoiceChanged(m_beepVoice);
    emit isSignalWeakChanged(m_isSignalWeak);
    emit pulseSearchChanged(m_pulseSearch);

    if(m_isPortConnected) {
        m_isPortConnected = false;
        emit isPortConnectedChanged(m_isPortConnected);
    }
    qDebug() << "Port manually disconnected and cleaned!";
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
    if(m_isSimulationMode){
        m_serialPort->readAll();
        return;
    }
    QByteArray rawData = m_serialPort->readAll();
    // Ham veriyi yazdıran qDebug kapatıldı. Böylece arayüz kilitlemeyecek.
    m_buffer.append(rawData);
    parseBuffer();
}

//3 saniye boyunca veri gelmezse çalışacak kurtarma fonksiyonu
void SmmManager::onWatchdogTimeout() {
    if(m_isDemoMode || m_isSimulationMode) {
        return;
    }

    if(m_saturation != 0 || m_pulseRate != 0 || m_isSignalWeak || m_waveform != 0){
        m_saturation = 0;
        m_pulseRate = 0;
        m_waveform = 0;
        m_isSignalWeak = false;
        m_beepVoice = false;
        m_pulseSearch = false;

        stopSoundEffect();

        emit saturationChanged(m_saturation); // Arayüzü "--" durumuna açık
        emit pulseRateChanged(m_pulseRate);
        emit waveformChanged(m_waveform);
        emit isSignalWeakChanged(m_isSignalWeak);
        emit beepVoiceChanged(m_beepVoice);
        emit pulseSearchChanged(m_pulseSearch);
    }

    if(!m_hasConnectionError) {
        m_hasConnectionError = true;
        emit hasConnectionErrorChanged(m_hasConnectionError);
    }
    qDebug() << "[WARNING] Data flow interrupted! Module is being awakened.";
    initializeBiolightModule(); //el sıkışma komutlarını baştan gönder
}

void SmmManager::initDatabase(){
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);

    QString dbPath = dataDir + "/SmmData.db";
    // DÜZELTME: Doğrudan "SmmData.db" yerine dbPath değişkenini kullanmalıyız!
    db.setDatabaseName(dbPath);
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
        qDebug() << "Measurements table error: " << query.lastError().text();
    }

    if(!query.exec("CREATE TABLE IF NOT EXISTS AlarmLogs ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "timestamp TEXT, "
                    "parameter_type TEXT, "
                    "value INTEGER, "
                    "priority TEXT)")) {
        qDebug() << "AlarmLogs table error: " << query.lastError().text();
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

    //eğer simülasyon modundaysa SQL kaydı yapmadan fonksiyondan çık
    if (m_isSimulationMode) {
        return;
    }

    QSqlQuery query;

    query.prepare("INSERT INTO measurements (timestamp, spo2, pulse_rate) VALUES (:timestamp, :spo2, :pulse_rate)");


    query.bindValue(":timestamp", QDateTime::currentDateTime().toString(Qt::ISODate));
    query.bindValue(":spo2", spo2);
    query.bindValue(":pulse_rate", pulseRate);

    if (!query.exec()) {
        qDebug() << "Could not be saved to the database:" << query.lastError().text();
    } else {
        refreshHistoryModel();
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
        qWarning() << "Port connection lost (Cable disconnected)!" << m_serialPort->errorString();

        if(m_serialPort->isOpen()){
            m_serialPort->close();
        }
        if(m_isPortConnected) {
            m_isPortConnected = false;
            emit isPortConnectedChanged(m_isPortConnected);
        }

        if (m_reconnectTimer && !m_reconnectTimer->isActive()) {
            m_isReconnectLogPrinted = false;
            m_reconnectTimer->start(2000);
        }
    }
}
void SmmManager::tryReconnect(){
    if(m_lastPortName.isEmpty()){
        return;
    }

    m_serialPort->setPortName(m_lastPortName);
    if(m_serialPort->open(QIODevice::ReadWrite)){
        qDebug() << "The SMM modul has been successfully connected: " << m_lastPortName;

        m_isReconnectLogPrinted = false;

        if(m_reconnectTimer && m_reconnectTimer->isActive()){
            m_reconnectTimer->stop();
        }

        if(!m_isPortConnected) {
            m_isPortConnected = true;
            emit isPortConnectedChanged(m_isPortConnected);
        }
        initializeBiolightModule();
        m_watchdogTimer->start(1000);
    }else {
        if(!m_isReconnectLogPrinted){
            qWarning() << "The port could not be opened!" << m_serialPort->errorString();
            qDebug() << "The port is being rescanning: " << m_lastPortName;

            m_isReconnectLogPrinted = true;
        }
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

void SmmManager::setFrequency(int freq){
    if(m_frequency == freq)
        return;

    m_frequency = freq;
    emit frequencyChanged(m_frequency);

    m_currentConfigByte &= ~0x003;

    if(freq == 50) {
        m_currentConfigByte |= 0x02;
    } else if(freq == 60){
        m_currentConfigByte |= 0x03;
    }

    sendBiolightSpo2Setting(m_currentConfigByte);
    qDebug() << "Frequency setting changed to " << "Hz. New Byte: " << QString::number(m_currentConfigByte, 16).toUpper();
}

void SmmManager::parseIncomingData(const QByteArray &data) {
    if(data.size() < 9){
        qWarning() << "The incoming data is incomplete or incorrect.";
        return;
    }
    quint8 modeValue= static_cast<quint8>(data.at(6));

    PatientMode parsedMode;
    switch (modeValue) {
    case 0:
        parsedMode = PatientMode::Adult;
        break;
    case 1:
        parsedMode = PatientMode::Newborn;
        break;
    case 2:
        parsedMode = PatientMode::Pediatric;
        break;
    default:
        return;
    }

    if(m_patientMode != parsedMode){
        m_patientMode = parsedMode;
        emit patientModeChanged(m_patientMode);
    }
    //spo2 için dinamik limit ve log kontrolü
    if(m_saturation > 0) {
        if(m_saturation < m_spo2LowerLimit || m_saturation > m_spo2UpperLimit) {
            QString priorityStr = "Red";
            if(m_spo2AlarmPriority == AlarmPriority::Yellow) priorityStr = "Yellow";
            else if(m_spo2AlarmPriority == AlarmPriority::Blue) priorityStr = "Blue";

            logAlarm("SpO2", m_saturation, priorityStr);
        }
    }
    if(m_pulseRate > 0) {
        if(m_pulseRate < m_pulseLowerLimit || m_pulseRate > m_pulseUpperLimit) {
            QString priorityStr = "Red";
            if(m_pulseAlarmPriority == AlarmPriority::Yellow) priorityStr = "Yellow";
            else if(m_pulseAlarmPriority == AlarmPriority::Blue) priorityStr = "Blue";

            logAlarm("Pulse", m_pulseRate, priorityStr);
        }
    }
}

QByteArray SmmManager::updatePatientModeInPacket(QByteArray currentPacket, PatientMode newMode){
    if(currentPacket.isEmpty()){
        currentPacket.append(static_cast<char>(0x00));
    }
    quint8 byteData = static_cast<quint8>(currentPacket.at(0));

    byteData &= 0xE3;

    byteData |= (static_cast<quint8>(newMode) << 2);

    currentPacket[0] = static_cast<char>(byteData);
    return currentPacket;
}

void SmmManager::setPatientMode(PatientMode mode){
    //mod aynı ise gereksiz bildirim gönderme
    if(m_patientMode == mode) {
        return;
    }
    //durumu güncelle ve qmle bildir
    m_patientMode = mode;
    emit patientModeChanged(m_patientMode);

    //cihaza gönderilecek veri hazılransın
    QByteArray commandPacket;
    commandPacket.append(static_cast<char>(m_currentConfigByte));

    //paketi yeni mode göre güncelle
    commandPacket = updatePatientModeInPacket(commandPacket, m_patientMode);

    m_currentConfigByte =  static_cast<quint8>(commandPacket.at(0));

    if(m_serialPort && m_serialPort->isOpen()){
        m_serialPort->write(commandPacket);
        qDebug() << "Patient Mode Setting Sent to the Module (Hex): " << commandPacket.toHex();
    } else{
        qWarning() << "The serial port is closed, mode setting could not be sent.";
    }
}
void SmmManager::setAverageSecond(AveragingSeconds seconds) {
    if(m_averageSecond == seconds)
        return;

    m_averageSecond = seconds;
    emit averageSecondChanged(m_averageSecond);

    m_currentConfigByte &= ~0xE0;

    quint8 bitsVal = 0;

    if(seconds == AveragingSeconds::sec4) {
        bitsVal = 0x4;
    }else if(seconds == AveragingSeconds::sec8){
        bitsVal = 0x5;
    }else if(seconds == AveragingSeconds::sec16) {
        bitsVal = 0x6;
    }
    m_currentConfigByte |= (bitsVal << 5);

    sendBiolightSpo2Setting(m_currentConfigByte);
    qDebug() << "Averaging Second changed. New Config Byte (Hex):" << QString::number(m_currentConfigByte, 16).toUpper();

}
void SmmManager::setSpo2LowerLimit (int limit) {
    if(m_spo2LowerLimit == limit)
        return;

    m_spo2LowerLimit = limit;
    emit spo2LowerLimitChanged();
    qDebug() << "SpO2 Lower Limit has been updated:" << m_spo2LowerLimit;
}

void SmmManager::setSpo2UpperLimit(int  limit) {
    if(m_spo2UpperLimit == limit)
        return;

    m_spo2UpperLimit = limit;
    emit spo2UpperLimitChanged();
    qDebug() << "SpO2 Upper Limit has been updated:" << m_spo2UpperLimit;
}
void SmmManager::setSpo2AlarmPriority(AlarmPriority priority) {
    if(m_spo2AlarmPriority == priority) {
        return;
    }
    m_spo2AlarmPriority = priority;
    emit spo2AlarmPriorityChanged(m_spo2AlarmPriority);

    qDebug() << "SpO2 Alarm Priority has been updated. New value:" << static_cast<int>(priority);
}
void SmmManager::setPulseLowerLimit(int limit) {
    if(m_pulseLowerLimit == limit) {
        return;
    }
    m_pulseLowerLimit = limit;
    emit pulseLowerLimitChanged();
    qDebug() << "Pulse Lower Limit has been updated:" << m_pulseLowerLimit;
}

void SmmManager::setPulseUpperLimit(int limit) {
    if(m_pulseUpperLimit == limit)
        return;

    m_pulseUpperLimit = limit;
    emit pulseUpperLimitChanged();

    qDebug() << "Pulse Upper Limit has been updated:" << m_pulseUpperLimit;
}

void SmmManager::setPulseAlarmPriority(AlarmPriority priority) {
    if(m_pulseAlarmPriority == priority)
        return;
    m_pulseAlarmPriority = priority;
    emit pulseAlarmPriorityChanged();
    qDebug() << "Pulse Alarm Priority has bee updated.";
}

QSqlQueryModel *SmmManager::getAlarmLogsModel() {
    struct AlarmRoleModel : public QSqlQueryModel {
        QHash<int,QByteArray> roleNames() const override {
            QHash<int, QByteArray> roles;
            roles[Qt::UserRole + 1] = "timestamp";
            roles[Qt::UserRole + 2] = "parameter_type";
            roles[Qt::UserRole + 3] = "value";
            roles[Qt::UserRole + 4] = "priority";
            return roles;
        }
        QVariant data(const QModelIndex &index, int role) const override {
            if(!index.isValid()) return QVariant();
            int column = -1;
            switch (role) {
            case Qt::UserRole + 1: column = 0; break;
            case Qt::UserRole + 2: column = 1;break;
            case Qt::UserRole + 3: column = 2; break;
            case Qt::UserRole + 4: column = 3; break;
            }
            QModelIndex sourceIndex = this->index(index.row(), column);
            return QSqlQueryModel::data(sourceIndex, Qt::DisplayRole);
        }
    };

    QSqlQueryModel *model = new AlarmRoleModel();
    model->setParent(this);
    model->setQuery("SELECT timestamp, parameter_type, value, priority FROM AlarmLogs ORDER BY id DESC");

    if (model->lastError().isValid()) {
        qDebug() << "Model Error:" << model->lastError().text();
    }

    return model;
}



void SmmManager::logAlarm(const QString &paramType, int value, const QString &priority) {
    if(m_isDemoMode) {
        return;
    }

    // SpO2 ve Pulse için ayrı ayrı zaman tutucular
    static qint64 lastSpo2LogTime = 0;
    static qint64 lastPulseLogTime = 0;
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();

    if (paramType == "SpO2") {
        if (currentTime - lastSpo2LogTime < 2000) return;
        lastSpo2LogTime = currentTime;
    } else if (paramType == "Pulse") {
        if (currentTime - lastPulseLogTime < 2000) return;
        lastPulseLogTime = currentTime;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO AlarmLogs (timestamp, parameter_type, value, priority) "
                  "VALUES (:timestamp, :parameter_type, :value, :priority)");

    query.bindValue(":timestamp", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    query.bindValue(":parameter_type", paramType);
    query.bindValue(":value", value);
    query.bindValue(":priority", priority);

    if(!query.exec()) {
        qDebug() << "Alarm log could not be written:" << query.lastError().text();
    } else {
        qDebug() << "SUCCESS! Alarm saved to DB:" << paramType << value << priority;
    }
}

void SmmManager::playSoundEffect(SoundType type) {
    QString soundPath;

    switch(type) {
    case SoundType::Alarm :
        soundPath = "qrc:/sounds2/lowAlarm.wav";
        m_soundPlayer->setLoops(QMediaPlayer::Infinite);
        break;
    case SoundType::Warning:
        soundPath = "qrc:/sounds2/warningSound.wav";
        m_soundPlayer->setLoops(1);
        break;
    case SoundType::Info :
        soundPath = "qrc:/sounds2/infoSound.wav";
        m_soundPlayer->setLoops(1);
        break;
    }
    if(m_soundPlayer->source() != QUrl(soundPath)) {
        m_soundPlayer->setSource(QUrl(soundPath));
    }
    if(!m_soundPlayer->isPlaying()) {
        m_soundPlayer->play();
    }
}

void SmmManager::stopSoundEffect() {
    if(m_soundPlayer->isPlaying()) {
        m_soundPlayer->stop();
    }
}

void SmmManager::muteAlarmForTwoMinutes() {
    m_isAlarmMuted = true;
    emit isAlarmMutedChanged(m_isAlarmMuted);

    stopSoundEffect();

    m_muteTimer->start(120000);
    qDebug() << "Alarms are muted for 2 minutes.";
}

void SmmManager::onMuteTimeout() {
    m_isAlarmMuted = false;
    emit isAlarmMutedChanged(m_isAlarmMuted);

    qDebug() << "Mute duration expired. Alarms are active again.";
}

void SmmManager::updateAlarmSound() {
    if(m_isAlarmMuted) {
        stopSoundEffect();
        return;
    }
    if(!m_isSpo2AlarmActive && !m_isPulseAlarmActive) {
        stopSoundEffect();
        return;
    }
    //varsayılan olarak en düşük önceliği atıyoru<
    AlarmPriority highestPriority = AlarmPriority::Blue;

    if (m_isSpo2AlarmActive) {
        highestPriority = m_spo2AlarmPriority;
    }

    if(m_isPulseAlarmActive) {
        if(m_pulseAlarmPriority == AlarmPriority::Red) {
            highestPriority = AlarmPriority::Red;
        } else if(m_pulseAlarmPriority == AlarmPriority::Yellow && highestPriority != AlarmPriority::Red) {
            highestPriority = AlarmPriority::Yellow;
        }
    }

    QString soundPath;
    if(highestPriority == AlarmPriority::Red) {
        soundPath = "qrc:/sounds2/highAlarm.wav";
    } else if(highestPriority == AlarmPriority::Yellow) {
        soundPath = "qrc:/sounds2/mediumAlarm.wav";
    } else {
        soundPath = "qrc:/sounds2/lowAlarm.wav";
    }

    if(m_soundPlayer->source() != QUrl(soundPath)) {
        m_soundPlayer->setSource(QUrl(soundPath));
        m_soundPlayer->setLoops(QMediaPlayer::Infinite);
        m_soundPlayer->play();
    } else if(!m_soundPlayer->isPlaying() && m_soundPlayer->mediaStatus() != QMediaPlayer::InvalidMedia) {
        m_soundPlayer->play();
    }
}

void SmmManager::setWaveformSpeed(int speed) {
    if(speed != 25 && speed != 50) return;
    if(m_waveformSpeed == speed) return;
    m_waveformSpeed = speed;
    emit waveformSpeedChanged(m_waveformSpeed);



    qDebug() << "Waveform speed updated to:" << m_waveformSpeed << " m/s";
}

void SmmManager::exportDataToPdf() {
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString filePath = desktopPath + "/SmmMonitor_Alarm_Report.pdf";

    QPdfWriter pdfWriter(filePath);
    pdfWriter.setPageSize(QPageSize::A4);
    pdfWriter.setResolution(300);

    QPainter painter(&pdfWriter);
    if (painter.isActive()) {
        int pageWidth = pdfWriter.width();

        // 1. Ana Başlık (Sol Üst - Boyut ve konum ayarlandı)
        QFont titleFont = painter.font();
        titleFont.setPointSize(14);
        titleFont.setBold(true);
        painter.setFont(titleFont);
        painter.drawText(200, 300, "SMM Monitor - Alarm Logs Report");

        // 2. Otomatik Güncel Tarih (Sağ Üst Köşe - Çakışmayacak güvenli koordinat)
        QFont dateFont = painter.font();
        dateFont.setPointSize(10);
        dateFont.setBold(false);
        painter.setFont(dateFont);

        QString currentDate = "Report Date: " + QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm");
        painter.drawText(pageWidth - 750, 300, currentDate);

        // Ayırıcı Çizgi
        painter.drawLine(200, 400, pageWidth - 200, 400);

        // 3. Alt Başlık ve Açıklama
        QFont subFont = painter.font();
        subFont.setPointSize(11);
        subFont.setBold(true);
        painter.setFont(subFont);
        painter.drawText(200, 480, "Alarm Geçmişi Kayıtları (AlarmLogs)");

        int startY = 550;
        subFont.setPointSize(10);
        painter.setFont(subFont);
        painter.drawText(200, startY, "Zaman");
        painter.drawText(900, startY, "Parametre");
        painter.drawText(1400, startY, "Değer");
        painter.drawText(1800, startY, "Öncelik (Priority)");

        painter.drawLine(200, startY + 40, pageWidth - 200, startY + 40);

        // Veritabanından AlarmLogs verilerini çekiyoruz (Hem SpO2 hem Pulse dahil)
        QSqlQuery alarmQuery("SELECT timestamp, parameter_type, value, priority FROM AlarmLogs ORDER BY id DESC LIMIT 20");
        int rowY = startY + 100;

        QFont rowFont = painter.font();
        rowFont.setPointSize(9);
        rowFont.setBold(false);
        painter.setFont(rowFont);

        while (alarmQuery.next() && rowY < 3200) {
            QString time = alarmQuery.value(0).toString();
            QString param = alarmQuery.value(1).toString();
            int val = alarmQuery.value(2).toInt();
            QString priority = alarmQuery.value(3).toString();

            painter.drawText(200, rowY, time);
            painter.drawText(900, rowY, param);
            painter.drawText(1400, rowY, QString::number(val));
            painter.drawText(1800, rowY, priority);

            rowY += 80;
        }

        painter.end();
        qDebug() << "Alarm PDF raporu başarıyla oluşturuldu:" << filePath;
    }
}

void SmmManager::exportDataToExcel() {
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString filePath = desktopPath + "/SmmMonitor_Measurements.csv";

    QFile file(filePath);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);

        // Türkçe Excel karakter bozulmasını önlemek için UTF-8 BOM
        out.setEncoding(QStringConverter::Utf8);
        out << "\xEF\xBB\xBF";

        // Excel sütun başlıkları (Noktalı virgül kullanıyoruz)
        out << "Zaman;SpO2 (%);Pulse Rate (bpm)\n";

        // Veritabanından measurements tablosundaki verileri çekiyoruz
        QSqlQuery query("SELECT timestamp, spo2, pulse_rate FROM measurements ORDER BY id DESC");
        while (query.next()) {
            QString timestamp = query.value(0).toString();
            int spo2 = query.value(1).toInt();
            int pulse = query.value(2).toInt();

            out << timestamp << ";" << spo2 << ";" << pulse << "\n";
        }

        file.close();
        qDebug() << "Excel (CSV) dosyası Masaüstüne kaydedildi:" << filePath;
    }
}
void SmmManager::injectTestData(int spo2, int pulse, bool isSignalWeak, bool isPulseSearching) {
    if(!m_isPortConnected) {
        m_isPortConnected = true;
        emit isPortConnectedChanged(m_isPortConnected);
    }

    if(isPulseSearching) {
        spo2 = 0;
        pulse = 0;
    }
    if(m_pulseSearch != isPulseSearching) {
        m_pulseSearch = isPulseSearching;
        emit pulseSearchChanged(m_pulseSearch);
    }
    if(m_isSignalWeak != isSignalWeak) {
        m_isSignalWeak = isSignalWeak;
        emit isSignalWeakChanged(m_isSignalWeak);
    }

    if(m_saturation != spo2) {
        m_saturation = spo2;
        emit saturationChanged(m_saturation);
    }

    if(m_pulseRate != pulse) {
        m_pulseRate = pulse;
        emit pulseRateChanged(m_pulseRate);
    }

    bool currentSpo2AlarmState = false;
    if(m_saturation > 0 && (m_saturation < m_spo2LowerLimit || m_saturation > m_spo2UpperLimit)) {
        currentSpo2AlarmState = true;
        QString priorityStr = "Red";
        if(m_spo2AlarmPriority == AlarmPriority::Yellow) priorityStr = "Yellow";
        else if(m_spo2AlarmPriority == AlarmPriority::Blue) priorityStr = "Blue";

        logAlarm("SpO2 (Demo)", m_saturation, priorityStr);
    }

    if(m_isSpo2AlarmActive != currentSpo2AlarmState) {
        m_isSpo2AlarmActive = currentSpo2AlarmState;
        emit isSpo2AlarmActiveChanged(m_isSpo2AlarmActive);
    }

    bool currentPulseState = false;
    if(m_pulseRate > 0 && (m_pulseRate < m_pulseLowerLimit || m_pulseRate > m_pulseUpperLimit)) {
        currentPulseState = true;

        QString priorityStr = "Red";
        if(m_pulseAlarmPriority == AlarmPriority::Yellow) priorityStr = "Yellow";
        else if(m_pulseAlarmPriority == AlarmPriority::Blue) priorityStr = "Blue";

        logAlarm("Pulse (Demo)", m_pulseRate, priorityStr);
    }

    if(m_isPulseAlarmActive != currentPulseState) {
        m_isPulseAlarmActive = currentPulseState;
        emit isPulseAlarmActiveChanged();
    }

    updateAlarmSound();
}

void SmmManager::setDemoMode(bool isDemo) {
    m_isDemoMode = isDemo;
    m_isSimulationMode = isDemo;

    if(isDemo) {
        if(m_watchdogTimer && m_watchdogTimer->isActive()) {
            m_watchdogTimer->stop();
        }
        if(m_demoWaveformTimer && !m_demoWaveformTimer->isActive()) {
            m_demoWaveformTimer->start(40);
        }
    } else {
        if(m_demoWaveformTimer && m_demoWaveformTimer->isActive()) {
            m_demoWaveformTimer->stop();
        }
    }
}
void SmmManager::updateDemoWaveform() {
    if(!m_isSimulationMode) return;
    if(m_pulseSearch) {
        m_waveform = 0;
        emit waveformChanged(m_waveform);
        return;
    }
    static double s_wavePhase = 0.0;
    const double beatsPerSecond = m_pulseRate > 0 ? (m_pulseRate / 60.0) : 1.0;
    const double phaseStep = beatsPerSecond / 25.0;

    s_wavePhase += phaseStep;
    if(s_wavePhase >= 1.0) s_wavePhase -= 1.0;

    m_waveform = generateRealisticPpgSample(s_wavePhase);
    emit waveformChanged(m_waveform);

    bool currentBeep = false;
    if(s_wavePhase < 0.15) {
        currentBeep = true;
    }
    if(m_beepVoice != currentBeep) {
        m_beepVoice = currentBeep;
        emit beepVoiceChanged(m_beepVoice);
    }

    sendUdpData();
}

void SmmManager::parseBuffer(){
    const quint8 BIOLIGHT_CODE = 21; //0x15

    QByteArray headerBytes;
    headerBytes.append(static_cast<char>(0xAA));
    headerBytes.append(static_cast<char>(0x55));

    while(true) {
        // 1. OPTİMİZASYON: Yavaş for döngüsü yerine indexOf ile anında bulma (İşlemciyi rahatlatır)
        int headerIndex = m_buffer.indexOf(headerBytes);

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
            // 2. OPTİMİZASYON: Hatalı pakette AA 55'i komple sil ki sonsuz döngüye girmesin
            m_buffer.remove(0, 2);
            continue;
        }

        if(code == BIOLIGHT_CODE && data.size() >= 9){
            m_watchdogTimer->start(1000);

            if (m_hasConnectionError) {
                m_hasConnectionError = false;
                emit hasConnectionErrorChanged(m_hasConnectionError);
            }

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
                emit isSignalWeakChanged(m_isSignalWeak);
            }

            const quint8 rawSpo2 = static_cast<quint8>(data[3]);
            const quint8 prMsb = static_cast<quint8>(data[4]);
            const quint8 prLsb = static_cast<quint8>(data[5]);
            int rawPulseRate = (prMsb << 8 | prLsb);
            const quint8 rawWaveform = static_cast<quint8>(data[1]);

            if(inSensorOff || rawSpo2 == 127 || rawPulseRate == 255 || rawWaveform == 127){
                if(m_saturation != 0) {
                    m_saturation = 0;
                    emit saturationChanged(m_saturation);
                }
                if(m_pulseRate != 0){
                    m_pulseRate = 0;
                    emit pulseRateChanged(m_pulseRate);
                }
                if(m_waveform != 0){
                    m_waveform = 0;
                    emit waveformChanged(m_waveform);
                }
                if(m_isSpo2AlarmActive) {
                    m_isSpo2AlarmActive = false;
                    emit isSpo2AlarmActiveChanged(m_isSpo2AlarmActive);
                }
                if(m_isPulseAlarmActive) {
                    m_isPulseAlarmActive = false;
                    emit isPulseAlarmActiveChanged();
                }
                stopSoundEffect();

                m_waveform = 0;
                emit waveformChanged(m_waveform);

                sendUdpData();

            } else {
                if(m_saturation != rawSpo2 || m_pulseRate != rawPulseRate ) {
                    m_saturation = rawSpo2;
                    m_pulseRate = rawPulseRate;

                    emit saturationChanged(m_saturation);
                    emit pulseRateChanged(m_pulseRate);
                }

                // --- 1. SpO2 ALARM KONTROLÜ (Sürekli kontrol eder, logAlarm 2 saniyede bir filtreler) ---
                bool currentSpo2AlarmState = false;
                if(m_saturation > 0) {
                    if(m_saturation < m_spo2LowerLimit || m_saturation > m_spo2UpperLimit) {
                        currentSpo2AlarmState = true;

                        QString priorityStr = "Red";
                        if(m_spo2AlarmPriority == AlarmPriority::Yellow) priorityStr = "Yellow";
                        else if(m_spo2AlarmPriority == AlarmPriority::Blue) priorityStr = "Blue";

                        logAlarm("SpO2", m_saturation, priorityStr);
                    }
                }

                // Alarm durumu değiştiyse arayüze bildir (Kırmızı yanıp sönme vs.)
                if(m_isSpo2AlarmActive != currentSpo2AlarmState) {
                    m_isSpo2AlarmActive = currentSpo2AlarmState;
                    emit isSpo2AlarmActiveChanged(m_isSpo2AlarmActive);
                }


                // --- 2. PULSE ALARM KONTROLÜ ---
                bool currentPulseAlarmState = false;
                if(m_pulseRate > 0) {
                    if(m_pulseRate < m_pulseLowerLimit || m_pulseRate > m_pulseUpperLimit) {
                        currentPulseAlarmState = true;

                        QString priorityStr = "Red";
                        if(m_pulseAlarmPriority == AlarmPriority::Yellow) priorityStr = "Yellow";
                        else if(m_pulseAlarmPriority == AlarmPriority::Blue) priorityStr = "Blue";

                        logAlarm("Pulse", m_pulseRate, priorityStr);
                    }
                }


                if(m_isPulseAlarmActive != currentPulseAlarmState) {
                    m_isPulseAlarmActive = currentPulseAlarmState;
                    emit isPulseAlarmActiveChanged();
                }

                updateAlarmSound();

                // --- 3. NORMAL ÖLÇÜM GEÇMİŞİ KAYDI (2 saniyede bir) ---
                qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
                if (currentTime - m_lastDbSaveTime > 2000) {
                    insertMeasurement(rawSpo2, rawPulseRate);
                    m_lastDbSaveTime = currentTime;
                }

                m_waveform = rawWaveform;
                emit waveformChanged(m_waveform);

                sendUdpData();
            }
        }
        m_buffer.remove(0, totalPacketSize);
    }
}

void SmmManager::setTargetIp(const QString &ipAddress) {
    if(ipAddress.isEmpty()) {
        m_targetIp.clear();
        qDebug() << "UDP Hedef IP temizlendi. Veri gönderimi durduruldu.";
    } else {
        m_targetIp = QHostAddress(ipAddress);
        qDebug() << "UDP Hedef Ip ayarlandı:" << m_targetIp.toString() << "Port: " << m_targetPort;
    }
}


void SmmManager::sendUdpData() {
    if(m_targetIp.isNull()) return;

    QByteArray datagram;
    QDataStream out(&datagram, QIODevice::WriteOnly);


    out.setVersion(QDataStream::Qt_6_0);
    out << m_saturation << m_pulseRate << m_waveform;

    m_udpSocket->writeDatagram(datagram, m_targetIp, m_targetPort);

    qDebug() << "UDP paketi gönderiliyor... SpO2: " << m_saturation;
}