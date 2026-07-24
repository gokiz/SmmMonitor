#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "smmmanager.h"
#include "smmsimulator.h"
#include "waveformplotter.h"

int main(int argc, char *argv[]){
    QGuiApplication app(argc, argv);
    SmmManager smmManager;
    SmmSimulator smmSimulator;

    qmlRegisterType<WaveformPlotter>("CustomControls", 1, 0, "WaveformPlotter");

    /* QObject::connect(&smmSimulator, &SmmSimulator::mockDataReady,
                     &smmManager, &SmmManager::injectTestData);
    */

    QQmlApplicationEngine engine;

    //C++ sınıfı QML tarafına 'smmManager' olarak kaydediliyor
    engine.rootContext()->setContextProperty("smmManager", &smmManager);
    engine.rootContext()->setContextProperty("smmSimulator", &smmSimulator);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl){
        if(!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    smmManager.connectToModule("COM5"); //Modülün adı bilgisayar bağlantısına göre değişir
    //modülü aktifleştir
    smmManager.initializeBiolightModule();


    return app.exec();
}