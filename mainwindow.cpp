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

#include "mainwindow.h"
#include "coupletDialog.h"
#include "dkView.h"

#include <QtGui>

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QFileInfo>
#include <QLineEdit>
#include <QApplication>

#include <QMenuBar>
#include <QFileDialog>
#include <QHeaderView>

#include <QDebug>
#include <QElapsedTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    appName = QGuiApplication::applicationDisplayName();
    appVersion = QGuiApplication::applicationVersion();;

    filePath = "";
    createActions();
    createMenus();
    createToolBars();
    readSettings(); //has to be after createActions
    createTable();
    setWindowTitle("[*]");
    htmlWindow = new TxtWindow(this);
    htmlWindow->hide();
}

void MainWindow::createTable()
{
    table = new QTableWidget(this);
    table->setColumnCount(3);
    table->setWordWrap(true);
    table->setAlternatingRowColors(true);
    table->setStyleSheet("QHeaderView::section { background-color:lightGray }");
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QHeaderView *header = table->horizontalHeader();
    QStringList headerStringList;
    headerStringList<<"No."<<"Lead 1"<<"Lead 2";
    table->setHorizontalHeaderLabels(headerStringList);

    header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(1, QHeaderView::Stretch);
    header->setSectionResizeMode(2, QHeaderView::Stretch);

    QHeaderView *verticalHeader = table->verticalHeader();
    verticalHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
    verticalHeader->hide();

    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(table, SIGNAL( cellDoubleClicked(int, int) ),
            this, SLOT( editClickedRow(int, int) ));

    setCentralWidget(table);
}

void MainWindow::viewBrowser()
{
    if(coupletList.size() == 0)
        return;

//    QFileInfo fileInfo(filePath);
//    QString path = fileInfo.absolutePath();
//    coupletList.setFilePath(path);

    coupletList.findPointerChains();
    coupletList.findEndpoints();
    dkView view(&coupletList, this);
    view.exec();
}

void MainWindow::viewHtml()
{
    if(coupletList.size() == 0)
        return;


//    coupletList.findPointerChains();
//    coupletList.findEndpoints();
//    dkView view(&coupletList, this);
//    view.exec();

    //    if(htmlWindow->isVisible())
    //    {
    //        htmlWindow->raise();
    //    }
    //        else
    //    {

    QString htmlTxt = coupletList.getHtmlTable();
    htmlWindow->setHtml(htmlTxt);
    htmlWindow->setWindowTitle(tr("Interactive key"));
    QApplication::restoreOverrideCursor();
    //    }
}

void MainWindow::import()
{
    if (!okToContinue())
        return;

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Import file"), filePath, "Text files (*.txt)");
    if( fileName.isEmpty() )
        return;

    clear();
    error.clear();
    filePath = fileName;

    QFile inFile(fileName);
    if (!inFile.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, appName,
                             tr("Cannot open file %1:\n%2.")
                             .arg(fileName)
                             .arg(inFile.errorString()));
        return;
    }

    QTextStream inStream(&inFile);
    QStringList outTxtList;
    while (!inStream.atEnd())
    {
        QString line = inStream.readLine();
        outTxtList.push_back(line);
    }
    inFile.close();

    if(outTxtList.size() == 0)
        return;
    coupletList.importTxt(outTxtList);

//    error = coupletList.getError();
//    QFile outFile("C:/tmp/errors.txt");
//    if (!outFile.open(QFile::WriteOnly | QFile::Text))
//    {
//        QMessageBox::warning
//                (this, appName,
//                 tr("Cannot write file %1:\n%2.")
//                 .arg(fileName)
//                 .arg(outFile.errorString())
//                 );
//        return;
//    }
//    QTextStream outStream(&outFile);
//    outStream << error;

    /////
    QFileInfo fileInfo(filePath);
    QString path = fileInfo.absolutePath();
    coupletList.setFilePath(path);
    coupletList.findFigs();

    fillTable();

    setWindowTitle(QString("%1[*]").arg(fileInfo.fileName()));
    setWindowModified(true);
}

