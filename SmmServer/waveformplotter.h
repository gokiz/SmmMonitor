#ifndef WAVEFORMPLOTTER_H
#define WAVEFORMPLOTTER_H

#include <QQuickPaintedItem>
#include <QPainter>
#include <QList>

class WaveformPlotter : public QQuickPaintedItem {
    Q_OBJECT
public:
    WaveformPlotter(QQuickItem *parent = nullptr);
    virtual void paint(QPainter *painter) override;

    Q_INVOKABLE void addPoint(int value);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void setWaveformSpeed(int speed);

private:
    QList<int>m_points;
    int m_currentIndex;
    int m_maxPoints;
    int m_gapSize;
    int m_waveformSpeed = 25;
};

#endif // WAVEFORMPLOTTER_H
