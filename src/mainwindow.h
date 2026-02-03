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
#include <QUndoStack>

#include "dkCouplet.h"
#include "dkCoupletList.h"
#include "dkTermList.h"
//#include "txtwindow.h"
#include "dkFormat.h"
#include "dkBrowser.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool loadFile(const QString &fileName);
    dkFormat * getFormat();
    dkTermList * getGlossary();
    dkTermList * getFigTxt();
    dkBrowser * getHtmlWindow();
    QString getFilePath() const;
    void updateCouplet(dkCouplet & theCouplet, int row);
    void removeCouplets(QList<int> &inList);
    void insertCouplets(QList<int> &inRows, QList<dkCouplet> &inCouplets);
    void insertEmptyCouplet(int inRow);

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
    void viewEndpointList();
    void viewTags();
    void viewIntro();
    void viewGlossary();
    void viewEndpoints();
    void viewFigTxt();
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
    // bool isVersionOK(const QString inVersion);

    QAction *newAct;
    QAction *openAct;
    QAction *importAct;
    QAction *appendAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exportHtmlAct;
    QAction *exitAct;
    QAction *undoAction;
    QAction *redoAction;
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
    QAction *viewEndpointListAct;
    QAction *viewTagsAct;
    QAction *viewIntroAct;
    QAction *viewGlossaryAct;
    QAction *viewEndpointsAct;
    QAction *viewFigTxtAct;
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
    dkBrowser * htmlWindow;

    QString filePath;

    void fillTable();
    void insertTableRow(int i);
    void updateTable();
    void updateTableRow(int i);
    QString exportHtmlImg(bool withPath);
    QString exportHtml();
    QString exportMd();
    void clear();
    bool isKeyOK();
    void addWarnings();

    dkCoupletList coupletList;
    dkCoupletList coupletClipboard;
    bool isCopy;

    dkFormat format;
    dkString intro;
    dkTermList glossary;
    dkTermList endpoints;
    dkTermList figTxt;
    QString error;

    QUndoStack *undoStack;
};

#endif // MAINWINDOW_H
