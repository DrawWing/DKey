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
#include <QFileInfo>
#include <QApplication>
#include <QMenuBar>
//#include <QStatusBar>
#include <QFileDialog>
#include <QHeaderView>
#include <QInputDialog>
//#include <QElapsedTimer>
//#include <QDebug>

#include "mainwindow.h"
#include "coupletDialog.h"
#include "dkView.h"
#include "termWindow.h"
#include "textEditor.h"
#include "commands.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    appName = QGuiApplication::applicationDisplayName();
    appVersion = QGuiApplication::applicationVersion();

    undoStack = new QUndoStack(this);

    filePath = "";
    createActions();
    createMenus();
    createToolBars();

    readSettings(); //has to be after createActions
    createTable();
    setWindowTitle("[*]");

    htmlWindow = new dkBrowser(this);
    htmlWindow->setFormat(&format);
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
    QString htmlTxt;
    htmlTxt += intro;
    coupletList.findFrom();
    htmlTxt += coupletList.getHtmlImg();

//    dkFormat format;
//    QFileInfo fileInfo(filePath);
//    QString path = fileInfo.absolutePath();
//    format.setFilePath(path);
//    format.setFigures(&figTxt);
//    htmlTxt += format.glossaryHtml(glossary, true);
    htmlTxt += format.glossaryHtml(true);
    //    htmlTxt += glossary.getHtml();

    htmlTxt = glossary.addLinks(htmlTxt);
//    htmlTxt = format.linkGlossary(htmlTxt);

    htmlWindow->setHtml(htmlTxt);
    htmlWindow->setWindowTitle(tr("Hypertext viewer"));
    QApplication::restoreOverrideCursor();
}

void MainWindow::viewEndpointList()
{
    if(coupletList.size() == 0)
        return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    coupletList.findEndpoints();
    QStringList endpointList = coupletList.getEndpointList();

    QString outTxt;
    if(endpointList.size() > 0)
    {
        outTxt += "List of endpoints:<br />";
        for(int i = 0; i < endpointList.size(); ++i)
            outTxt += endpointList[i] + "<br />";
    } else
        outTxt += "No endpoints was found.";

    htmlWindow->setHtml(outTxt);
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
        outTxt += "List of tags:<br />";
        for(int i = 0; i < tagList.size(); ++i)
            outTxt += tagList[i] + "<br />";
    } else
        outTxt += "No tags was found.";

    htmlWindow->setHtml(outTxt);
    htmlWindow->setWindowTitle(tr("Hypertext viewer"));
    QApplication::restoreOverrideCursor();
}

void MainWindow::viewIntro()
{
    dkEditor *introWindow = new dkEditor(&intro, this);
    introWindow->setWindowTitle(tr("Introduction")+"[*]");
    introWindow->show();
}

void MainWindow::viewGlossary()
{
    termWindow * glossaryWindow = new termWindow(&glossary, this);
    glossaryWindow->setWindowTitle(tr("Glossary")+"[*]");
    glossaryWindow->show();
}

void MainWindow::viewEndpoints()
{
    termWindow * endpointsWindow = new termWindow(&endpoints, this);
    endpointsWindow->setWindowTitle(tr("Endpoints")+"[*]");
    endpointsWindow->show();
}

void MainWindow::viewFigTxt()
{
    termWindow * figTxtWindow = new termWindow(&figTxt, this);
    figTxtWindow->setWindowTitle(tr("Figure legends")+"[*]");
    figTxtWindow->show();
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
    intro = coupletList.getIntro();

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
        QMessageBox::warning(this, tr("Warning"), tr("Cannot open %1.").arg(file.fileName()));
        return ;
    }

    QString errorStr;
    int errorLine;
    int errorColumn;
    if (!xmlDoc.setContent(&file, false, &errorStr, &errorLine, &errorColumn)) {
        file.close();
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
        QMessageBox::warning(this, QObject::tr("DOM Parser"),QObject::tr("No <%1> element found in the XML file!").arg(elementName));
        return;
    }
//    QString versionTxt = docElement.attribute("version");
//    double version = versionTxt.toDouble();

    elementName = "key";
//    QDomNode keyNode = xmlDoc.namedItem(elementName);
    QDomElement keyElement = dkeyNode.namedItem(elementName).toElement();
    if ( keyElement.isNull() )
    {
        QMessageBox::warning(this, QObject::tr("DOM Parser"),QObject::tr("No <%1> element found in the XML file!").arg(elementName));
        return;
    }
    ///

    dkCoupletList appendCouplets;
    appendCouplets.fromDkXml(keyElement);
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
        insertTableRow(lastRow);
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
        insertTableRow(i);
}

