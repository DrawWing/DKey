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

#ifndef DKVIEW_H
#define DKVIEW_H

#include <QTextBrowser>
#include <QTableWidget>
#include <QMainWindow>
#include <QAction>
#include <QMenu>

#include "dkCoupletList.h"

class dkView : public QMainWindow
{
    Q_OBJECT
public:
    explicit dkView(dkCoupletList *inList, QWidget *parent = 0);

signals:

public slots:

private slots:
    void goToNumber(int inNumber);
    void goToCouplet();
    void goToTag();
    void goToEndpoint(bool first);
    void clickedCouplet(QUrl inUrl);
    void clickedPath(int row, int col);
    void clickedRemaining(int row, int col);
    void clickedExcluded(int row, int col);
private:
    void createActions();
    void createMenus();
    void goToItemTxt(const QString & theItemString);

    QAction *goToNumberAct;
    QAction *goToTagAct;
    QAction *exitAct;
    QAction *aboutAct;

    QMenu *navMenu;
    QMenu *helpMenu;
    QMenuBar *my_menuBar;

    int number;
    dkCoupletList * coupletList;
    dkCouplet currCouplet;
    QStringList endpointList;
    QStringList tagList;
    QString filePath;
    QTextBrowser *lead1Browser;
    QTextBrowser *lead2Browser;
    QTableWidget *pathTab;
    QTableWidget *remainingTab;
    QTableWidget *excludedTab;
};

#endif // DKVIEW_H
