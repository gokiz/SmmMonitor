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

    //qmlde yeni veri geldiğinde çağıralacak fonksiyon
    Q_INVOKABLE void addPoint(int value);
    Q_INVOKABLE void clear();

    Q_INVOKABLE void setWaveformSpeed(int speed);
    Q_INVOKABLE void calibrate(double pixelDensity);

private:
    QList<int>m_points;
    int m_currentIndex; //çizimin x ekseninde o an nerede olduğunu tutar
    int m_maxPoints; // ekrana aynı anda sığacak nokta sayısı
    int m_gapSize; // çizinm imlecinin önğndeki silinen (boş) alanın boyutu
    int m_waveformSpeed = 25;
    int m_samplingRate = 50;
    double m_physicalWidthMm = 300.0;
    double m_pixelDensity = 4.0;
    void recalculateMaxPoints();
};

#endif // WAVEFORMPLOTTER_H
