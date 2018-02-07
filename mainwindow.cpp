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

#include <QtGui>
#include <QFile>
#include <QTextStream>
//#include <QDebug>
#include <QFileInfo>
#include <QApplication>
#include <QMenuBar>
//#include <QStatusBar>
#include <QFileDialog>
#include <QHeaderView>
#include <QInputDialog>

//#include <QElapsedTimer>

#include "mainwindow.h"
#include "coupletDialog.h"
#include "dkView.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    appName = QGuiApplication::applicationDisplayName();
    appVersion = QGuiApplication::applicationVersion();

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

void MainWindow::viewBrowser()
{
    if(coupletList.size() == 0)
        return;

    coupletList.findFigs();
    coupletList.findPointerChains();
    coupletList.findEndpoints();
    coupletList.findTags();
    if(!isKeyOK())
    {
        findErrors();
        return;
    }

    dkView * view = new dkView(&coupletList, this);
    view->showMaximized();
}

void MainWindow::viewHtml()
{
    if(coupletList.size() == 0)
        return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QString htmlTxt = coupletList.getHtmlImg();
    htmlWindow->setHtml(htmlTxt);
    htmlWindow->setWindowTitle(tr("Hypertext viewer"));
    QApplication::restoreOverrideCursor();
}

void MainWindow::viewEndpoints()
{
    if(coupletList.size() == 0)
        return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    coupletList.findEndpoints();
    QStringList endpointList = coupletList.getEndpointList();

    QString outTxt;
    if(endpointList.size() > 0)
    {
        outTxt += "List of endpoints:\n";
        for(int i = 0; i < endpointList.size(); ++i)
            outTxt += endpointList[i] + "\n";
    } else
        outTxt += "No endpoints was found.";

    htmlWindow->setPlainTxt(outTxt);
    htmlWindow->setWindowTitle(tr("Hypertext viewer"));
    QApplication::restoreOverrideCursor();
}

void MainWindow::viewTags()
{
    if(coupletList.size() == 0)
        return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QStringList tagList = coupletList.findTags();

    QString outTxt;
    if(tagList.size() > 0)
    {
        outTxt += "List of tags:\n";
        for(int i = 0; i < tagList.size(); ++i)
            outTxt += tagList[i] + "\n";
    } else
        outTxt += "No tags was found.";
    htmlWindow->setPlainTxt(outTxt);
    htmlWindow->setWindowTitle(tr("Hypertext viewer"));
    QApplication::restoreOverrideCursor();
}

void MainWindow::import()
{
    if (!okToContinue())
        return;

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Import file"), filePath, tr("Text files (*.txt)"));
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
    inStream.setCodec("UTF-8");
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

    if(coupletList.size() == 0)
        return;

    QFileInfo fileInfo(filePath);
    QString path = fileInfo.absolutePath();
    coupletList.setFilePath(path);

    fillTable();

    setWindowTitle(QString("%1[*]").arg(fileInfo.fileName()));
    setWindowModified(true);
}

void MainWindow::appendFile()
{
    if(coupletList.size() == 0)
        return;

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Append file"), filePath, tr("dKey files (*.dk.xml)"));
    if( fileName.isEmpty() )
        return;

///
    QDomDocument xmlDoc;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(this, tr("Warning"), tr("Cannot open %1.").arg(file.fileName()));
        return ;
    }

    QString errorStr;
    int errorLine;
    int errorColumn;
    if (!xmlDoc.setContent(&file, false, &errorStr, &errorLine, &errorColumn)) {
        file.close();
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(this, QObject::tr("DOM Parser"),
                             QObject::tr("Parse error at line %1, column %2:\n%3")
                             .arg(errorLine)
                             .arg(errorColumn)
                             .arg(errorStr));
        return;
    }


    file.close();
///
    dkCoupletList appendCouplets;
    appendCouplets.fromDkXml(xmlDoc);
    QString error = coupletList.getError();
    if(!error.isEmpty())
    {
        error += tr("Use import to read the file. \n");
        QMessageBox::warning(this, tr("Append file"), error);
        return;
    }

    int startNumber = coupletList.getMaxNumber();
    appendCouplets.reNumber(startNumber+1);
    for(int i = 0; i < appendCouplets.size(); ++i)
    {
        int lastRow = coupletList.size();
        coupletList.insertAt(lastRow, appendCouplets.at(i));
        insertTabRow(lastRow);
    }
    setWindowModified(true);
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
    headerStringList<<tr("No.")<<tr("First lead")<<tr("Second lead");
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

