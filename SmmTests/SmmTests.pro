CONFIG += warn_on qmltestcase

TEMPLATE = app

QT += testlib core gui quick serialport qmltest

DEFINES += QUICK_TEST_SOURCE_DIR=\\\"$$PWD\\\"

DISTFILES += \
    tst_smmtest.qml

SOURCES += \
    main.cpp

# deployment rules
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
# Asıl projenin dosyalarını test projesine dahil ediyoruz
INCLUDEPATH += $$PWD/../SmmMonitor

SOURCES += \
    $$PWD/../SmmMonitor/smmmanager.cpp

HEADERS += \
    $$PWD/../SmmMonitor/smmmanager.h