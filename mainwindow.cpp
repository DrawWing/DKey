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

#include <QtGui>

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QFileInfo>
#include <QLineEdit>

#include <QMenuBar>
#include <QFileDialog>
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    appName = QGuiApplication::applicationDisplayName();
    appVersion = QGuiApplication::applicationVersion();;

    filePath = "";
    createActions();
    createMenus();
    readSettings(); //has to be after createActions
    createTable();
    htmlWindow = new TxtWindow(this);
    htmlWindow->hide();
}

void MainWindow::createTable()
{
    table = new QTableWidget(this);
    table->setColumnCount(2);
    table->setWordWrap(true);
    table->setAlternatingRowColors(true);
    QHeaderView *header = table->horizontalHeader();
    header->hide();
    header->setSectionResizeMode(QHeaderView::Stretch);
    QHeaderView *verticalHeader = table->verticalHeader();
    verticalHeader->setSectionResizeMode(QHeaderView::ResizeToContents);

    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(table, SIGNAL( cellDoubleClicked(int, int) ),
            this, SLOT( editClickedRow(int, int) ));

    setCentralWidget(table);
}

void MainWindow::selectTableRow(int row, int col)
{
    table->setCurrentCell(row, col, QItemSelectionModel::Select | QItemSelectionModel::Rows);
}

void MainWindow::viewHtml()
{

    if(htmlWindow->isVisible())
    {
        htmlWindow->raise();
    }
        else
    {
        QFileInfo fileInfo(filePath);
        QString path = fileInfo.absolutePath();
        QString htmlTxt = coupletList.getHtmlTable(path);
        htmlWindow->update(htmlTxt);
//        htmlWindow = new TxtWindow(htmlTxt, this);
        //    htmlWindow->setWindowTitle(appName+" - "+tr("Key")+"[*]");
        htmlWindow->show();
    }
}

void MainWindow::import()
{
    if (!okToContinue())
        return;

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Import File"), filePath, "Text files (*.txt)");
    if( fileName.isEmpty() )
        return;

    importTxt(fileName); // used also by open recent files

}

void MainWindow::importTxt(const QString & fileName)
{
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

    coupletList.importTxt(outTxtList);
//    coupletList.fromTxt(outTxtList);
    error = coupletList.getError();

    QFile outFile("C:/tmp/errors.txt");
    if (!outFile.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning
                (this, appName,
                 tr("Cannot write file %1:\n%2.")
                 .arg(fileName)
                 .arg(outFile.errorString())
                 );
        return;
    }
    QTextStream outStream(&outFile);
    outStream << error;

    /////
    QFileInfo fileInfo(filePath);
    QString path = fileInfo.absolutePath();
    coupletList.findFigs(path);

    fillTable();

    setWindowTitle(QString("%1[*]").arg(fileInfo.fileName()));
    setWindowModified(true);
}

void MainWindow::fillTable()
{
    // clear previous table
    for(int i = 0; i < table->rowCount(); ++i)
        table->removeRow(i);

    for(int i = 0; i < coupletList.size(); ++i)
    {
        insertTabRow(i);
//        dkCouplet theCouplet = coupletList.at(i);

//        table->insertRow(i);

//        QString lead1 = theCouplet.getLead1txt();
//        QTableWidgetItem *item1 = new QTableWidgetItem(lead1);
//        item1->setFlags(item1->flags() ^ Qt::ItemIsEditable);
//        table->setItem(i, 0, item1);

//        QString lead2 = theCouplet.getLead2txt();
//        QTableWidgetItem *item2 = new QTableWidgetItem(lead2);
//        item2->setFlags(item2->flags() ^ Qt::ItemIsEditable);
//        table->setItem(i, 1, item2);
    }
}

void MainWindow::insertTabRow(int i)
{
    dkCouplet theCouplet = coupletList.at(i);

    table->insertRow(i);

    int theNumber = theCouplet.getNumber();
    QString theNumberStr = QString::number(theNumber);
    QTableWidgetItem *item0 = new QTableWidgetItem(theNumberStr);
    item0->setFlags(item0->flags() ^ Qt::ItemIsEditable);
    table->setVerticalHeaderItem(i, item0);

    QString lead1 = theCouplet.getLead1txt();
    QTableWidgetItem *item1 = new QTableWidgetItem(lead1);
    item1->setFlags(item1->flags() ^ Qt::ItemIsEditable);
    table->setItem(i, 0, item1);

    QString lead2 = theCouplet.getLead2txt();
    QTableWidgetItem *item2 = new QTableWidgetItem(lead2);
    item2->setFlags(item2->flags() ^ Qt::ItemIsEditable);
    table->setItem(i, 1, item2);
}