void MainWindow::fillTable()
{
    // clear previous table
    while(table->rowCount() > 0)
        table->removeRow(0);

    for(int i = 0; i < coupletList.size(); ++i)
        insertTabRow(i);
}

void MainWindow::insertTabRow(int i)
{
    table->insertRow(i);

    dkCouplet theCouplet = coupletList.at(i);

    int theNumber = theCouplet.getNumber();
    QString theNumberStr = QString::number(theNumber);
    table->setItem(i, 0, new QTableWidgetItem(theNumberStr));

    QString lead1 = theCouplet.getLead1txt();
    table->setItem(i, 1, new QTableWidgetItem(lead1));
    table->item(i,1)->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);

    QString lead2 = theCouplet.getLead2txt();
    table->setItem(i, 2, new QTableWidgetItem(lead2));
    table->item(i,2)->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);

}

void MainWindow::updateTable()
{
    for(int i = 0; i < coupletList.size(); ++i)
        updateTableRow(i);
}

// used by updateTable and editRow
void MainWindow::updateTableRow(int i)
{
    // it is faster to remove the row and insert a new one
    table->removeRow(i);
    insertTabRow(i);
}

bool MainWindow::isKeyOK()
{
    if(coupletList.size() == 0)
        return true;

    error.clear();
    bool ok = coupletList.isNumberUnique();
    if(!ok)
        error+= coupletList.getError();

    ok = coupletList.isNumberingOK();
    if(!ok)
        error+= coupletList.getError();

    // do no check if not unique and consequtive
    ok = coupletList.isContentOK();
    if(!ok)
        error+= coupletList.getError();

    ok = coupletList.isPointerOK();
    if(!ok)
        error+= coupletList.getError();

    ok = coupletList.isPointerChainOK();
    if(!ok)
        error+= coupletList.getError();

    if(error.isEmpty())
        return true;
    else
        return false;
}

void MainWindow::addWarnings()
{
    if(coupletList.size() == 0)
        return;

    bool ok = coupletList.isFromSingle();
    if(!ok)
        error+= coupletList.getError();

    ok = coupletList.isEndpointOK();
    if(!ok)
        error+= coupletList.getError();

    ok = coupletList.isPointerNoWarning();
    if(!ok)
        error+= coupletList.getError();

    ok = coupletList.isKeyCyclic(); // needs to be after isPointerChainOK
    if(!ok)
        error+= coupletList.getError();

}

void MainWindow::findErrors()
{
    if(coupletList.size() == 0)
        return;

    isKeyOK();
    addWarnings();
    if(error.isEmpty())
        error = tr("No errors was found.");
    htmlWindow->setPlainTxt(error);
    htmlWindow->setWindowTitle(tr("Find errors"));
}

void MainWindow::findImgErrors()
{
    if(coupletList.size() == 0)
        return;

    QStringList figList = coupletList.findFigs();
    error = coupletList.getError();

    ///
    QFileInfo fileInfo(filePath);
    QString path = fileInfo.absolutePath();
    QDir inDir( path );
    QFileInfoList localFileInfoList = inDir.entryInfoList(QStringList("fig-*"), QDir::Files, QDir::Name );
    for (int i = 0; i < localFileInfoList.size(); ++i) {
        QFileInfo localFileInfo = localFileInfoList.at(i);
        QString fileName = localFileInfo.fileName();
        if(!figList.contains(fileName))
            error += QString("Image %1 not referenced in the key.\n").arg(fileName);
    }
    ///

    if(error.isEmpty())
        error = tr("No image errors was found.");
    htmlWindow->setPlainTxt(error);
    htmlWindow->setWindowTitle(tr("Find image errors"));
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
                                                    tr("Open file"), filePath, tr("DKey files (*.dk.xml)"));
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

    /// move to function used by append and load
    QDomDocument xmlDoc;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(this, tr("Warning"), tr("Cannot open %1.").arg(file.fileName()));
        return false;
    }

    QString errorStr;
    int errorLine;
    int errorColumn;
    if (!xmlDoc.setContent(&file, false, &errorStr, &errorLine, &errorColumn)) {
        file.close();
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(this, QObject::tr("DOM Parser"),
                             QObject::tr("Parse error at line %1, column %2:\n%3")
                             .arg(errorLine)
                             .arg(errorColumn)
                             .arg(errorStr));
        return false;
    }


    file.close();
    ///

    coupletList.fromDkXml(xmlDoc);

    QApplication::restoreOverrideCursor();

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

    fillTable();
    //    qDebug() << timer.elapsed() << "fromDkTxt";

    updateRecentFiles(fileName);
    updateRecentFileActions();

    setWindowTitle(QString("%1[*]").arg(fileInfo.fileName()));
    return true;
}

