#include "waveformplotter.h"

WaveformPlotter::WaveformPlotter(QQuickItem *parent) : QQuickPaintedItem(parent){
    m_maxPoints = 600;
    m_gapSize = 3;
    m_currentIndex = 0;

    m_points.resize(m_maxPoints);
    m_points.fill(0);   // başlangıçta düz çizgi

    setAntialiasing(true);
}

void WaveformPlotter::addPoint(int value){
    m_points[m_currentIndex] = value;
    m_currentIndex = (m_currentIndex + 1) % m_maxPoints;   // her zaman +1, veri tekrarı yok

    for(int i = 0; i < m_gapSize; ++i){
        m_points[(m_currentIndex + i) % m_maxPoints] = -1;
    }

    update();
}

void WaveformPlotter::paint(QPainter *painter){
    if(m_points.isEmpty()) return;

    QPen pen(QColor(0x10, 0xB9, 0x81));
    pen.setWidth(2);
    pen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(pen);

    double stepX = width() / (double)(m_maxPoints - 1);
    // <-- hız etkisi SADECE burada: 50'de noktalar arası mesafe 2 katı

    double scaleY = height() / 126.0;

    for(int i = 0; i < m_points.size() - 1; ++i){
        if(m_points[i] == -1 || m_points[i+1] == -1){
            continue;
        }

        QPointF p1(i * stepX, height() - (m_points[i] * scaleY));
        QPointF p2((i + 1) * stepX, height() - (m_points[i+1] * scaleY));

        if (p1.x() > width() && p2.x() > width()) break; // ekran dışına taşanları çizme, gereksiz iş yapma

        painter->drawLine(p1, p2);
    }
}

void WaveformPlotter::clear() {
    m_points.fill(0);
    m_currentIndex = 0;
    update();
}

void WaveformPlotter::calibrate(double pixelDensity) {
    if(pixelDensity > 0) {
        m_pixelDensity = pixelDensity;
        recalculateMaxPoints();
    }
}

void WaveformPlotter::recalculateMaxPoints() {
    if(width() <= 0 || m_pixelDensity <= 0 ) return;

    double speedInPixelsPerSecond = m_waveformSpeed * m_pixelDensity;

    //600 nokta / 12 saniye
    const double SAMPLING_RATE = 50.0;

    double stepX = speedInPixelsPerSecond / SAMPLING_RATE;

    int newMaxPoints = static_cast<int>(width() / stepX);
    if(newMaxPoints < 10 ) newMaxPoints = 10;

    if(m_maxPoints != newMaxPoints) {
        m_maxPoints = newMaxPoints;
        m_points.resize(m_maxPoints);
        m_points.fill(-1);
        m_currentIndex = 0;
        update();
    }
}

void WaveformPlotter::setWaveformSpeed(int speed) {
    if(speed != 25 && speed != 50) return;
    if(m_waveformSpeed == speed) return;

    m_waveformSpeed = speed;

    // YENİ: Hız (25/50) değiştiğinde noktaları baştan hesapla
    recalculateMaxPoints();
}