#ifndef SMMSIMULATOR_H
#define SMMSIMULATOR_H

#include <QObject>
#include <QTimer>
#include <QByteArray>
#include <QRandomGenerator>

class SmmSimulator : public QObject {
    Q_OBJECT

public:
    explicit SmmSimulator(QObject *parent = nullptr);

    //qmlde butona basip baslatıp durudurabilmek icin
    Q_INVOKABLE void startSimulation();
    Q_INVOKABLE void stopSimulation();
    Q_INVOKABLE void forceState(int stateIndex);

signals:

    //simulator sahte paketi hazirlayinca bu sinyalle sisteme gonderilecek
    void mockDataReady(const QByteArray &data);

private slots:
    void simulateNextState(); //QTimer tetiklendiginde calisacak

private:
    QTimer *m_simTimer;
    int m_simState;
    quint8 m_currentSpo2;
    quint8 m_currentPulse;

    bool m_isManuelMode;

    static quint8 calcChecksum(quint8 len, quint8 code, const QByteArray &data);


};

#endif // SMMSIMULATOR_H
