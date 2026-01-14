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

#include <QtWidgets>
#include <QHeaderView>
#include <QAction>
#include <QMenu>
#include <QFileInfo>

#include "termWindow.h"
#include "termDialog.h"

termWindow::termWindow(dkTermList *inList, MainWindow *inParent)
    : QMainWindow(inParent)
{
    parent = inParent;
    setAttribute(Qt::WA_DeleteOnClose);
    readSettings();

    // termList = inList;
    if(termList.size() == 0)
        termList.insertDummyAt(0);

    createActions();
    createMenus();
    createToolBars();
    createTable();
    fillTable();
}


termWindow::termWindow(QString &fileName, MainWindow *inParent)
    : QMainWindow(inParent)
{
    filePath = fileName;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QDomDocument xmlDoc;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(this, tr("Warning"), tr("Cannot open %1.").arg(file.fileName()));
        return;
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
    QString elementName = "DKey";
    QDomNode dkeyNode = xmlDoc.namedItem(elementName);
    QDomElement dkeyElement = dkeyNode.toElement();
    if ( dkeyElement.isNull() )
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(this, QObject::tr("DOM Parser"),QObject::tr("No <%1> element found in the XML file!").arg(elementName));
        return;
    }
    // QString versionTxt = dkeyElement.attribute("version");
    // if(!isVersionOK(versionTxt))
    // {
    //     QApplication::restoreOverrideCursor();
    //     QMessageBox::warning(this, QObject::tr("DOM Parser"),QObject::tr("The file was saved in newer version of DKey.\nPlease download the most recent version."));
    //     QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    // }

    // load terminology
    QDomElement glossaryElement = dkeyNode.namedItem("glossary").toElement();
    terminology.fromDkXml(glossaryElement);
    termList = terminology;  // remove terminology ????????????
    termList.setTag("glossary");
    // format.setGlossary(&termList);

    QApplication::restoreOverrideCursor();

    ////
    parent = inParent;
    setAttribute(Qt::WA_DeleteOnClose);
    readSettings();

    // termList = inList;
    if(termList.size() == 0)
        termList.insertDummyAt(0);

    createActions();
    createMenus();
    createToolBars();
    createTable();
    fillTable();
}

void termWindow::createActions()
{
    newAct = new QAction(QIcon(":/images/new.png"), tr("&New terms"), this);
    connect(newAct, SIGNAL(triggered()), this, SLOT(newTerms()));

    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(openFile()));

    importAct = new QAction( tr("&Import..."), this);
    importAct->setShortcut(tr("Ctrl+I"));
    connect(importAct, SIGNAL(triggered()), this, SLOT(import()));

    saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save..."), this);
    saveAct->setShortcut(tr("Ctrl+S"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    exportAct = new QAction( tr("&Export..."), this);
    exportAct->setShortcut(tr("Ctrl+E"));
    connect(exportAct, SIGNAL(triggered()), this, SLOT(exportTxt()));

    insertRowAct = new QAction(QIcon(":/images/insert.png"), tr("&Insert row below"), this);
    connect(insertRowAct, SIGNAL(triggered()), this, SLOT(insertRow()));

    removeRowAct = new QAction(QIcon(":/images/remove.png"), tr("&Remove row"), this);
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

    editRowAct = new QAction(QIcon(":/images/edit.png"), tr("&Edit"), this);
    connect(editRowAct, SIGNAL(triggered()), this, SLOT(editRow()));

    sortTableAct = new QAction(tr("&Sort"), this);
    connect(sortTableAct, SIGNAL(triggered()), this, SLOT(sortTable()));

}

void termWindow::createMenus()
{
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(importAct);
    fileMenu->addAction(exportAct);

    editMenu = new QMenu(tr("&Edit"), this);
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);
    editMenu->addAction(editRowAct);
    editMenu->addAction(insertRowAct);
    editMenu->addAction(removeRowAct);
    editMenu->addAction(sortTableAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(editMenu);
}

void termWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("&File"));
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);

    editToolBar = addToolBar(tr("&Edit"));
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);
    editToolBar->addAction(editRowAct);
    editToolBar->addAction(insertRowAct);
    editToolBar->addAction(removeRowAct);
}