void MainWindow::updateTable()
{
    for(int i = 0; i < coupletList.size(); ++i)
    {
        updateTableRow(i);
//        dkCouplet theCouplet = coupletList.at(i);

//        int theNumber = theCouplet.getNumber();
//        QString theNumberStr = QString::number(theNumber);
//        QTableWidgetItem *item0 = table->verticalHeaderItem(i);
//        item0->setText(theNumberStr);

//        QString lead1 = theCouplet.getLead1txt();
//        QTableWidgetItem *item1 = table->item(i,0);
//        item1->setText(lead1);

//        QString lead2 = theCouplet.getLead2txt();
//        QTableWidgetItem *item2 = table->item(i,1);
//        item2->setText(lead2);
    }
}

// used by updateTable and editRow
void MainWindow::updateTableRow(int i)
{
    dkCouplet theCouplet = coupletList.at(i);

    int theNumber = theCouplet.getNumber();
    QString theNumberStr = QString::number(theNumber);
    QTableWidgetItem *item0 = table->verticalHeaderItem(i);
    item0->setText(theNumberStr);

    QString lead1 = theCouplet.getLead1txt();
    QTableWidgetItem *item1 = table->item(i,0);
    item1->setText(lead1);

    QString lead2 = theCouplet.getLead2txt();
    QTableWidgetItem *item2 = table->item(i,1);
    item2->setText(lead2);
}

QString MainWindow::isSane()
{
    // check double tabs per line

    QString problems;
//    for(unsigned i = 0; i < id.size(); ++i)
//    {
//        QString theLine;
//        if(txtY[i].isEmpty() || txtN[i].isEmpty())
//        {
//            theLine = QStringLiteral("couplet %1: Empty lead.").arg(id[i]);
//            problems.append(theLine);
//        }
//        if(idYto[i] == -1 && txtYto[i].isEmpty())
//        {
//            theLine = QStringLiteral("couplet %1: Empty first reference.").arg(id[i]);
//            problems.append(theLine);
//        }
//        if(idNto[i] == -1 && txtNto[i].isEmpty())
//        {
//            theLine = QStringLiteral("couplet %1: Empty second reference.").arg(id[i]);
//            problems.append(theLine);
//        }
//    }
    return problems;
}

QString MainWindow::errorIsConsequtive() const
{
    QString error;
//    int prevNumber = coupletList[0].getNumber();
//    if(prevNumber != 1)
//        error += "First couplet has number other than 1. \n";
//    for(int i = 1; i < coupletList.size(); ++i)
//    {
//        int currNumber = coupletList[i].getNumber();
//        if(currNumber != prevNumber+1)
//            error += QString("After couplet %1 couplet %2. \n").arg(prevNumber).arg(currNumber);
//    }
    return error;
}

//QString MainWindow::idChain(int coupletNo)
//{
//    QString outString;

////    if(coupletNo > coupletList.size())
////        return outString;
////    int theCouplet = coupletNo;
////    for(int i = theCouplet; i > -1; --i)
////    {
////        if(coupletList[i].getPointer1() == theCouplet ||
////                coupletList[i].getPointer2() == theCouplet)
////        {
////            QString theString = QStringLiteral("%1, ").arg(coupletList[i].getNumber());
////            outString.append(theString);
////            theCouplet = i;
////        }
////    }

//    return outString;
//}

void MainWindow::openFile()
{
    if (!okToContinue())
        return;

    clear();
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open File"), filePath, "dKey files (*.dk.txt)");
    if( fileName.isEmpty() )
        return;

    loadFile(fileName);
}

// used by openFile and open recent files
bool MainWindow::loadFile(const QString & fileName)
{
    clear();

    filePath = fileName;

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
    coupletList.findFigs(path);

    fillTable();

    //proces recent files
    recentFiles.removeAll(fileName);
    recentFiles.prepend(fileName);
    updateRecentFileActions();

    setWindowTitle(QString("%1[*]").arg(fileInfo.fileName()));

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

    return saveFile(fileName);
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
//        if(selectedList.size() > 1 || selectedList.size() == 0)
    {
        QMessageBox::warning
                (this, tr("Insert couplet"), tr("Please select one row."));
        return;
    }
    QTableWidgetSelectionRange selectedRange = selectedList.at(0);
    int selectedCount = selectedRange.rowCount();
    if(selectedCount != 1)
    {
        QMessageBox::warning
                (this, tr("Insert couplet"), tr("Please select one row."));
        return;
    }
    int selectedRow = selectedRange.bottomRow();
    dkCouplet prevCouplet = coupletList.at(selectedRow);
    int prevNumber = prevCouplet.getNumber();
    dkCouplet newCouplet;
//    newCouplet.setFrom(prevNumber);
    newCouplet.setNumber(prevNumber+1); // it is increased secend time by step up
    newCouplet.setLead1(tr("Lead 1."));
    newCouplet.setEndpoint1(tr("Endpoint 1."));
    newCouplet.setLead2(tr("Lead 2."));
    newCouplet.setEndpoint2(tr("Endpoint 2."));

    dkCouplet emptyCouplet;
    coupletList.insertAt(selectedRow+1, emptyCouplet);
    coupletList.setCouplet(newCouplet,selectedRow+1); // too avoid stepup

    insertTabRow(selectedRow+1);
    updateTable();

    setWindowModified(true);
}

