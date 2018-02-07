#-------------------------------------------------
#
# Project created by QtCreator 2017-08-06T20:01:41
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DKey
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    coupletDialog.cpp \
    dkCouplet.cpp \
    dkCoupletList.cpp \
    txtwindow.cpp \
    dkString.cpp \
    dkView.cpp

HEADERS  += mainwindow.h \
    coupletDialog.h \
    dkCouplet.h \
    dkCoupletList.h \
    txtwindow.h \
    dkString.h \
    dkView.h

OTHER_FILES += \
    DKey.rc \
    DKey.ico

RC_FILE = DKey.rc

DEFINES += VERSION_STRING=\\\"1.3.0\\\"

RESOURCES += \
    DKey.qrc

