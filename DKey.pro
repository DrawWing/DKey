 # This file is part of DKey software.
 # Copyright (c) 2017 Adam Tofilski
 #
 # This program is free software: you can redistribute it and/or modify
 # it under the terms of the GNU General Public License as published by
 # the Free Software Foundation, version 3.
 #
 # This program is distributed in the hope that it will be useful, but
 # WITHOUT ANY WARRANTY; without even the implied warranty of
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 # General Public License for more details.
 #
 # You should have received a copy of the GNU General Public License
 # along with this program. If not, see <http://www.gnu.org/licenses/>.

QT       += core gui widgets xml

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

DEFINES += VERSION_STRING=\\\"2.2.0\\\"

RESOURCES += \
    DKey.qrc

win32 {
DESTDIR = $$PWD/../DKey-bin
QMAKE_POST_LINK =  windeployqt $$shell_path($$DESTDIR/$${TARGET}.exe)
}

DISTFILES += \
    bin/DKey-windows-binaries.zip
