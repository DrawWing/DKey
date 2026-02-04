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

#include "dkBrowser.h"

dkBrowser::dkBrowser(QWidget *parent)
    : QMainWindow(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    textEdit = new QTextBrowser(this);
    textEdit->setOpenLinks(false);
    connect(textEdit, SIGNAL( anchorClicked( QUrl ) ),
            this, SLOT( clickedLink( QUrl ) ));
    setCentralWidget(textEdit);
    readSettings();
}

dkBrowser::dkBrowser(const QString & txt, QWidget *parent)
    : QMainWindow(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    textEdit = new QTextBrowser(this);
    textEdit->setHtml(txt);
    setCentralWidget(textEdit);
    readSettings();
}

dkBrowser::~dkBrowser()
{
}

void dkBrowser::setPlainTxt(const QString &txt)
{
    textEdit->setPlainText(txt);
    show();
}

void dkBrowser::setHtml(const QString & txt)
{
    textEdit->setHtml(txt);
    show();
}

void dkBrowser::setFormat(dkFormat * inFormat)
{
    format = inFormat;
}

void dkBrowser::closeEvent(QCloseEvent *event)
{
    writeSettings();
    hide();
    event->ignore();
    //    event->accept();
}

void dkBrowser::readSettings()
{
    QString companyName = QGuiApplication::organizationName();
    QString windowName = QGuiApplication::applicationName() + " - Viewer";
    QSettings settings(companyName, windowName);
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    move(pos);
    resize(size);
}

void dkBrowser::writeSettings()
{
    QString companyName = QGuiApplication::organizationName();
    QString windowName = QGuiApplication::applicationName() + " - Viewer";
    QSettings settings(companyName, windowName);
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

void dkBrowser::clickedLink(QUrl inUrl)
{
    QString fragment =  inUrl.fragment();
    if(!fragment.isEmpty() && fragment[0]=='g')
    {
        QString nrStr = fragment.mid(1);
        bool ok;
        int nr = nrStr.toInt(&ok);
        if(ok)
            goToGlossary(nr-1);
    }
//    else if(fragment[0]=='k')
//    {
//        QString nrStr = fragment.mid(1);
//        bool ok;
//        int nr = nrStr.toInt(&ok);
//        if(ok)
//            goToNumber(nr);
//    }
}

void dkBrowser::goToGlossary(int nr)
{
    QString outTxt = format->glossHtml(nr, true);
//    outTxt += "<br />"; // otherwise images in the same line
//    outTxt += format.imagesHtml(outTxt, true);

    // todo: use only pointers to glossary in linkGlossary
//    outTxt = format->linkGlossary(outTxt); //moved to glossHtml

    textEdit->setHtml(outTxt);
    setWindowTitle(tr("Hypertext viewer"));
}
