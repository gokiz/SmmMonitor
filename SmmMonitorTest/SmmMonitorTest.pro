QT += core testlib sql serialport multimedia

INCLUDEPATH += $$quote(C:/Program Files/OpenSSL-Win64/include)
LIBS += $$quote(C:/Program Files/OpenSSL-Win64/libcrypto-3-x64.dll)
LIBS += $$quote(C:/Program Files/OpenSSL-Win64/libssl-3-x64.dll)


CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

HEADERS += \
    ../SmmMonitor/smmmanager.h\
    ../SmmMonitor/aesgcmcrypto.h

SOURCES += \
    tst_smmtest.cpp \
    ../SmmMonitor/smmmanager.cpp\
    ../SmmMonitor/aesgcmcrypto.cpp

# deployment rules
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
