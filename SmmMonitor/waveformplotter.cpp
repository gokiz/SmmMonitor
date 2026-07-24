#include "waveformplotter.h"

WaveformPlotter::WaveformPlotter(QQuickItem *parent) : QQuickPaintedItem(parent){
    m_maxPoints = 150; //grafiğin x ekseninde kaç nokta tutulacağı(sıklık)
    setAntialiasing(true);
}

void WaveformPlotter::addPoint(int value){
    m_points.append(value);

    //liste belirlenen boyutu aşarsa, en eski veriyi(sola kaydırma efekti)
    if(m_points.size() > m_maxPoints) {
        m_points.removeFirst();
    }

    //arayuzu yeniden cizmeye zorla
    update();
}
void WaveformPlotter::paint(QPainter *painter){
    if(m_points.size() < 2) return;

    //kalem(çizgi) ayarları
    QPen pen(QColor("#10b981")); //spo3deki yeşil
    pen.setWidth(2);
    painter->setPen(pen);

    //çizim alanının genişlik ve yüksekliğine göre oranlama
    double stepX = width() / (double)(m_maxPoints - 1);
    double scaleY = height() / 100.0; //veriler 0-99 arasında

    //noktaları birbirine bağlayarak çiz
    for(int i = 0; i < m_points.size() - 1; ++i){
        //Y ekseni qpainter'da yukarıdan aşağıya büyüdüğü için yğksekliği ter çevir
        QPointF p1(i * stepX, height() - (m_points[i] * scaleY));
        QPointF p2((i + 1) * stepX, height() - (m_points[i+1] * scaleY));

        painter->drawLine(p1,p2);
    }
}