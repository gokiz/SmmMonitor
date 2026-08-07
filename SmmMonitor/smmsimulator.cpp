#include <QDebug>
#include "smmsimulator.h"

SmmSimulator::SmmSimulator(QObject *parent) : QObject(parent), m_spo2(98), m_pulseRate(75) {
    m_dataTimer = new QTimer(this);
    connect(m_dataTimer, &QTimer::timeout, this, &SmmSimulator::generateMockData);

    m_scenarioTimer = new QTimer(this);
    connect(m_scenarioTimer, &QTimer::timeout, this, &SmmSimulator::advanceScenario);

}

int SmmSimulator::getSpo2() const {
    return m_spo2;
}

int SmmSimulator::getPulseRate() const {
    return m_pulseRate;
}

void SmmSimulator::enterScenario(Scenario scenario) {
    m_currentScenario = scenario;

    switch(scenario) {
    case Scenario::Normal :
        m_scenarioTicksLeft = 8 + QRandomGenerator::global()->bounded(6);
        break;
    case Scenario::Spo2LowAlarm :
        m_scenarioTicksLeft = 4 + QRandomGenerator::global()->bounded(3);
        break;
    case Scenario::PulseAlarm :
        m_scenarioTicksLeft = 4 + QRandomGenerator::global()->bounded(3);
        break;
    case Scenario::SignalWeak :
        m_scenarioTicksLeft = 3 + QRandomGenerator::global()->bounded(3);
        break;
    case Scenario::SearchingForPulse :
        m_scenarioTicksLeft = 3 + QRandomGenerator::global()->bounded(3);
        break;
    }
}

void SmmSimulator::advanceScenario() {
    if(m_scenarioTicksLeft > 0){
        m_scenarioTicksLeft--;
        return;
    }

    if(m_currentScenario != Scenario::Normal) {
        enterScenario(Scenario::Normal);
        return;
    }

    const int roll = QRandomGenerator::global()->bounded(100);
    if(roll < 55) {
        enterScenario(Scenario::Normal);
    } else if(roll < 70){
        enterScenario(Scenario::Spo2LowAlarm);
    } else if (roll < 85) {
        enterScenario(Scenario::PulseAlarm);
    } else if(roll < 93) {
        enterScenario(Scenario::SignalWeak);
    } else {
        enterScenario(Scenario::SearchingForPulse);
    }
}

void SmmSimulator::generateMockData() {
    static int tickCounter = 25;
    if(tickCounter >= 25) {
        tickCounter = 0;
        switch(m_currentScenario) {
        case Scenario::Normal :
            m_spo2 = 95 + QRandomGenerator::global()->bounded(5);
            m_pulseRate = 70 + QRandomGenerator::global()->bounded(21);
            break;

        case Scenario::Spo2LowAlarm :
            m_spo2 = 82 + QRandomGenerator::global()->bounded(6);
            m_pulseRate = 75 + QRandomGenerator::global()->bounded(10);
            break;

        case Scenario::PulseAlarm :
            m_spo2 = 96 + QRandomGenerator::global()->bounded(4);
            if(QRandomGenerator::global()->bounded(2) == 0){
                m_pulseRate = 40 + QRandomGenerator::global()->bounded(10);
            } else {
                m_pulseRate = 110 + QRandomGenerator::global()->bounded(20);
            }
            break;

        case Scenario::SignalWeak :

            m_spo2 = 90 + QRandomGenerator::global()->bounded(8);
            m_pulseRate = 70 + QRandomGenerator::global()->bounded(20);
            break;

        case Scenario::SearchingForPulse :

            m_spo2 = 0;
            m_pulseRate = 0;
            break;
        }
    }

    tickCounter++;

    bool signalWeak = false;
    bool pulseSearching = false;
    emit dataChanged(m_spo2, m_pulseRate, signalWeak, pulseSearching);
}





void SmmSimulator::startDemo() {
    enterScenario(Scenario::Normal);
    m_dataTimer->start(40);
    m_scenarioTimer->start(1000);
}

void SmmSimulator::stopDemo() {
    m_dataTimer->stop();
    m_scenarioTimer->stop();
}