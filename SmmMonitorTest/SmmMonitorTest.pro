QT += core testlib sql serialport multimedia


CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

HEADERS += \
    ../SmmMonitor/smmmanager.h

SOURCES += \
    tst_smmtest.cpp \
    ../SmmMonitor/smmmanager.cpp

# deployment rules
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