void MainWindow::appendFile()
{
    if (!okToContinue())
        return;

    if(coupletList.size() == 0)
        return;

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Append file"), filePath, "dKey files (*.dk.txt)");
    if( fileName.isEmpty() )
        return;

    dkCoupletList appendCouplets;
    appendCouplets.fromDkTxt(fileName);
    QString error = coupletList.getError();
    if(!error.isEmpty())
    {
        error += tr("Use import to read the file. \n");
        QMessageBox::warning(this, tr("Append file"), error);
        return;
    }

    for(int i = 0; i < appendCouplets.size(); ++i)
    {
        int lastRow = coupletList.size();
        coupletList.copyAt(lastRow, appendCouplets.at(i));
        insertTabRow(lastRow);
    }
    setWindowModified(true);
}


void MainWindow::fillTable()
{
    // clear previous table
    for(int i = 0; i < table->rowCount(); ++i)
        table->removeRow(i);

    table->setRowCount(coupletList.size());
    updateTable();
}

void MainWindow::insertTabRow(int i)
{
    table->insertRow(i);
    updateTableRow(i);
}

void MainWindow::updateTable()
{
    for(int i = 0; i < coupletList.size(); ++i)
        updateTableRow(i);
}

// used by updateTable and editRow
void MainWindow::updateTableRow(int i)
{
    dkCouplet theCouplet = coupletList.at(i);

    int theNumber = theCouplet.getNumber();
    QString theNumberStr = QString::number(theNumber);
    table->setItem(i, 0, new QTableWidgetItem(theNumberStr));

    QString lead1 = theCouplet.getLead1txt();
    table->setItem(i, 1, new QTableWidgetItem(lead1));

    QString lead2 = theCouplet.getLead2txt();
    table->setItem(i, 2, new QTableWidgetItem(lead2));
}

void MainWindow::findErrors()
{
    if(coupletList.size() == 0)
        return;

    QString error;
    bool ok = coupletList.isNumberUnique();
    if(!ok)
        error+= coupletList.getError();

    // do no check if not unique
    ok = coupletList.isContentOK();
    if(!ok)
        error+= coupletList.getError();

    ok = coupletList.isEndpointOK();
    if(!ok)
        error+= coupletList.getError();

    ok = coupletList.isPointerOK();
    if(!ok)
        error+= coupletList.getError();

    ok = coupletList.isNumberingOK();
    if(!ok)
        error+= coupletList.getError();

    ok = coupletList.isPointerChainOK();
    if(!ok)
        error+= coupletList.getError();

    ok = coupletList.isFromOK(); // needs to be after isPointerChainOK
    if(!ok)
        error+= coupletList.getError();

    if(error.isEmpty())
        error = tr("No errors was found.");
    htmlWindow->setPlainTxt(error);
    htmlWindow->setWindowTitle(tr("Find errors"));
}

void MainWindow::newKey()
{
    if (!okToContinue())
        return;

    clear();

    coupletList.insertDummyAt(0);
    insertTabRow(0);
    setWindowModified(true);

    // remove file name to prevent save as previous file
    // only file path is left
    QFileInfo fileInfo(filePath);
    filePath = fileInfo.absolutePath();
}


void MainWindow::openFile()
{
    if (!okToContinue())
        return;

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open file"), filePath, "dKey files (*.dk.txt)");
    if( fileName.isEmpty() )
        return;

    loadFile(fileName);
}

// used by openFile and open recent files
bool MainWindow::loadFile(const QString & fileName)
{
//    QElapsedTimer timer;
//    timer.start();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    clear();
    filePath = fileName;

//    qDebug() << timer.elapsed() << "clear";

    coupletList.fromDkTxt(fileName);
    QString error = coupletList.getError();
    if(!error.isEmpty())
    {
        error += tr("Use import to read the file. \n");
        QMessageBox::warning(this, tr("Open file"), error);
        return false;
    }

    QFileInfo fileInfo(filePath);
    QString path = fileInfo.absolutePath();
    coupletList.setFilePath(path);
    coupletList.findFigs();
//    coupletList.findFigs(path);

    fillTable();
    //    qDebug() << timer.elapsed() << "fromDkTxt";

    //proces recent files
    updateRecentFiles(fileName);
    setWindowTitle(QString("%1[*]").arg(fileInfo.fileName()));
    QApplication::restoreOverrideCursor();
    return true;
}

bool MainWindow::save()
{
    if(!isWindowModified())
        return true;
    QFileInfo fileInfo(filePath);
    if (fileInfo.completeSuffix() == "dk.txt")
        return saveFile(filePath);
    else
        return saveAs();
}

