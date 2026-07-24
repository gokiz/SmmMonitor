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

private:
    QList<int>m_points;
    int m_currentIndex; //çizimin x ekseninde o an nerede olduğunu tutar
    int m_maxPoints; // ekrana aynı anda sığacak nokta sayısı
    int m_gapSize; // çizinm imlecinin önğndeki silinen (boş) alanın boyutu
};

#endif // WAVEFORMPLOTTER_H
