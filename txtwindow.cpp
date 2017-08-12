/*
 * This file is part of DKey software.
 * Copyright (c) 2017 Adam Tofilski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtWidgets>

#include "txtwindow.h"

TxtWindow::TxtWindow(QWidget *parent)
: QMainWindow(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    textEdit = new QTextBrowser;
    setCentralWidget(textEdit);
    readSettings();
}

TxtWindow::TxtWindow(const QString & txt, QWidget *parent)
    : QMainWindow(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    textEdit = new QTextBrowser;
    textEdit->setHtml(txt);
    setCentralWidget(textEdit);
    readSettings();
}

void TxtWindow::setPlainTxt(const QString &txt)
{
    textEdit->setPlainText(txt);
    show();
}


void TxtWindow::setHtml(const QString & txt)
{
    textEdit->setHtml(txt);
    show();
}

void TxtWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    hide();
        event->ignore();
    //    event->accept();
}


void TxtWindow::readSettings()
{
    QString companyName = QGuiApplication::organizationName();
    QString windowName = QGuiApplication::applicationName() + " - Viewer";
    QSettings settings(companyName, windowName);
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    move(pos);
    resize(size);
}

void TxtWindow::writeSettings()
{
    QString companyName = QGuiApplication::organizationName();
    QString windowName = QGuiApplication::applicationName() + " - Viewer";
    QSettings settings(companyName, windowName);
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