void MainWindow::insertTableRow(int i)
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
    insertTableRow(i);
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

    ok = coupletList.isCircularityOK();
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

    ok = coupletList.isPointerIncreasing();
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
    insertTableRow(0);
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
    //    qDebug() << timer.elapsed() << "start";

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
    if(!isVersionOK(versionTxt))
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(this, QObject::tr("DOM Parser"),QObject::tr("The file was saved in newer version of DKey.\nPlease download the most recent version."));
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    }


    elementName = "key";
    QDomElement keyElement = dkeyNode.namedItem(elementName).toElement();
    if ( keyElement.isNull() )
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(this, QObject::tr("DOM Parser"),QObject::tr("No <%1> element found in the XML file!").arg(elementName));
        return false;
    }

    coupletList.fromDkXml(keyElement);

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
    format.setFilePath(path);

    fillTable();
    //    qDebug() << timer.elapsed() << "fromDkTxt";

    updateRecentFiles(fileName);
    updateRecentFileActions();
    setWindowTitle(QString("%1[*]").arg(fileInfo.fileName()));

    // load introduction
    QDomElement introElement = dkeyNode.namedItem("introduction").toElement();
    if(!introElement.isNull())
        intro = introElement.text();

    // load glossary
    QDomElement glossaryElement = dkeyNode.namedItem("glossary").toElement();
    glossary.fromDkXml(glossaryElement);
    glossary.setTag("glossary");
    format.setGlossary(&glossary);

    // load endpoints
    QDomElement endpointsElement = dkeyNode.namedItem("endpoints").toElement();
    endpoints.fromDkXml(endpointsElement);
    endpoints.setTag("endpoints");
    format.setEndpoints(&endpoints);

    // load figures
    QDomElement figElement = dkeyNode.namedItem("figures").toElement();
    figTxt.fromDkXml(figElement);
    figTxt.setTag("figures");
    format.setFigures(&figTxt);
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

    ///
    QString outTxt = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    outTxt += QStringLiteral("<DKey version=\"%1\">\n").arg(appVersion);
    outTxt += "<introduction>";
    outTxt += intro.toHtmlEscaped();
    outTxt += "</introduction>\n";
    outTxt += coupletList.getDkXml();
    outTxt += glossary.getDkXml();
    outTxt += endpoints.getDkXml();
    outTxt += figTxt.getDkXml();
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

bool MainWindow::isVersionOK(const QString inVersion)
{
    if(inVersion == "1.0")
        return true;

    QStringList inList = inVersion.split('.');
    if(inList.size() < 3)
        return false;

    QStringList versionList = appVersion.split('.');
    if(inList[0] < versionList[0])
        return true;
    if(inList[1] < versionList[1])
        return true;
    if(inList[2] <= versionList[2])
        return true;

    return false;
}

