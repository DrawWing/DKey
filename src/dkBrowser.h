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

#ifndef DKBROWSER_H
#define DKBROWSER_H

#include <QMainWindow>
#include <QUrl>

#include "dkFormat.h"

class QTextBrowser;

class dkBrowser: public QMainWindow
{
    Q_OBJECT

public:
    dkBrowser(QWidget *parent = 0);
    ~dkBrowser();
    dkBrowser(const QString & txt, QWidget *parent = 0);
    void setPlainTxt(const QString & txt);
    void setHtml(const QString & txt);
    void setFormat(dkFormat * inFormat);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void clickedLink(QUrl inUrl);

private:
    void readSettings();
    void writeSettings();
    void goToGlossary(int nr);

    QTextBrowser *textEdit;
    dkFormat * format;
};

#endif // DKBROWSER_H
