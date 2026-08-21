#include "networkutils.h"
#include <QNetworkInterface>
#include <QList>
#include <QDebug>

QString NetworkUtils::getValidMacAddress() {
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    QString macAddress = " ";

    foreach (const QNetworkInterface &iface, interfaces) {
        QNetworkInterface::InterfaceFlags flags = iface.flags();
        QNetworkInterface::InterfaceType type = iface.type();

        bool isUpAndRunning = (flags & QNetworkInterface::IsUp) && (flags & QNetworkInterface::IsRunning);
        bool isLoopbackOrP2P = (flags & QNetworkInterface::IsLoopBack) || (flags & QNetworkInterface::IsPointToPoint);
        bool isVirtualType = (type == QNetworkInterface::Virtual || type == QNetworkInterface::Loopback);
        if(!isUpAndRunning || isLoopbackOrP2P || isVirtualType) {
            continue;
        }

        QString currentMac = iface.hardwareAddress();

        if(currentMac.isEmpty()) {
            continue;
        }
        macAddress = currentMac;
        qDebug()  << "Lisanslama için seçilen MAC: " << macAddress;
        break;
    }
    return macAddress;
}