void MainWindow::exportKey()
{
    if(coupletList.size() == 0)
        return;

    QFileInfo fileInfo(filePath);
    QString outName = fileInfo.path()+"/"+fileInfo.baseName()+".html";
    QString selectedFilter = tr("HTML with images (*.html)");
    QString fileName = QFileDialog::getSaveFileName
            (this, tr("Export the key as"), outName,
             tr("Text (*.txt);;Formated text RTF (*.rtf);;HTML with tabulators (*.html);;HTML table (*.html);;HTML with images (*.html);;Newick tree (*.nwk)"),
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
    coupletList.findFrom();
    if(selectedFilter == "Formated text RTF (*.rtf)")
        htmlTxt = coupletList.getRtf();
    else if(selectedFilter == "Text (*.txt)")
        htmlTxt = coupletList.getTxt();
    else if(selectedFilter == "HTML with tabulators (*.html)")
        htmlTxt = coupletList.getHtmlTabulator();
    else if(selectedFilter == "HTML table (*.html)")
        htmlTxt = coupletList.getHtmlTab();
    else if(selectedFilter == "HTML with images (*.html)")
        htmlTxt = exportHtmlImg(false);
    else if(selectedFilter == "Newick tree (*.nwk)")
        htmlTxt = coupletList.newick();

    QTextStream outStream(&outFile);
    if(selectedFilter != "Formated text RTF (*.rtf)")
        outStream.setCodec("UTF-8");

    outStream << htmlTxt;
    //    statusBar()->showMessage(tr("File saved"), 2000);
}

QString MainWindow::exportHtmlImg(bool withPath)
{
    QElapsedTimer timer;
    timer.start();

    QString htmlTxt = "<head><meta charset=\"UTF-8\"/></head>\n";
    htmlTxt += intro;
//    htmlTxt += coupletList.getHtmlImg(withPath);

//    dkFormat format;
//    QFileInfo fileInfo(filePath);
//    QString path = fileInfo.absolutePath();
//    format.setFilePath(path);
//    format.setFigures(&figTxt);
    htmlTxt += format.keyHtml(coupletList, withPath);
    qDebug() << timer.elapsed() << "key";

//    htmlTxt += format.glossaryHtml(glossary, withPath);
    htmlTxt += format.glossaryHtml(withPath);
    htmlTxt += format.endpointsHtml(withPath);
    htmlTxt += format.figuresHtml(withPath);
    qDebug() << timer.elapsed() << "glossary";

//    htmlTxt += glossary.getHtml();
//    htmlTxt = format.linkGlossary(htmlTxt);
//    htmlTxt = format.linkEndpoints(htmlTxt);
    htmlTxt = format.addLinks(htmlTxt);

    qDebug() << timer.elapsed() << "linking";

    return htmlTxt;
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

    int theRow = selectedRange.bottomRow() + 1;
    //
    QUndoCommand *insertCommand = new InsertCommand(this, theRow);
    undoStack->push(insertCommand);
    //

//    coupletList.insertDummyAt(theRow + 1);
//    insertTableRow(theRow + 1);
//    setWindowModified(true);
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

    //
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, appName,
                                  tr("Do you want to remove the selected couplets?"),
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply != QMessageBox::Yes)
        return;
    //

    QList<int> toRemove;
    for(int i = 0; i < selectedList.size(); ++i)
    {
        QTableWidgetSelectionRange selectedRange = selectedList.at(i);
        int selectedCount = selectedRange.rowCount();
        int topRow = selectedRange.topRow();

        for(int j = 0; j < selectedCount; ++j)
            toRemove.push_back(topRow + j);
    }
    std::sort(toRemove.begin(), toRemove.end());

    //
    QList<dkCouplet> outList;
    for(int i = 0; i < toRemove.size(); ++i)
    {
        dkCouplet theCouplet = coupletList.at(toRemove[i]);
        outList.push_back(theCouplet);
    }
    QUndoCommand *removeCommand = new RemoveCommand(this, toRemove,
                                                    outList);
    undoStack->push(removeCommand);
    //

//    // remove from last to first
//    while(toRemove.size())
//    {
//        int theRow = toRemove.takeLast();
//        table->removeRow(theRow);
//        coupletList.removeAt(theRow);
//    }
//    setWindowModified(true);
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
    std::sort(toRemove.begin(), toRemove.end());

    //
    QList<dkCouplet> outList;
    for(int i = 0; i < toRemove.size(); ++i)
    {
        dkCouplet theCouplet = coupletList.at(toRemove[i]);
        outList.push_back(theCouplet);
    }
    QUndoCommand *cutCommand = new CutCommand(this, toRemove,
                                                    outList);
    undoStack->push(cutCommand);
    //

//    // remove from last to first
//    while(toRemove.size())
//    {
//        int theRow = toRemove.takeLast();
//        table->removeRow(theRow);
//        coupletList.removeAt(theRow);
//    }
//    setWindowModified(true);
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

    //
    QList<int> toRemove;
    for(int i = 0; i < coupletClipboard.size(); ++i)
    {
        toRemove.push_back(bottomRow+1+i);
    }

    QList<dkCouplet> outList;
    for(int i = 0; i < coupletClipboard.size(); ++i)
    {
        dkCouplet theCouplet = coupletClipboard.at(i);
        outList.push_back(theCouplet);
    }
    QUndoCommand *pasteCommand = new PasteCommand(this, toRemove,
                                                    outList);
    undoStack->push(pasteCommand);
    //

//    for(int i = 0; i < coupletClipboard.size(); ++i)
//    {
//        coupletList.insertAt(bottomRow+1+i, coupletClipboard.at(i));
//        insertTableRow(bottomRow+1+i);
//    }

    if(!isCopy)
        coupletClipboard.clear(); // couplets can be pasted only in one place

//    setWindowModified(true);
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

// col is only for compatibility with cellDoubleClicked(row, col)
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
    if (dialog.exec())
    {
        //
        dkCouplet oldCouplet = coupletList.at(row);
        QUndoCommand *editCommand = new EditCommand(this, theCouplet,
                                                    oldCouplet, row);
        undoStack->push(editCommand);
        //

//        coupletList.setCouplet(theCouplet, row);
//        updateTableRow(row);
//        table->selectRow(row);
//        setWindowModified(true); // use undo clean state
    }
    --col; // to prevent error messages
}

// public undo command
void MainWindow::updateCouplet(dkCouplet & theCouplet, int row)
{
    table->clearSelection();

    coupletList.setCouplet(theCouplet, row);
    updateTableRow(row);
    table->selectRow(row);
    setWindowModified(true); // change to undo clean state
}

