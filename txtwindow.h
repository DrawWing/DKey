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

#ifndef TXTWINDOW_H
#define TXTWINDOW_H

#include <QMainWindow>

class QAction;
class QMenu;
class QTextBrowser;

class TxtWindow: public QMainWindow
{
    Q_OBJECT

public:
    TxtWindow(QWidget *parent = 0);
    TxtWindow(const QString & txt, QWidget *parent = 0);
    void setPlainTxt(const QString & txt);
    void setHtml(const QString & txt);

protected:
    void closeEvent(QCloseEvent *event);

private slots:

private:
    void readSettings();
    void writeSettings();

    QTextBrowser *textEdit;
};

#endif //TXTWINDOW_H
