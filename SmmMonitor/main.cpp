#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug> // Loglama yapabilmek için eklendi
#include "smmmanager.h"
#include "smmsimulator.h"
#include "waveformplotter.h"

int main(int argc, char *argv[]){
    QGuiApplication app(argc, argv);

    SmmManager smmManager;
    SmmSimulator smmSimulator;

    qmlRegisterType<WaveformPlotter>("CustomControls", 1, 0, "WaveformPlotter");
    qmlRegisterUncreatableType<SmmManager>("Backend", 1, 0, "SmmManager", "SmmManager nesnesi QML'den olusturulamaz!");


    QObject::connect(&smmSimulator, SIGNAL(dataChanged(int,int)),
                     &smmManager, SLOT(injectTestData(int,int)));

    QQmlApplicationEngine engine;

    // C++ sınıfları QML tarafına 'context property' olarak kaydediliyor
    engine.rootContext()->setContextProperty("smmManager", &smmManager);
    engine.rootContext()->setContextProperty("smmSimulator", &smmSimulator);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl){
                         if(!obj && url == objUrl)
                             QCoreApplication::exit(-1);
                     }, Qt::QueuedConnection);
    engine.load(url);


    smmManager.connectToModule("COM5");
    smmManager.initializeBiolightModule();

    return app.exec();
}