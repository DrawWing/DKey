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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextBrowser>
#include <QDir>
#include <QMessageBox>

#include <QAction>
#include <QMenu>

#include <QTableWidget>

#include <QList>

#include "dkCouplet.h"
#include "dkCoupletList.h"
#include "txtwindow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool loadFile(const QString &fileName);

protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void openFile();
    void import();
    bool save();
    bool saveAs();
    void about();
    void exportKey();
    void openRecentFile();
    void insertRow();
    void removeRow();
    void editRow();
    void editClickedRow(int row, int col);
    void swapLeads();
    void showSteps();
    void reNumber();
    void viewHtml();
    void selectTableRow(int row, int col);

private:
    void createTable();
    void createActions();
    void createMenus();
    void readSettings();
    void writeSettings();
    void updateRecentFileActions();
    bool okToContinue();
    bool saveFile(const QString &fileName);

    QAction *openAct;
    QAction *importAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exportHtmlAct;
    QAction *exitAct;
    QAction *insertRowAct;
    QAction *removeRowAct;
    QAction *editRowAct;
    QAction *swapLeadsAct;
    QAction *reNumberAct;
    QAction *showStepsAct;
    QAction *viewHtmlAct;
    QAction *aboutAct;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;

    QStringList recentFiles;
    enum { MaxRecentFiles = 5 };
    QAction *recentFileActions[MaxRecentFiles];
    QAction *separatorAction;

    QString appName;
    QString appVersion;

    QTableWidget *table;
//    QTextBrowser *textEdit;
    TxtWindow * htmlWindow;

    QString filePath;

    QString isSane();
    QString errorIsConsequtive() const;
//    QString idChain(int coupletNo);
    void importTxt(const QString &fileName);
    void fillTable();
    void insertTabRow(int i);
    void updateTable();
    void updateTableRow(int i);
    void clear();

    dkCoupletList coupletList;
    QString error;
};

#endif // MAINWINDOW_H
