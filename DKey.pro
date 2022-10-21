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
    dkString.cpp \
    dkView.cpp \
    dkTerm.cpp \
    dkTermList.cpp \
    termDialog.cpp \
    termWindow.cpp \
    dkFormat.cpp \
    dkStringInt.cpp \
    dkBrowser.cpp \
    textEditor.cpp \
    commands.cpp

HEADERS  += mainwindow.h \
    coupletDialog.h \
    dkCouplet.h \
    dkCoupletList.h \
    dkString.h \
    dkView.h \
    dkTerm.h \
    dkTermList.h \
    termDialog.h \
    termWindow.h \
    dkFormat.h \
    dkStringInt.h \
    dkBrowser.h \
    textEditor.h \
    commands.h

OTHER_FILES += \
    DKey.rc \
    DKey.ico

RC_FILE = DKey.rc

DEFINES += VERSION_STRING=\\\"2.0.0\\\"

RESOURCES += \
    DKey.qrc

