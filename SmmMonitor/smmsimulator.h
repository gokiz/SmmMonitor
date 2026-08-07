#ifndef SMMSIMULATOR_H
#define SMMSIMULATOR_H

#include <QObject>
#include <QTimer>
#include <QByteArray>
#include <QRandomGenerator>

class SmmSimulator : public QObject {
    Q_OBJECT
    Q_PROPERTY(int spo2 READ getSpo2 NOTIFY dataChanged)
    Q_PROPERTY(int pulseRate READ getPulseRate NOTIFY dataChanged)

public:
    explicit SmmSimulator(QObject *parent = nullptr);
    int getSpo2() const;
    int getPulseRate() const;

    Q_INVOKABLE void startDemo();
    Q_INVOKABLE void stopDemo();

signals:
    void dataChanged(int spo2, int pulseRate, bool isSignalWeak, bool isPulseSearchin);



private slots:
    void generateMockData();
    void advanceScenario();

private:
    enum class Scenario {
        Normal,
        Spo2LowAlarm,
        PulseAlarm,
        SignalWeak,
        SearchingForPulse
    };

    void enterScenario(Scenario scenario);

    int m_spo2;
    int m_pulseRate;
    QTimer *m_dataTimer;
    QTimer *m_scenarioTimer;

    Scenario m_currentScenario = Scenario::Normal;
    int m_scenarioTicksLeft = 0;

};

#endif // SMMSIMULATOR_H