// public undo command
void MainWindow::removeCouplets(QList<int> &inList)
{
    table->clearSelection();

    // remove from last to first
    for(int i = inList.size() - 1; i >= 0; --i)
    {
        coupletList.removeAt(inList[i]);
        table->removeRow(inList[i]);
    }
    setWindowModified(true);
}

// public undo command
void MainWindow::insertCouplets(QList<int> &inRows, QList<dkCouplet> &inCouplets)
{
    table->clearSelection();

    // insert from first to last
    for(int i = 0; i < inRows.size(); ++i)
    {
        int theRow = inRows[i];
        dkCouplet theCouplet =  inCouplets[i];
        coupletList.insertAt(theRow, theCouplet);
        insertTableRow(theRow);
        table->selectRow(theRow);
    }
    setWindowModified(true);
}

// public undo command
void MainWindow::insertEmptyCouplet(int inRow)
{
    table->clearSelection();

    coupletList.insertDummyAt(inRow);
    insertTableRow(inRow);
    table->selectRow(inRow);
    setWindowModified(true);
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

    undoAction = undoStack->createUndoAction(this, tr("&Undo"));
    undoAction->setShortcuts(QKeySequence::Undo);

    redoAction = undoStack->createRedoAction(this, tr("&Redo"));
    redoAction->setShortcuts(QKeySequence::Redo);

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

    viewEndpointListAct = new QAction(tr("Endpoint &list"), this);
    connect(viewEndpointListAct, SIGNAL(triggered()), this, SLOT(viewEndpointList()));

    viewTagsAct = new QAction(tr("&Tags"), this);
    connect(viewTagsAct, SIGNAL(triggered()), this, SLOT(viewTags()));

    viewIntroAct = new QAction(tr("&Introduction"), this);
    connect(viewIntroAct, SIGNAL(triggered()), this, SLOT(viewIntro()));

    viewGlossaryAct = new QAction(tr("&Glossary"), this);
    connect(viewGlossaryAct, SIGNAL(triggered()), this, SLOT(viewGlossary()));

    viewEndpointsAct = new QAction(tr("&Endpoints"), this);
    connect(viewEndpointsAct, SIGNAL(triggered()), this, SLOT(viewEndpoints()));

    viewFigTxtAct = new QAction(tr("&Figure Legends"), this);
    connect(viewFigTxtAct, SIGNAL(triggered()), this, SLOT(viewFigTxt()));

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

    separatorAction = fileMenu->addSeparator();
    for (int i = 0; i < MaxRecentFiles; ++i)
        fileMenu->addAction(recentFileActions[i]);
    fileMenu->addSeparator();

    fileMenu->addAction(exitAct);

    menuBar()->addMenu(fileMenu);

    editMenu = new QMenu(tr("&Edit"), this);
    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);
    editMenu->addSeparator();

    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);
    editMenu->addAction(editRowAct);
    editMenu->addAction(insertRowAct);
    editMenu->addAction(removeRowAct);
    editMenu->addAction(swapLeadsAct);
    editMenu->addAction(renumberAct);
//    editMenu->addAction(arrangeAct);
    menuBar()->addMenu(editMenu);

    debugMenu = new QMenu(tr("&Debug"), this);
    debugMenu->addAction(findErrorsAct);
    debugMenu->addAction(findImgErrorsAct);
    menuBar()->addMenu(debugMenu);

    viewMenu = new QMenu(tr("&View"), this);
    viewMenu->addAction(viewBrowserAct);
    viewMenu->addAction(viewHtmlAct);
    viewMenu->addAction(viewEndpointListAct);
    viewMenu->addAction(viewTagsAct);
    viewMenu->addSeparator();
    viewMenu->addAction(viewIntroAct);
    viewMenu->addAction(viewGlossaryAct);
    viewMenu->addAction(viewEndpointsAct);
    viewMenu->addAction(viewFigTxtAct);
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
                         "<p>If you find this software useful please cite it:<br />Tofilski A (2018) DKey software for editing and browsing dichotomous keys. ZooKeys 735: 131-140. <a href=\"https://doi.org/10.3897/zookeys.735.21412\">https://doi.org/10.3897/zookeys.735.21412</a></p>"
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

    intro.clear();
    coupletList.clear();
    glossary.clear();
    figTxt.clear();
    htmlWindow->hide();
}

MainWindow::~MainWindow()
{
}

dkFormat * MainWindow::getFormat()
{
    return &format;
}

dkTermList * MainWindow::getGlossary()
{
    return &glossary;
}

dkTermList * MainWindow::getFigTxt()
{
    return &figTxt;
}

dkBrowser * MainWindow::getHtmlWindow()
{
    return htmlWindow;
}

QString MainWindow::getFilePath() const
{
    return filePath;
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

