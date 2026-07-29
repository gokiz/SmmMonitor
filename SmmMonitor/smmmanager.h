#ifndef SMMMANAGER_H
#define SMMMANAGER_H

#include <QObject>
#include <QSerialPort>
#include <QByteArray>
#include <QTimer>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlQueryModel>


class SmmManager : public QObject
{
    Q_OBJECT
    // QML tarafında bu property'i okuyup Rectangle içinde göstereceğiz
    Q_PROPERTY(int saturation READ saturation NOTIFY saturationChanged)
    Q_PROPERTY(int pulseRate READ pulseRate  NOTIFY pulseRateChanged )
    Q_PROPERTY(bool isSignalWeak READ isSignalWeak  NOTIFY isSignalWeakChanged)
    Q_PROPERTY(bool beepVoice READ beepVoice NOTIFY beepVoiceChanged)
    Q_PROPERTY(bool pulseSearch READ pulseSearch NOTIFY pulseSearchChanged )
    Q_PROPERTY(bool isPortConnected READ isPortConnected NOTIFY isPortConnectedChanged)
    Q_PROPERTY(int waveform READ waveform NOTIFY waveformChanged )
    Q_PROPERTY(int frequency READ frequency WRITE setFrequency NOTIFY frequencyChanged )

    Q_PROPERTY(PatientMode patientMode READ patientMode WRITE setPatientMode NOTIFY patientModeChanged )

public:
    explicit SmmManager(QObject *parent = nullptr);
    ~SmmManager();

    enum class PatientMode : quint8 {
        Adult = 4,
        Newborn = 5,
        Pediatric = 6
    };
    Q_ENUM(PatientMode);

    int saturation() const { return m_saturation; }
    int pulseRate() const {return m_pulseRate; }
    bool isSignalWeak() const {return m_isSignalWeak; }
    bool beepVoice() const {return m_beepVoice; }
    bool pulseSearch() const {return m_pulseSearch; }
    bool isPortConnected() const {return m_isPortConnected;}
    int waveform() const {return m_waveform;}
    int frequency() const {return m_frequency; }

    PatientMode patientMode() const {return m_patientMode; }
    //UART bağlantısını başlatma fonksiyonu
    Q_INVOKABLE void connectToModule(const QString &portName);
    // İsim aynı kaldı ama artık gerçek "eski protokolden yeniye geçiş"
    // el sıkışmasını yapıyor (0xBF,0x5F,0xFF), 0xB2 gibi dokümanda
    // olmayan bir komut değil.
    Q_INVOKABLE void initializeBiolightModule();    // başlatma komutu fonksiyonu
    Q_INVOKABLE QSqlQueryModel *getHistoryModel();
    Q_INVOKABLE void filterHistoryByDate(const QString &startDate, const QString &endDate);
    Q_INVOKABLE void clearFilter();
    Q_INVOKABLE void clearHistory();
    Q_INVOKABLE void deleteHistoryByDateRange(const QString &startDate, const QString &endDate);
    Q_INVOKABLE void setFrequency(int freq);
    Q_INVOKABLE void setPatientMode(PatientMode mode);



    QByteArray updatePatientModeInPacket(QByteArray currentPacket, PatientMode newMode);

public slots:
    void injectTestData(const QByteArray &data) {
        m_buffer.append(data);
        parseBuffer();
    }
    void setSimulationMode(bool isSimulating) {
        m_isSimulationMode = isSimulating;
    }
    void parseIncomingData(const QByteArray &data);


signals:
    void saturationChanged(int newSaturation);
    void pulseRateChanged(int newPulseRate);
    void isSignalWeakChanged(bool isSignalWeak);
    void beepVoiceChanged(bool beepVoice);
    void pulseSearchChanged(bool pulseSearch);
    void isPortConnectedChanged(bool isPortConnected);
    void waveformChanged(int newWaveform);
    void frequencyChanged(int newFrequency);
    void patientModeChanged(SmmManager::PatientMode mode);


private slots:
    void readData(); //UART'a veri geldikçe tetiklenir
    void sendNextHandshakeByte();
    void onWatchdogTimeout(); // Modül uyuduğunda tetiklenecek
private:
    void parseBuffer(); //gelen baytları SMM protokolüne göre işler
    static quint8 calcChecksum(quint8 len, quint8 code, const QByteArray &data);
    void sendBiolightSpo2Setting(quint8 configByte = 0xB2);

    QSerialPort *m_serialPort;
    QByteArray m_buffer;
    int m_saturation;
    int m_pulseRate;
    bool m_isSignalWeak = false;
    bool m_beepVoice = false;
    bool m_pulseSearch = false;
    int m_waveform = 0;

    PatientMode m_patientMode = PatientMode::Adult; //varsayılan hasta yetişkin

    void handlePortError(QSerialPort::SerialPortError error);
    void tryReconnect();
    bool m_isPortConnected = false;
    QTimer *m_reconnectTimer;
    QString m_lastPortName;

    QSqlQueryModel *m_historyModel = nullptr;

    QTimer *m_handshakeTimer;
    int m_handshakeStep = 0;

    QTimer *m_watchdogTimer; // veri akışını denetleyecek zamanlayıcı
    void initDatabase();
    void insertMeasurement(int spo2, int pulseRate);
    void refreshHistoryModel();

    bool m_isFilterActive = false;
    QString m_filterStartDate;
    QString m_filterEndDate;

    bool m_isSimulationMode = false;

    int m_frequency = 50;
    quint8 m_currentConfigByte = 0xB2;

    bool m_isReconnectLogPrinted = false;
};

#endif // SMMMANAGER_H