void termWindow::createTable()
{
    table = new QTableWidget(this);
    table->setColumnCount(2);
    table->setWordWrap(true);
    table->setAlternatingRowColors(true);
    table->setStyleSheet("QHeaderView::section { background-color:lightGray }");
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QHeaderView *header = table->horizontalHeader();
    QStringList headerStringList;
    headerStringList<<tr("No.")<<tr("Term");
    table->setHorizontalHeaderLabels(headerStringList);

    header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(1, QHeaderView::Stretch);

    QHeaderView *verticalHeader = table->verticalHeader();
    verticalHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
    verticalHeader->hide();

    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(table, SIGNAL( cellDoubleClicked(int, int) ),
            this, SLOT( editClickedRow(int, int) ));

    setCentralWidget(table);
}

void termWindow::fillTable()
{
    // clear previous table
    while(table->rowCount() > 0)
        table->removeRow(0);

    for(int i = 0; i < termList.size(); ++i)
        insertTableRow(i);
}

void termWindow::insertTableRow(int i)
{
    table->insertRow(i);

    dkTerm theTerm = termList.at(i);

//    int theNumber = theTerm.getNumber();
    QString theNumberStr = QString::number(i+1);
    table->setItem(i, 0, new QTableWidgetItem(theNumberStr));

    QString termTxt = theTerm.getTxt();
    table->setItem(i, 1, new QTableWidgetItem(termTxt));
    table->item(i,1)->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
}

void termWindow::updateTable()
{
    for(int i = 0; i < termList.size(); ++i)
        updateTableRow(i);
}

// used by updateTable and editRow
void termWindow::updateTableRow(int i)
{
    // it is faster to remove the row and insert a new one
    table->removeRow(i);
    insertTableRow(i);
}

void termWindow::insertRow()
{
    QList<QTableWidgetSelectionRange> selectedList = table->selectedRanges();
    if(selectedList.size() != 1)
    {
        QMessageBox::warning
                (this, tr("Insert row below"), tr("Please select one row."));
        return;
    }

    QTableWidgetSelectionRange selectedRange = selectedList.at(0);
    int selectedCount = selectedRange.rowCount();
    if(selectedCount != 1)
    {
        QMessageBox::warning
                (this, tr("Insert row below"), tr("Please select one row."));
        return;
    }

    int theRow = selectedRange.bottomRow();
    termList.insertDummyAt(theRow + 1);
    insertTableRow(theRow + 1);
    setWindowModified(true);
}

void termWindow::removeRow()
{
    QList<QTableWidgetSelectionRange> selectedList = table->selectedRanges();
    if(selectedList.size() == 0)
    {
        QMessageBox::warning
                (this, tr("Remove row"), tr("Please select at least one row."));
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
        termList.removeAt(theRow);
    }

    setWindowModified(true);
}

void termWindow::copyRows()
{
    QList<QTableWidgetSelectionRange> selectedList = table->selectedRanges();
    if(selectedList.size() == 0)
    {
        QMessageBox::warning
                (this, tr("Copy"), tr("Please select at least one row."));
        return;
    }

    termClipboard.clear();
    isCopy = true;
    for(int i = 0; i < selectedList.size(); ++i)
    {
        QTableWidgetSelectionRange selectedRange = selectedList.at(i);
        int selectedCount = selectedRange.rowCount();
        int topRow = selectedRange.topRow();

        for(int j = 0; j < selectedCount; ++j)
            termClipboard.push_back(termList.at(topRow+j));
    }
}

void termWindow::cutRows()
{
    QList<QTableWidgetSelectionRange> selectedList = table->selectedRanges();
    if(selectedList.size() == 0)
    {
        QMessageBox::warning
                (this, tr("Cut"), tr("Please select at least one row."));
        return;
    }

    termClipboard.clear();
    isCopy = false;
    QList<int> toRemove;
    for(int i = 0; i < selectedList.size(); ++i)
    {
        QTableWidgetSelectionRange selectedRange = selectedList.at(i);
        int selectedCount = selectedRange.rowCount();
        int topRow = selectedRange.topRow();

        for(int j = 0; j < selectedCount; ++j)
        {
            termClipboard.push_back(termList.at(topRow+j));
            toRemove.push_back(topRow+j);
        }
    }
    // remove from last to first
    std::sort(toRemove.begin(), toRemove.end());
    while(toRemove.size())
    {
        int theRow = toRemove.takeLast();
        table->removeRow(theRow);
        termList.removeAt(theRow);
    }

    setWindowModified(true);
}

