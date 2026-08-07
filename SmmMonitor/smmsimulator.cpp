#include <QDebug>
#include "smmsimulator.h"

SmmSimulator::SmmSimulator(QObject *parent) : QObject(parent), m_spo2(98), m_pulseRate(75) {
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &SmmSimulator::generateMockData);

}

int SmmSimulator::getSpo2() const {
    return m_spo2;
}

int SmmSimulator::getPulseRate() const {
    return m_pulseRate;
}

void SmmSimulator::generateMockData() {
    m_spo2 = 95 + QRandomGenerator::global()->bounded(5);
    m_pulseRate = 75 + QRandomGenerator::global()->bounded(15);

    emit dataChanged(m_spo2, m_pulseRate);
}

void SmmSimulator::startDemo() {
    m_timer->start(1000);
}

void SmmSimulator::stopDemo() {
    m_timer->stop();
}