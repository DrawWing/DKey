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

#ifndef TERMWINDOW_H
#define TERMWINDOW_H

#include <QMainWindow>
#include <QTableWidget>

#include "dkTermList.h"
#include "mainwindow.h"

//class QAction;
//class QMenu;

class termWindow: public QMainWindow
{
    Q_OBJECT

public:
    termWindow(dkTermList *inList, MainWindow *inParent = 0);
    termWindow(QString &inFileName, MainWindow *inParent = 0);
protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void newTerms();
    void openFile();
    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName);
    void import();
    void exportTxt();
    void insertRow();
    void removeRow();
    void copyRows();
    void cutRows();
    void pasteRows();
    void editRow();
    void editClickedRow(int row, int col=1);
    void sortTable();

private:
    QAction *newAct;
    QAction *importAct;
    QAction *saveAct;
    QAction *exportAct;
    QAction *insertRowAct;
    QAction *removeRowAct;
    QAction *copyAct;
    QAction *cutAct;
    QAction *pasteAct;
    QAction *editRowAct;
    QAction *sortTableAct;

    QMenu *fileMenu;
    QMenu *editMenu;
    QToolBar *editToolBar;

    bool loadFile(const QString &fileName);
    void createActions();
    void createMenus();
    void createToolBars();
    void createTable();
    void fillTable();
    void insertTableRow(int i);
    void updateTable();
    void updateTableRow(int i);
    bool okToContinue();
    void clear();

    void readSettings();
    void writeSettings();

    QTableWidget *table;
    dkTermList termList;
    dkTermList terminology; // temporary

    dkTermList termClipboard;
    bool isCopy;
    QString filePath;

    MainWindow *parent;
};

#endif // TERMWINDOW_H