void termWindow::pasteRows()
{
    if(termClipboard.size() == 0)
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

//    if(isCopy)
//    {
//        int startNumber = termList.getMaxNumber();
//        termClipboard.reNumber(startNumber+1);
//    }

    for(int i = 0; i < termClipboard.size(); ++i)
    {
        termList.insertAt(bottomRow+1+i, termClipboard.at(i));
        insertTableRow(bottomRow+1+i);
    }
    if(!isCopy)
        termClipboard.clear(); // rows can be pasted only in one place

    setWindowModified(true);
}

void termWindow::editRow()
{
    QList<QTableWidgetSelectionRange> selectedList = table->selectedRanges();
    if(selectedList.size() != 1)
    {
        QMessageBox::warning
                (this, tr("Edit row"), tr("Please select one row."));
        return;
    }
    QTableWidgetSelectionRange selectedRange = selectedList.at(0);
    int selectedCount = selectedRange.rowCount();
    if(selectedCount != 1)
    {
        QMessageBox::warning
                (this, tr("Edit row"), tr("Please select one row."));
        return;
    }

    int theRow = selectedRange.topRow();
    editClickedRow(theRow);
}

// col is only for compatibility with cellDoubleClicked(row, col)
void termWindow::editClickedRow(int row, int col)
{
    dkTerm theTerm = termList.at(row);

    termDialog dialog(& theTerm, this);
    if (dialog.exec()) {
        termList.setTerm(theTerm, row);
        updateTableRow(row);
        table->selectRow(row);
        setWindowModified(true);
    }
    --col; // to prevent error messages
}

void termWindow::sortTable()
{
    if(termList.size() == 0)
        return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    termList.sort();
    updateTable();
    setWindowModified(true);
    QApplication::restoreOverrideCursor();
}

void termWindow::closeEvent(QCloseEvent *event)
{
    // if(isWindowModified())
    //     parent->setWindowModified(true);
    writeSettings();
    hide();
    event->ignore();
    //    event->accept();
}