bool MainWindow::save()
{
    if(!isWindowModified())
        return true;
    QFileInfo fileInfo(filePath);
    if (fileInfo.completeSuffix() == "dk.xml")
        return saveFile(filePath);
    else
        return saveAs();
}

bool MainWindow::saveAs()
{
    if(coupletList.size() == 0)
        return false;

    QFileInfo fileInfo(filePath);
    QString newFileName = fileInfo.absolutePath()+"/"+fileInfo.baseName();

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                    newFileName, tr("DKey files (*.dk.xml)"));
    if (fileName.isEmpty())
        return false;

    bool wasSaved = saveFile(fileName);
    if(wasSaved)
    {
        updateRecentFiles(fileName);
        updateRecentFileActions();

        QFileInfo fileInfo(filePath);
        QString path = fileInfo.absolutePath();
        coupletList.setFilePath(path);
    }
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
    out.setCodec("UTF-8");

    QString outTxt = coupletList.getDkXml();
    out << outTxt;

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
    QString outName = fileInfo.path()+"/"+fileInfo.baseName()+".rtf";
    QString selectedFilter = tr("Formated text RTF (*.rtf)");
    QString fileName = QFileDialog::getSaveFileName
            (this, tr("Export the key as"), outName,
             tr("Formated text RTF (*.rtf);;Simple HTML (*.html);;HTML table (*.html);;HTML with images (*.html)"),
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
    if(selectedFilter == "Formated text RTF (*.rtf)")
        htmlTxt = coupletList.getRtf();
    else if(selectedFilter == "Simple HTML (*.html)")
        htmlTxt = coupletList.getHtml();
    else if(selectedFilter == "HTML table (*.html)")
        htmlTxt = coupletList.getHtmlTab();
    else if(selectedFilter == "HTML with images (*.html)")
        htmlTxt = coupletList.getHtmlImg(false);

    QTextStream outStream(&outFile);
    if(selectedFilter != "Formated text RTF (*.rtf)")
        outStream.setCodec("UTF-8");

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
            coupletClipboard.push_back(coupletList.at(topRow+j));
            toRemove.push_back(topRow+j);
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

    if(isCopy)
    {
        int startNumber = coupletList.getMaxNumber();
        coupletClipboard.reNumber(startNumber+1);
    }

    for(int i = 0; i < coupletClipboard.size(); ++i)
    {
        coupletList.insertAt(bottomRow+1+i, coupletClipboard.at(i));
        insertTabRow(bottomRow+1+i);
    }
    if(!isCopy)
        coupletClipboard.clear(); // couplets can be pasted only in one place

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
    int max = coupletList.getMaxNumber();
    if(theCouplet.getPointer1() > max || theCouplet.getPointer2() > max)
    {
        if(theCouplet.getPointer1() > max )
            theCouplet.setPointer1(1);
        if(theCouplet.getPointer2() > max )
            theCouplet.setPointer2(1);
        QMessageBox::warning
                (this, tr("Edit couplet"), tr("Incorect pointer was changed to 1. Please correct it."));
    }

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
    if(coupletList.size() == 0)
        return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    coupletList.reNumber();
    updateTable();
    setWindowModified(true);
    QApplication::restoreOverrideCursor();
}

void MainWindow::arrange()
{
    if(coupletList.size() == 0)
        return;

    if(!isKeyOK())
    {
        findErrors();
        return;
    }
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    coupletList.arrange();
    fillTable();
    setWindowModified(true);
    QApplication::restoreOverrideCursor();
}

void MainWindow::goToNumber()
{
    if(coupletList.size() == 0)
        return;

    int max = coupletList.getMaxNumber();
    bool ok;
    int inNumber = QInputDialog::getInt(this, tr("Go to couplet"), tr("Go to couplet number:"),
                                              1, 1, max, 1, &ok);
    if(!ok)
        return;

    int index = coupletList.getIndexWithNumber(inNumber);
    table->selectRow(index);
}