void MainWindow::removeRow()
{
    QList<QTableWidgetSelectionRange> selectedList = table->selectedRanges();
    if(selectedList.size() != 1)
    {
        QMessageBox::warning
                (this, tr("Remove couplet"), tr("Please select one row."));
        return;
    }
    QTableWidgetSelectionRange selectedRange = selectedList.at(0);
    int selectedCount = selectedRange.rowCount();
    int topRow = selectedRange.topRow();

    for(int i = selectedCount; i > 0; --i)
    {
        table->removeRow(topRow);
        coupletList.removeAt(topRow);
    }
    updateTable();

    setWindowModified(true);
}

void MainWindow::editRow()
{
    QList<QTableWidgetSelectionRange> selectedList = table->selectedRanges();
    if(selectedList.size() > 1 || selectedList.size() == 0)
    {
        QMessageBox::warning
                (this, tr("Edit couplet"), tr("Please select one row."));
        return;
    }
    QTableWidgetSelectionRange selectedRange = selectedList.at(0);
    int selectedCount = selectedRange.rowCount();
    if(selectedCount > 1 || selectedCount == 0 )
    {
        QMessageBox::warning
                (this, tr("Edit couplet"), tr("Please select one row."));
        return;
    }

    int theRow = selectedRange.topRow();
    dkCouplet theCouplet = coupletList.at(theRow);
    coupletDialog dialog(& theCouplet, theRow+2, coupletList.size()+1, this);
    if (dialog.exec()) {
        // update copletList
        coupletList.setCouplet(theCouplet, theRow);
        // update table
        updateTableRow(theRow);

        setWindowModified(true);
    }
}

void MainWindow::editClickedRow(int row, int col)
{
    dkCouplet theCouplet = coupletList.at(row);
    coupletDialog dialog(& theCouplet, row+2, coupletList.size()+1, this);
    if (dialog.exec()) {
        // update copletList
        coupletList.setCouplet(theCouplet, row);
        // update table
        updateTableRow(row);

        setWindowModified(true);
    }
}

void MainWindow::swapLeads()
{
    QList<QTableWidgetSelectionRange> selectedList = table->selectedRanges();
    if(selectedList.size() > 1 || selectedList.size() == 0)
    {
        QMessageBox::warning
                (this, tr("Edit couplet"), tr("Please select one row."));
        return;
    }
    QTableWidgetSelectionRange selectedRange = selectedList.at(0);
    int selectedCount = selectedRange.rowCount();
    if(selectedCount > 1 || selectedCount == 0 )
    {
        QMessageBox::warning
                (this, tr("Edit couplet"), tr("Please select one row."));
        return;
    }

    int theRow = selectedRange.bottomRow();
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
    openAct = new QAction( tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(openFile()));

    importAct = new QAction( tr("&Import..."), this);
    importAct->setShortcut(tr("Ctrl+I"));
    connect(importAct, SIGNAL(triggered()), this, SLOT(import()));

    saveAct = new QAction( tr("&Save..."), this);
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
//    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    insertRowAct = new QAction(tr("&Insert couplet"), this);
    connect(insertRowAct, SIGNAL(triggered()), this, SLOT(insertRow()));

    removeRowAct = new QAction(tr("&Remove couplet"), this);
    connect(removeRowAct, SIGNAL(triggered()), this, SLOT(removeRow()));

    editRowAct = new QAction(tr("&Edit couplet"), this);
    connect(editRowAct, SIGNAL(triggered()), this, SLOT(editRow()));

    swapLeadsAct = new QAction(tr("&Swap leads"), this);
    connect(swapLeadsAct, SIGNAL(triggered()), this, SLOT(swapLeads()));

    reNumberAct = new QAction(tr("&Renumber key"), this);
    connect(reNumberAct, SIGNAL(triggered()), this, SLOT(reNumber()));

    viewHtmlAct = new QAction(tr("&Interactive key"), this);
    connect(viewHtmlAct, SIGNAL(triggered()), this, SLOT(viewHtml()));

//    viewActGroup = new QActionGroup(this);
//    viewActGroup->addAction(viewTableAct);
//    viewActGroup->addAction(viewHtmlAct);
//    viewActGroup->setExclusive(true);
//    viewTableAct->setChecked(true);

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
    fileMenu->addAction(openAct);
    fileMenu->addAction(importAct);
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
    editMenu->addAction(insertRowAct);
    editMenu->addAction(removeRowAct);
    editMenu->addAction(editRowAct);
    editMenu->addAction(swapLeadsAct);
    editMenu->addAction(reNumberAct);
    menuBar()->addMenu(editMenu);

    viewMenu = new QMenu(tr("&View"), this);
    viewMenu->addAction(viewHtmlAct);
    menuBar()->addMenu(viewMenu);

    helpMenu = new QMenu(tr("&Help"), this);
    helpMenu->addAction(aboutAct);
    menuBar()->addMenu(helpMenu);
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
        int r = QMessageBox::warning(this, appName,
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
