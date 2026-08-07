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
    void dataChanged(int spo2, int pulseRate);



private slots:
    void generateMockData();

private:
    int m_spo2;
    int m_pulseRate;
    QTimer *m_timer;

};

#endif // SMMSIMULATOR_H
