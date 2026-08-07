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
#include <QtMultimedia/QSoundEffect>
#include <QMediaPlayer>
#include <QAudioOutput>

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
    Q_PROPERTY(AveragingSeconds averageSecond READ averageSecond WRITE setAverageSecond NOTIFY averageSecondChanged)
    Q_PROPERTY(QStringList availablePorts READ availablePorts  NOTIFY availablePortsChanged)
    Q_PROPERTY(bool hasConnectionError READ hasConnectionError NOTIFY hasConnectionErrorChanged )

    Q_PROPERTY(int spo2LowerLimit READ spo2LowerLimit WRITE setSpo2LowerLimit NOTIFY spo2LowerLimitChanged )
    Q_PROPERTY(int spo2UpperLimit READ spo2UpperLimit WRITE setSpo2UpperLimit NOTIFY spo2UpperLimitChanged )

    Q_PROPERTY(bool isSpo2AlarmActive READ isSpo2AlarmActive NOTIFY isSpo2AlarmActiveChanged )
    Q_PROPERTY(AlarmPriority spo2AlarmPriority READ spo2AlarmPriority WRITE setSpo2AlarmPriority NOTIFY spo2AlarmPriorityChanged )

    Q_PROPERTY(int pulseLowerLimit READ pulseLowerLimit WRITE setPulseLowerLimit NOTIFY pulseLowerLimitChanged )
    Q_PROPERTY(int pulseUpperLimit READ pulseUpperLimit WRITE setPulseUpperLimit NOTIFY pulseUpperLimitChanged )
    Q_PROPERTY(bool isPulseAlarmActive READ isPulseAlarmActive NOTIFY isPulseAlarmActiveChanged )
    Q_PROPERTY(AlarmPriority pulseAlarmPriority READ pulseAlarmPriority WRITE setPulseAlarmPriority NOTIFY pulseAlarmPriorityChanged )

    Q_PROPERTY(bool isAlarmMuted READ isAlarmMuted NOTIFY isAlarmMutedChanged)

    Q_PROPERTY(int waveformSpeed READ waveformSpeed WRITE setWaveformSpeed NOTIFY waveformSpeedChanged )

public:
    explicit SmmManager(QObject *parent = nullptr);
    ~SmmManager();

    enum class PatientMode : quint8 {
        Adult = 4,
        Newborn = 5,
        Pediatric = 6
    };
    Q_ENUM(PatientMode);

    enum class AveragingSeconds : quint8 {
        sec4 = 4,
        sec8 = 8,
        sec16 = 16
    };
    Q_ENUM(AveragingSeconds);

    enum class AlarmPriority {
        Blue = 0,
        Yellow,
        Red
    };
    Q_ENUM(AlarmPriority);

    enum class SoundType {
        Alarm,
        Warning,
        Info
    };
    Q_ENUM(SoundType);

    int saturation() const { return m_saturation; }
    int pulseRate() const {return m_pulseRate; }
    bool isSignalWeak() const {return m_isSignalWeak; }
    bool beepVoice() const {return m_beepVoice; }
    bool pulseSearch() const {return m_pulseSearch; }
    bool isPortConnected() const {return m_isPortConnected;}
    int waveform() const {return m_waveform;}
    int frequency() const {return m_frequency; }

    // Bu satırların public bloğu altında olduğundan emin ol
    int spo2LowerLimit() const { return m_spo2LowerLimit; }
    int spo2UpperLimit() const { return m_spo2UpperLimit; }
    bool isSpo2AlarmActive() const {return m_isSpo2AlarmActive;}
    AlarmPriority spo2AlarmPriority() const {return m_spo2AlarmPriority;}

    int pulseLowerLimit() const {return m_pulseLowerLimit;}
    int pulseUpperLimit() const {return m_pulseUpperLimit;}
    bool isPulseAlarmActive() const {return m_isPulseAlarmActive;}
    AlarmPriority pulseAlarmPriority() const {return m_pulseAlarmPriority;}

    bool isAlarmMuted() const {return m_isAlarmMuted;}

    int waveformSpeed() const {return m_waveformSpeed;}


    PatientMode patientMode() const {return m_patientMode; }
    AveragingSeconds averageSecond() const { return m_averageSecond;}
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
    Q_INVOKABLE void setAverageSecond (AveragingSeconds seconds);
    Q_INVOKABLE void refreshPorts();
    Q_INVOKABLE QStringList availablePorts() const;

    Q_INVOKABLE void disconnectPort();

    Q_INVOKABLE void setSpo2LowerLimit(int limit);
    Q_INVOKABLE void setSpo2UpperLimit(int limit);

    Q_INVOKABLE void setSpo2AlarmPriority(AlarmPriority priority);

    Q_INVOKABLE void setPulseLowerLimit(int limit);
    Q_INVOKABLE void setPulseUpperLimit(int limit);
    Q_INVOKABLE void setPulseAlarmPriority(AlarmPriority priority);

    Q_INVOKABLE QSqlQueryModel *getAlarmLogsModel();

    Q_INVOKABLE void playSoundEffect(SoundType type);
    Q_INVOKABLE void stopSoundEffect();
    Q_INVOKABLE void muteAlarmForTwoMinutes();

    Q_INVOKABLE void setWaveformSpeed(int speed);

    Q_INVOKABLE void exportDataToPdf();
    Q_INVOKABLE void exportDataToExcel();

    Q_INVOKABLE void setDemoMode(bool isDemo);




    QByteArray updatePatientModeInPacket(QByteArray currentPacket, PatientMode newMode);

