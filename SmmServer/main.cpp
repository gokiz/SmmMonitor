#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "udpreceiver.h"
#include "waveformplotter.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    // "Backedn" yerine "Backend" olarak düzeltildi
    qmlRegisterType<UdpReceiver>("Backend", 1, 0, "UdpReceiver");
    qmlRegisterType<WaveformPlotter>("Smm.Grafik", 1, 0, "WaveformGraph");

    QQmlApplicationEngine engine;

    const QUrl url(QStringLiteral("qrc:/main.qml")); // Dosya adın main.qml ise böyle kalabilir

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec(); // QGuiApplication::exec() yerine doğrudan app nesnesini kullanmak daha yaygın bir pratiktir
}