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

SOURCES += src/main.cpp\
    src/mainwindow.cpp \
    src/coupletDialog.cpp \
    src/dkCouplet.cpp \
    src/dkCoupletList.cpp \
    src/dkString.cpp \
    src/dkView.cpp \
    src/dkTerm.cpp \
    src/dkTermList.cpp \
    src/termDialog.cpp \
    src/termWindow.cpp \
    src/dkFormat.cpp \
    src/dkStringInt.cpp \
    src/dkBrowser.cpp \
    src/textEditor.cpp \
    src/commands.cpp

HEADERS  += src/mainwindow.h \
    src/coupletDialog.h \
    src/dkCouplet.h \
    src/dkCoupletList.h \
    src/dkString.h \
    src/dkView.h \
    src/dkTerm.h \
    src/dkTermList.h \
    src/termDialog.h \
    src/termWindow.h \
    src/dkFormat.h \
    src/dkStringInt.h \
    src/dkBrowser.h \
    src/textEditor.h \
    src/commands.h

DEFINES += VERSION_STRING=\\\"2.2.0\\\"

RESOURCES += assets/DKey.qrc

win32:RC_ICONS = assets/DKey.ico

win32 {
DESTDIR = $$PWD/../DKey-windows-binaries
QMAKE_POST_LINK =  windeployqt $$shell_path($$DESTDIR/$${TARGET}.exe)
}

DISTFILES += \
    README.md \
    assets/DKey.ico \
    bin/DKey-windows-binaries.zip \
    docs/README-dkey-main-window.png \
    docs/README-dkey-taxonomy.png \
    docs/README-dkey48.png