bool MainWindow::saveAs()
{
    QFileInfo fileInfo(filePath);
    QString newFileName = filePath;
    if(fileInfo.completeSuffix().toUpper() != "DK.TXT"){
        newFileName = fileInfo.absolutePath()+"/"+fileInfo.baseName() + ".dk.txt";
    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                    newFileName);
    if (fileName.isEmpty())
        return false;

    bool wasSaved = saveFile(fileName);
    if(wasSaved)
        updateRecentFiles(fileName);
    return wasSaved;
}

bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("DKey"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
//    QApplication::setOverrideCursor(Qt::WaitCursor);
    QString outTxt = coupletList.getDkTxt();
    out << outTxt;
//    QApplication::restoreOverrideCursor();

    if(out.status() == QTextStream::WriteFailed)
        return false;

    filePath = fileName;

    setWindowModified(false);

    QFileInfo fileInfo(filePath);
    setWindowTitle(QString("%1[*]").arg(fileInfo.fileName()));

//    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

void MainWindow::exportKey()
{
    if(coupletList.size() == 0)
        return;

    QFileInfo fileInfo(filePath);
    QString outName = fileInfo.path()+"/"+fileInfo.baseName()+".html";
    QString selectedFilter = tr("HTML table (*.html)");
    QString fileName = QFileDialog::getSaveFileName
            (this, tr("Export the key as"), outName,
             tr("HTML table (*.html);;Simple HTML (*.html)"),
             &selectedFilter
             );
    if (fileName.isEmpty())
        return;

    QFile outFile(fileName);
    if (!outFile.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning
                (this, appName,
                 tr("Cannot write file %1:\n%2.")
                 .arg(fileName)
                 .arg(outFile.errorString())
                 );
        return;
    }

    QString htmlTxt;
    if(selectedFilter == "HTML table (*.html)")
        htmlTxt = coupletList.getHtmlTable();
//    htmlTxt = formatHtmlTable();
    else if(selectedFilter == "Simple HTML (*.html)")
        htmlTxt = coupletList.getHtml();

    QTextStream outStream(&outFile);
    outStream << htmlTxt;
    //    statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::insertRow()
{
    QList<QTableWidgetSelectionRange> selectedList = table->selectedRanges();
    if(selectedList.size() != 1)
    {
        QMessageBox::warning
                (this, tr("Insert couplet below"), tr("Please select one row."));
        return;
    }

    QTableWidgetSelectionRange selectedRange = selectedList.at(0);
    int selectedCount = selectedRange.rowCount();
    if(selectedCount != 1)
    {
        QMessageBox::warning
                (this, tr("Insert couplet below"), tr("Please select one row."));
        return;
    }

    int theRow = selectedRange.bottomRow();
    coupletList.insertDummyAt(theRow + 1);
    insertTabRow(theRow + 1);
//    updateTable();
    setWindowModified(true);
}

void MainWindow::removeRow()
{
    QList<QTableWidgetSelectionRange> selectedList = table->selectedRanges();
    if(selectedList.size() == 0)
    {
        QMessageBox::warning
                (this, tr("Remove couplet"), tr("Please select at least one row."));
        return;
    }

    QList<int> toRemove;
    for(int i = 0; i < selectedList.size(); ++i)
    {
        QTableWidgetSelectionRange selectedRange = selectedList.at(i);
        int selectedCount = selectedRange.rowCount();
        int topRow = selectedRange.topRow();

        for(int j = 0; j < selectedCount; ++j)
            toRemove.push_back(topRow);
    }
    // remove from last to first
    std::sort(toRemove.begin(), toRemove.end());
    while(toRemove.size())
    {
        int theRow = toRemove.takeLast();
        table->removeRow(theRow);
        coupletList.removeAt(theRow);
    }

//    updateTable();
    setWindowModified(true);
}

void MainWindow::copyRows()
{
    QList<QTableWidgetSelectionRange> selectedList = table->selectedRanges();
    if(selectedList.size() == 0)
    {
        QMessageBox::warning
                (this, tr("Copy"), tr("Please select at least one row."));
        return;
    }

    coupletClipboard.clear();
    isCopy = true;
    for(int i = 0; i < selectedList.size(); ++i)
    {
        QTableWidgetSelectionRange selectedRange = selectedList.at(i);
        int selectedCount = selectedRange.rowCount();
        int topRow = selectedRange.topRow();

        for(int j = 0; j < selectedCount; ++j)
            coupletClipboard.push_back(coupletList.at(topRow+j));
    }
}

void MainWindow::cutRows()
{
    QList<QTableWidgetSelectionRange> selectedList = table->selectedRanges();
    if(selectedList.size() == 0)
    {
        QMessageBox::warning
                (this, tr("Cut"), tr("Please select at least one row."));
        return;
    }

    coupletClipboard.clear();
    isCopy = false;
    QList<int> toRemove;
    for(int i = 0; i < selectedList.size(); ++i)
    {
        QTableWidgetSelectionRange selectedRange = selectedList.at(i);
        int selectedCount = selectedRange.rowCount();
        int topRow = selectedRange.topRow();

        for(int j = 0; j < selectedCount; ++j)
        {
            coupletClipboard.push_back(coupletList.at(topRow));
            toRemove.push_back(topRow);
        }
    }
    // remove from last to first
    std::sort(toRemove.begin(), toRemove.end());
    while(toRemove.size())
    {
        int theRow = toRemove.takeLast();
        table->removeRow(theRow);
        coupletList.removeAt(theRow);
    }

    setWindowModified(true);
}

void MainWindow::pasteRows()
{
    if(coupletClipboard.size() == 0)
        return;

    QList<QTableWidgetSelectionRange> selectedList = table->selectedRanges();
    if(selectedList.size() != 1)
    {
        QMessageBox::warning
                (this, tr("Paste below"), tr("Please select one row."));
        return;
    }

    QTableWidgetSelectionRange selectedRange = selectedList.at(0);
    int bottomRow = selectedRange.bottomRow();

    for(int i = 0; i < coupletClipboard.size(); ++i)
    {
        if(isCopy)
            coupletList.copyAt(bottomRow+1+i, coupletClipboard.at(i));
        else
            coupletList.insertAt(bottomRow+1+i, coupletClipboard.at(i));
        insertTabRow(bottomRow+1+i);
    }
    if(!isCopy)
        coupletClipboard.clear(); // couplets can be pasted only in one place

//    updateTable();
    setWindowModified(true);
}

void MainWindow::editRow()
{
    QList<QTableWidgetSelectionRange> selectedList = table->selectedRanges();
    if(selectedList.size() != 1)
    {
        QMessageBox::warning
                (this, tr("Edit couplet"), tr("Please select one row."));
        return;
    }
    QTableWidgetSelectionRange selectedRange = selectedList.at(0);
    int selectedCount = selectedRange.rowCount();
    if(selectedCount != 1)
    {
        QMessageBox::warning
                (this, tr("Edit couplet"), tr("Please select one row."));
        return;
    }

    int theRow = selectedRange.topRow();
    editClickedRow(theRow);
}

// col is only for compativility with cellDoubleClicked(row, col)
void MainWindow::editClickedRow(int row, int col)
{
    dkCouplet theCouplet = coupletList.at(row);
//    coupletDialog dialog(& theCouplet, row+2, coupletList.getMaxNumber(), this);
    coupletDialog dialog(& theCouplet, coupletList.getMaxNumber(), this);
    if (dialog.exec()) {
        coupletList.setCouplet(theCouplet, row);
        updateTableRow(row);
        setWindowModified(true);
    }
    --col; // to prevent error messages
}

void MainWindow::swapLeads()
{
    QList<QTableWidgetSelectionRange> selectedList = table->selectedRanges();
    if(selectedList.size() != 1)
    {
        QMessageBox::warning
                (this, tr("Swap leads"), tr("Please select one row."));
        return;
    }
    QTableWidgetSelectionRange selectedRange = selectedList.at(0);
    int selectedCount = selectedRange.rowCount();
    if(selectedCount != 1)
    {
        QMessageBox::warning
                (this, tr("Swap leads"), tr("Please select one row."));
        return;
    }

    int theRow = selectedRange.topRow();
    dkCouplet theCouplet = coupletList.at(theRow);
    theCouplet.swapLeads();

    coupletList.setCouplet(theCouplet, theRow);
    updateTableRow(theRow);
    setWindowModified(true);
}

void MainWindow::reNumber()
{
    coupletList.reNumber();
    updateTable();
    setWindowModified(true);
}

void MainWindow::showSteps()
{
    QList<QTableWidgetSelectionRange> selectedList = table->selectedRanges();
    if(selectedList.size() > 1 || selectedList.size() == 0)
        return;
    QTableWidgetSelectionRange selectedRange = selectedList.at(0);
//    int bottomRow = selectedRange.bottomRow();
    QString steps = ""; // idChain(bottomRow);
    QMessageBox::warning(this, appName, steps.toLatin1(), QMessageBox::Ok);
}

void MainWindow::createActions()
{
    newAct = new QAction(QIcon(":/images/new.png"), tr("&New key"), this);
    newAct->setShortcut(tr("Ctrl+N"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newKey()));

    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(openFile()));

    importAct = new QAction( tr("&Import..."), this);
    importAct->setShortcut(tr("Ctrl+I"));
    connect(importAct, SIGNAL(triggered()), this, SLOT(import()));

    appendAct = new QAction( tr("&Append..."), this);
    connect(appendAct, SIGNAL(triggered()), this, SLOT(appendFile()));

    saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save..."), this);
    saveAct->setShortcut(tr("Ctrl+S"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction( tr("Save &As..."), this);
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    exportHtmlAct = new QAction(tr("&Export"), this);
    connect(exportHtmlAct, SIGNAL(triggered()), this, SLOT(exportKey()));

    showStepsAct = new QAction(tr("Show s&teps"), this);
    connect(showStepsAct, SIGNAL(triggered()), this, SLOT(showSteps()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    insertRowAct = new QAction(QIcon(":/images/insert.png"), tr("&Insert couplet below"), this);
    connect(insertRowAct, SIGNAL(triggered()), this, SLOT(insertRow()));

    removeRowAct = new QAction(QIcon(":/images/remove.png"), tr("&Remove couplet"), this);
    connect(removeRowAct, SIGNAL(triggered()), this, SLOT(removeRow()));

    cutAct = new QAction(QIcon(":/images/cut.png"), tr("Cu&t"), this);
    cutAct->setShortcut(tr("Ctrl+X"));
    connect(cutAct, SIGNAL(triggered()), this, SLOT(cutRows()));

    copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
    copyAct->setShortcut(tr("Ctrl+C"));
    connect(copyAct, SIGNAL(triggered()), this, SLOT(copyRows()));

    pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste below"), this);
    pasteAct->setShortcut(tr("Ctrl+V"));
    connect(pasteAct, SIGNAL(triggered()), this, SLOT(pasteRows()));

    editRowAct = new QAction(QIcon(":/images/edit.png"), tr("&Edit couplet"), this);
    connect(editRowAct, SIGNAL(triggered()), this, SLOT(editRow()));

    swapLeadsAct = new QAction(QIcon(":/images/swap.png"), tr("&Swap leads"), this);
    connect(swapLeadsAct, SIGNAL(triggered()), this, SLOT(swapLeads()));

    renumberAct = new QAction(QIcon(":/images/renumber.png"), tr("&Renumber key"), this);
    connect(renumberAct, SIGNAL(triggered()), this, SLOT(reNumber()));

    findErrorsAct = new QAction(QIcon(":/images/error.png"), tr("&Find errors"), this);
    connect(findErrorsAct, SIGNAL(triggered()), this, SLOT(findErrors()));

    viewBrowserAct = new QAction(QIcon(":/images/interactive.png"), tr("&Key browser"), this);
    connect(viewBrowserAct, SIGNAL(triggered()), this, SLOT(viewBrowser()));

    viewHtmlAct = new QAction(tr("&Hypertext key in table"), this);
    connect(viewHtmlAct, SIGNAL(triggered()), this, SLOT(viewHtml()));

    QString aboutStr = tr("&About %1");
    aboutAct = new QAction(aboutStr.arg(appName), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActions[i] = new QAction(this);
        recentFileActions[i]->setVisible(false);
        connect(recentFileActions[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }

}

void MainWindow::createMenus()
{
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(importAct);
    fileMenu->addAction(appendAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addAction(exportHtmlAct);
//    fileMenu->addAction(showStepsAct);

    separatorAction = fileMenu->addSeparator();
    for (int i = 0; i < MaxRecentFiles; ++i)
        fileMenu->addAction(recentFileActions[i]);
    fileMenu->addSeparator();

    fileMenu->addAction(exitAct);

    menuBar()->addMenu(fileMenu);

    editMenu = new QMenu(tr("&Edit"), this);
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);
    editMenu->addAction(editRowAct);
    editMenu->addAction(insertRowAct);
    editMenu->addAction(removeRowAct);
    editMenu->addAction(swapLeadsAct);
    editMenu->addAction(renumberAct);
    menuBar()->addMenu(editMenu);

    debugMenu = new QMenu(tr("&Debug"), this);
    debugMenu->addAction(findErrorsAct);
    menuBar()->addMenu(debugMenu);

    viewMenu = new QMenu(tr("&View"), this);
    viewMenu->addAction(viewBrowserAct);
    viewMenu->addAction(viewHtmlAct);
    menuBar()->addMenu(viewMenu);

    helpMenu = new QMenu(tr("&Help"), this);
    helpMenu->addAction(aboutAct);
    menuBar()->addMenu(helpMenu);
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("&File"));
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);
    fileToolBar->addAction(findErrorsAct);
    fileToolBar->addAction(viewBrowserAct);

    editToolBar = addToolBar(tr("&Edit"));
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);
    editToolBar->addAction(editRowAct);
    editToolBar->addAction(insertRowAct);
    editToolBar->addAction(removeRowAct);
    editToolBar->addAction(swapLeadsAct);
    editToolBar->addAction(renumberAct);
}

void MainWindow::about()
{
    QString aboutTxt(tr(
                      "<p><b>%1 version %2</b></p>"
                      "<p>Author: Adam Tofilski</p>"
                      "<p>This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.</p>"
                      "<p>This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details. </p>"
                      "<p>You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.</p>"
                      ));
    QMessageBox::about(this, "About",aboutTxt.arg(appName).arg(appVersion));
}

void MainWindow::readSettings()
{
    QString companyName = QGuiApplication::organizationName();
    QSettings settings(companyName, appName);
    recentFiles = settings.value("recentFiles").toStringList();
    updateRecentFileActions();
    filePath = settings.value("filePath").toString();

    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    move(pos);
    resize(size);
}

void MainWindow::writeSettings()
{
    QString companyName = QGuiApplication::organizationName();
    QSettings settings(companyName, appName);
    settings.setValue("recentFiles", recentFiles);
    settings.setValue("filePath", filePath);

    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

void MainWindow::updateRecentFiles(const QString &fileName)
{
    recentFiles.removeAll(fileName);
    recentFiles.prepend(fileName);
}

void MainWindow::updateRecentFileActions()
{
    QMutableStringListIterator i(recentFiles);
    while (i.hasNext()) {
        if (!QFile::exists(i.next()))
            i.remove();
    }
    for (int j = 0; j < MaxRecentFiles; ++j) {
        if (j < recentFiles.count()) {
            QString text = tr("&%1 %2")
                    .arg(j + 1)
                    .arg(QFileInfo(recentFiles[j]).fileName()); //strippedName(recentFiles[j]));
            recentFileActions[j]->setText(text);
            recentFileActions[j]->setData(recentFiles[j]);
            recentFileActions[j]->setVisible(true);
        } else {
            recentFileActions[j]->setVisible(false);
        }
    }
    separatorAction->setVisible(!recentFiles.isEmpty());
}

void MainWindow::openRecentFile()
{
    if (okToContinue()) {
        QAction *action = qobject_cast<QAction *>(sender());
        if (action)
            loadFile(action->data().toString());
    }
}

void MainWindow::closeEvent(QCloseEvent *event) //unreferenced event needst to be here
{
    if (okToContinue()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

bool MainWindow::okToContinue()
{
    if (isWindowModified()) {
        int r = QMessageBox::warning(this, "Save changes?",
                                     tr("The key has been modified.\n""Do you want to save changes?"),
                                     QMessageBox::Yes | QMessageBox::Default,
                                     QMessageBox::No,
                                     QMessageBox::Cancel | QMessageBox::Escape);
        if (r == QMessageBox::Yes) {
            return save();
        } else if (r == QMessageBox::No) {
            setWindowModified(false);
            return true;
        } else if (r == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

void MainWindow::clear()
{
//    table->clear(); //does not work
    table->deleteLater();
    createTable();

    coupletList.clear();
    htmlWindow->hide();
}

MainWindow::~MainWindow()
{

}
