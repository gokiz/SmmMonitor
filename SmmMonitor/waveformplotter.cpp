#include "waveformplotter.h"

WaveformPlotter::WaveformPlotter(QQuickItem *parent) : QQuickPaintedItem(parent){
    m_maxPoints = 350; //grafiğin x ekseninde kaç nokta tutulacağı(sıklık)
    m_gapSize = 5; //çizgiler ilerlerken hemen önğnde açılacak boşluğun boyutu
    m_currentIndex = 0;

    //dizinin boyutunu sabitle ve -1 ile doldur
    m_points.resize(m_maxPoints);
    m_points.fill(-1);

    setAntialiasing(true);
}

void WaveformPlotter::addPoint(int value){
    m_points[m_currentIndex] = value;

    m_currentIndex++;
    if(m_currentIndex >= m_maxPoints){
        m_currentIndex = 0;
    }

    for(int i = 0; i < m_gapSize; ++i){
        m_points[(m_currentIndex + i) % m_maxPoints] = -1;
    }

    //arayuzu yeniden cizmeye zorla
    update();
}
void WaveformPlotter::paint(QPainter *painter){
    if(m_points.isEmpty()) return;

    //kalem(çizgi) ayarları
    QPen pen(QColor(0x10, 0xB9, 0x81)); //spo3deki yeşil
    pen.setWidth(2);
    pen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(pen);

    //çizim alanının genişlik ve yüksekliğine göre oranlama
    double stepX = width() / (double)(m_maxPoints - 1);

    //grafiğin altından ve üstünden 5er piksel pay bırakarak güvenli bir çizim alanı oluşturulur
    double drawableHeight = height() - 10.0;
    double scaleY = height() / 100.0; //veriler 0-99 arasında

    //noktaları birbirine bağlayarak çiz
    for(int i = 0; i < m_points.size() - 1; ++i){

        if(m_points[i] == -1 || m_points[i+1] == -1){
            continue;
        }


        //Y ekseni qpainter'da yukarıdan aşağıya büyüdüğü için yğksekliği ter çevir
        QPointF p1(i * stepX, height() - (m_points[i] * scaleY));
        QPointF p2((i + 1) * stepX, height() - (m_points[i+1] * scaleY));

        painter->drawLine(p1,p2);
    }
}