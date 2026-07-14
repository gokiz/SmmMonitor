#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "smmmanager.h"

int main(int argc, char *argv[]){
    QGuiApplication app(argc, argv);
    SmmManager smmManager;
    QQmlApplicationEngine engine;

    //C++ sınıfı QML tarafına 'smmManager' olarak kaydediliyor
    engine.rootContext()->setContextProperty("smmManager", &smmManager);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl){
        if(!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    smmManager.connectToModule("COM5"); //Modülün adı bilgisayar bağlantısına göre değişir

    return app.exec();
}