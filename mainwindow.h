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
#include <QToolBar>
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
    void newKey();
    void openFile();
    void import();
    void appendFile();
    bool save();
    bool saveAs();
    void about();
    void exportKey();
    void openRecentFile();
    void insertRow();
    void removeRow();
    void copyRows();
    void cutRows();
    void pasteRows();
    void editRow();
    void editClickedRow(int row, int col=1);
    void swapLeads();
    void reNumber();
    void goToTag();
    void arrange();
    void goToNumber();
    void findErrors();
    void findImgErrors();
    void viewBrowser();
    void viewHtml();
    void viewEndpoints();
    void viewTags();
    void test();

private:
    void createTable();
    void createActions();
    void createMenus();
    void createToolBars();
    void readSettings();
    void writeSettings();
    void updateRecentFiles( const QString &fileName);
    void updateRecentFileActions();
    bool okToContinue();
    bool saveFile(const QString &fileName);

    QAction *newAct;
    QAction *openAct;
    QAction *importAct;
    QAction *appendAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exportHtmlAct;
    QAction *exitAct;
    QAction *insertRowAct;
    QAction *removeRowAct;
    QAction *copyAct;
    QAction *cutAct;
    QAction *pasteAct;
    QAction *editRowAct;
    QAction *swapLeadsAct;
    QAction *renumberAct;
    QAction *arrangeAct;
    QAction *goToNumberAct;
    QAction *goToTagAct;
    QAction *findErrorsAct;
    QAction *findImgErrorsAct;
    QAction *viewBrowserAct;
    QAction *viewHtmlAct;
    QAction *viewEndpointsAct;
    QAction *viewTagsAct;
    QAction *aboutAct;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *debugMenu;
    QMenu *viewMenu;
    QMenu *navMenu;
    QMenu *helpMenu;

    QToolBar *fileToolBar;
    QToolBar *editToolBar;

    QStringList recentFiles;
    enum { MaxRecentFiles = 5 };
    QAction *recentFileActions[MaxRecentFiles];
    QAction *separatorAction;

    QString appName;
    QString appVersion;

    QTableWidget *table;
    TxtWindow * htmlWindow;

    QString filePath;

    void fillTable();
    void insertTabRow(int i);
    void updateTable();
    void updateTableRow(int i);
    void clear();
    bool isKeyOK();
    void addWarnings();

    dkCoupletList coupletList;
    dkCoupletList coupletClipboard;
    bool isCopy;
    QString error;
};

#endif // MAINWINDOW_H