bool termWindow::okToContinue()
{
    if (isWindowModified()) {
        int r = QMessageBox::warning(this, tr("Save changes?"),
                                     tr("The list has been modified.\n""Do you want to save changes?"),
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

void termWindow::clear()
{
    while(table->rowCount() > 0)
        table->removeRow(0);

    termList.clear();
    // htmlWindow->hide();
}

void termWindow::readSettings()
{
    QString companyName = QGuiApplication::organizationName();
    QString windowName = QGuiApplication::applicationName() + " - Terms";
    QSettings settings(companyName, windowName);
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    move(pos);
    resize(size);
}

void termWindow::writeSettings()
{
    QString companyName = QGuiApplication::organizationName();
    QString windowName = QGuiApplication::applicationName() + " - Terms";
    QSettings settings(companyName, windowName);
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

void termWindow::newTerms()
{
    if (!okToContinue())
        return;

    clear();

    termList.insertDummyAt(0);
    insertTableRow(0);
    setWindowModified(true);

    // remove file name to prevent save as previous file
    // only file path is left
    QFileInfo fileInfo(filePath);
    filePath = fileInfo.absolutePath();
}

void termWindow::openFile()
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
bool termWindow::loadFile(const QString & fileName)
{

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    // clear();
    while(table->rowCount() > 0)
        table->removeRow(0);

    filePath = fileName;

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
    QString elementName = "DKey";
    QDomNode dkeyNode = xmlDoc.namedItem(elementName);
    QDomElement dkeyElement = dkeyNode.toElement();
    if ( dkeyElement.isNull() )
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(this, QObject::tr("DOM Parser"),QObject::tr("No <%1> element found in the XML file!").arg(elementName));
        return false;
    }
    QString versionTxt = dkeyElement.attribute("version");
    if(!parent->isVersionOK(versionTxt))
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(this, QObject::tr("DOM Parser"),QObject::tr("The file was saved in newer version of DKey.\nPlease download the most recent version."));
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    }

    QApplication::restoreOverrideCursor();

    QFileInfo fileInfo(filePath);
    QString path = fileInfo.absolutePath();
    // format.setFilePath(path);

    // updateRecentFiles(fileName);
    // updateRecentFileActions();
    setWindowTitle(QString("%1[*]").arg(fileInfo.fileName()));

    // load glossary
    QDomElement termsElement = dkeyNode.namedItem("term_list").toElement();
    termList.fromDkXml(termsElement);
    QString tagTxt = termsElement.attribute("tag");
    termList.setTag(tagTxt);
    // format.setGlossary(&glossary);

    fillTable();

    return true;
}

bool termWindow::save()
{
    if(!isWindowModified())
        return true;
    QFileInfo fileInfo(filePath);
    if (fileInfo.completeSuffix() == "dk.xml")
        return saveFile(filePath);
    else
        return saveAs();
}

bool termWindow::saveAs()
{
    if(termList.size() == 0)
        return false;

    QFileInfo fileInfo(filePath);
    QString newFileName = fileInfo.absolutePath()+"/"+fileInfo.baseName();

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                    newFileName, tr("DKey files (*.dk.xml)"));
    if (fileName.isEmpty())
        return false;

    bool wasSaved = saveFile(fileName);
    // if(wasSaved)
    // {
    //     updateRecentFiles(fileName);
    //     updateRecentFileActions();

    //     QFileInfo fileInfo(filePath);
    //     QString path = fileInfo.absolutePath();
    //     coupletList.setFilePath(path);
    // }
    return wasSaved;
}

bool termWindow::saveFile(const QString &fileName)
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
    // qt6   out.setEncoding(QStringConverter::Utf8);

    ///
    QString outTxt = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    QString appVersion = "2.0.0";
    outTxt += QStringLiteral("<DKey version=\"%1\">\n").arg(appVersion);
    outTxt += termList.getDkXml();
    outTxt += "</DKey>\n";
    ///

    //    QString outTxt = coupletList.getDkXml();
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

void termWindow::import()
{
    QFileInfo fileInfo(parent->getFilePath());
    QString filePath = fileInfo.path();

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Import file"), filePath, tr("Text files (*.txt)"));
    if( fileName.isEmpty() )
        return;

    QFile inFile(fileName);
    if (!inFile.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, QGuiApplication::applicationDisplayName(),
                             tr("Cannot open file %1:\n%2.")
                             .arg(fileName)
                             .arg(inFile.errorString()));
        return;
    }

    QTextStream inStream(&inFile);
    inStream.setCodec("UTF-8");
// qt6   inStream.setEncoding(QStringConverter::Utf8);
    QStringList outTxtList;
    while (!inStream.atEnd())
    {
        QString line = inStream.readLine();
        outTxtList.push_back(line);
    }
    inFile.close();

    if(outTxtList.size() == 0)
        return;
    termList.importTxt(outTxtList);

    if(termList.size() == 0)
        return;

    fillTable();
    setWindowModified(true);
}

void termWindow::exportTxt()
{
    if(termList.size() == 0)
        return;

    QFileInfo fileInfo(parent->getFilePath());
    QString outName = fileInfo.path()+"/"+fileInfo.baseName()+"-exported-terms.txt";
    QString fileName = QFileDialog::getSaveFileName
            (this, tr("Export the term list as"), outName,
             tr("Text (*.txt)")
             );
    if( fileName.isEmpty() )
        return;

    QFile inFile(fileName);
    if (!inFile.open(QIODevice::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, QGuiApplication::applicationDisplayName(),
                             tr("Cannot open file %1:\n%2.")
                             .arg(fileName)
                             .arg(inFile.errorString()));
        return;
    }

    QTextStream outStream(&inFile);
    outStream.setCodec("UTF-8");
// qt6   outStream.setEncoding(QStringConverter::Utf8);

    QString outTxt = termList.exportTxt();
    outStream << outTxt;

    if(outStream.status() == QTextStream::WriteFailed)
        QMessageBox::warning(this, QGuiApplication::applicationDisplayName(),
                             tr("Writing to file %1 failed.")
                             .arg(fileName));
    inFile.close();
}