void MainWindow::goToTag()
{
    QStringList tagList = coupletList.findTags();
    if(tagList.size() == 0)
        return;

    bool ok;
    QString inString = QInputDialog::getItem(this, tr("Go to tag"), tr("Go to tag:"),
                                              tagList, 0, false, &ok);
    if(!ok)
        return;

    int inNumber = coupletList.getLastNumber(inString);
    if( inNumber == -1 )
        return;
    int index = coupletList.getIndexWithNumber(inNumber);
    table->selectRow(index);
}

void MainWindow::createActions()
{
    newAct = new QAction(QIcon(":/images/new.png"), tr("&New key"), this);
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

    arrangeAct = new QAction(tr("&Arrange couplets"), this);
    connect(arrangeAct, SIGNAL(triggered()), this, SLOT(arrange()));

    findErrorsAct = new QAction(QIcon(":/images/error.png"), tr("&Find errors"), this);
    findErrorsAct->setShortcut(tr("Ctrl+E"));
    connect(findErrorsAct, SIGNAL(triggered()), this, SLOT(findErrors()));

    findImgErrorsAct = new QAction(tr("&Find image errors"), this);
    connect(findImgErrorsAct, SIGNAL(triggered()), this, SLOT(findImgErrors()));

    viewBrowserAct = new QAction(QIcon(":/images/interactive.png"), tr("&Key browser"), this);
    connect(viewBrowserAct, SIGNAL(triggered()), this, SLOT(viewBrowser()));

    viewHtmlAct = new QAction(tr("&Hypertext in table"), this);
    connect(viewHtmlAct, SIGNAL(triggered()), this, SLOT(viewHtml()));

    viewEndpointsAct = new QAction(tr("&Endpoints"), this);
    connect(viewEndpointsAct, SIGNAL(triggered()), this, SLOT(viewEndpoints()));

    viewTagsAct = new QAction(tr("&Tags"), this);
    connect(viewTagsAct, SIGNAL(triggered()), this, SLOT(viewTags()));

    goToNumberAct = new QAction(tr("&Go to &number"), this);
    goToNumberAct->setShortcut(tr("Ctrl+N"));
    connect(goToNumberAct, SIGNAL(triggered()), this, SLOT(goToNumber()));

    goToTagAct = new QAction(tr("Go to &tag"), this);
    goToTagAct->setShortcut(tr("Ctrl+T"));
    connect(goToTagAct, SIGNAL(triggered()), this, SLOT(goToTag()));

    QString aboutStr = tr("&About %1");
    aboutAct = new QAction(aboutStr.arg(appName), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
//    connect(aboutAct, SIGNAL(triggered()), this, SLOT(test()));

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
    editMenu->addAction(arrangeAct);
    menuBar()->addMenu(editMenu);

    debugMenu = new QMenu(tr("&Debug"), this);
    debugMenu->addAction(findErrorsAct);
    debugMenu->addAction(findImgErrorsAct);
    menuBar()->addMenu(debugMenu);

    viewMenu = new QMenu(tr("&View"), this);
    viewMenu->addAction(viewBrowserAct);
    viewMenu->addAction(viewHtmlAct);
    viewMenu->addAction(viewEndpointsAct);
    viewMenu->addAction(viewTagsAct);
    menuBar()->addMenu(viewMenu);

    navMenu = new QMenu(tr("&Navigation"), this);
    navMenu->addAction(goToNumberAct);
    navMenu->addAction(goToTagAct);
    menuBar()->addMenu(navMenu);

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
                         "<p>Home page: <a href=\"http://drawwing.org/dkey\">drawwing.org/dkey</a></p>"
                         "<p>If you find this software useful please cite it:<br>Tofilski A (2018) DKey software for editing and browsing dichotomous keys. ZooKeys 735: 131-140. <a href=\"https://doi.org/10.3897/zookeys.735.21412\">https://doi.org/10.3897/zookeys.735.21412</a></p>"
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
        int r = QMessageBox::warning(this, tr("Save changes?"),
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
    while(table->rowCount() > 0)
        table->removeRow(0);

    coupletList.clear();
    htmlWindow->hide();
}

MainWindow::~MainWindow()
{
}

void MainWindow::test()
{
    if(coupletList.size() == 0)
        return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    coupletList.findEndpoints();
    QStringList endpointList = coupletList.getEndpointList();
    QString outTxt;
    for(int i = 0; i < endpointList.size(); ++i)
        outTxt += endpointList[i] + "\n";
    htmlWindow->setPlainTxt(outTxt);
    htmlWindow->setWindowTitle(tr("Hypertext viewer"));
    QApplication::restoreOverrideCursor();
}