public slots:
    void injectTestData(int spo2, int pulseRate, bool isSignalWeak = false, bool isPulseSearching = false);
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
    void averageSecondChanged(SmmManager::AveragingSeconds seconds);
    void availablePortsChanged();
    void hasConnectionErrorChanged(bool hasError);
    void spo2LowerLimitChanged();
    void spo2UpperLimitChanged();
    void isSpo2AlarmActiveChanged(bool active);
    void spo2AlarmPriorityChanged(SmmManager::AlarmPriority priority);
    void pulseLowerLimitChanged();
    void pulseUpperLimitChanged();
    void isPulseAlarmActiveChanged();
    void pulseAlarmPriorityChanged();
    void isAlarmMutedChanged(bool muted);
    void waveformSpeedChanged(int newSpeed);


private slots:
    void readData(); //UART'a veri geldikçe tetiklenir
    void sendNextHandshakeByte();
    void onWatchdogTimeout(); // Modül uyuduğunda tetiklenecek
private:
    quint64 m_lastDbSaveTime = 0;
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
    AveragingSeconds m_averageSecond = AveragingSeconds::sec4;

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
    quint8 m_currentConfigByte = 0x82;

    bool m_isReconnectLogPrinted = false;
    bool m_hasConnectionError = false;
    bool hasConnectionError() const {return m_hasConnectionError;}

    int m_spo2LowerLimit = 90;
    int m_spo2UpperLimit = 100;

    bool m_isSpo2AlarmActive = false;

    AlarmPriority m_spo2AlarmPriority = AlarmPriority::Red;

    int m_pulseLowerLimit = 60;
    int m_pulseUpperLimit = 100;

    bool m_isPulseAlarmActive = false;

    AlarmPriority m_pulseAlarmPriority = AlarmPriority::Red;

    void logAlarm (const QString &paramType, int value, const QString &priority);

    QMediaPlayer *m_soundPlayer;
    QAudioOutput *m_audioOutput;

    QTimer *m_muteTimer;
    bool m_isAlarmMuted = false;
    void onMuteTimeout();

    int m_waveformSpeed = 25;

    bool m_isDemoMode = false;
};

#endif // SMMMANAGER_H