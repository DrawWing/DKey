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

#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QSplitter>
#include <QLabel>

#include "dkView.h"

dkView::dkView(dkCoupletList * inList, QWidget *parent) :
    QMainWindow(parent)
{
    lead1Browser = new QTextBrowser(this);
    lead1Browser->setOpenLinks(false);
    lead2Browser = new QTextBrowser(this);
    lead2Browser->setOpenLinks(false);
    QHBoxLayout *browserLayout = new QHBoxLayout;
    browserLayout->addWidget(lead1Browser);
    browserLayout->addWidget(lead2Browser);
    QWidget *browserContainer = new QWidget;
    browserContainer->setLayout(browserLayout);

    QSplitter * tabSpliter = new QSplitter;

    pathTab = new QTableWidget;
    pathTab->setWordWrap(true);
    pathTab->setColumnCount(1);
    pathTab->setEditTriggers(QAbstractItemView::NoEditTriggers);
    pathTab->horizontalHeader()->hide();
    pathTab->horizontalHeader()->setStretchLastSection(true);
    pathTab->verticalHeader()->hide();
    pathTab->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    QVBoxLayout *pathLayout = new QVBoxLayout;
    QLabel *pathLabel = new QLabel("Path");
    pathLayout->addWidget(pathLabel);
    pathLayout->addWidget(pathTab);
    QWidget *pathContainer = new QWidget;
    pathContainer->setLayout(pathLayout);
    tabSpliter->addWidget(pathContainer);

    remainingTab = new QTableWidget;
    remainingTab->setWordWrap(true);
    remainingTab->setColumnCount(1);
    remainingTab->setEditTriggers(QAbstractItemView::NoEditTriggers);
    remainingTab->horizontalHeader()->hide();
    remainingTab->horizontalHeader()->setStretchLastSection(true);
    remainingTab->verticalHeader()->hide();
    remainingTab->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    QVBoxLayout *remainingLayout = new QVBoxLayout;
    QLabel *remainingLabel = new QLabel("Remaining endpoints");
    remainingLayout->addWidget(remainingLabel);
    remainingLayout->addWidget(remainingTab);
    QWidget *remainingContainer = new QWidget;
    remainingContainer->setLayout(remainingLayout);
    tabSpliter->addWidget(remainingContainer);

    excludedTab = new QTableWidget;
    excludedTab->setWordWrap(true);
    excludedTab->setColumnCount(1);
    excludedTab->setEditTriggers(QAbstractItemView::NoEditTriggers);
    excludedTab->horizontalHeader()->hide();
    excludedTab->horizontalHeader()->setStretchLastSection(true);
    excludedTab->verticalHeader()->hide();
    excludedTab->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    QVBoxLayout *excludedLayout = new QVBoxLayout;
    QLabel *excludedLabel = new QLabel("Excluded endpoints");
    excludedLayout->addWidget(excludedLabel);
    excludedLayout->addWidget(excludedTab);
    QWidget *excludedContainer = new QWidget;
    excludedContainer->setLayout(excludedLayout);
    tabSpliter->addWidget(excludedContainer);

    QVBoxLayout *tabLayout = new QVBoxLayout;
    tabLayout->addWidget(tabSpliter);
    QWidget *tabContainer = new QWidget;
    tabContainer->setLayout(tabLayout);

    QSplitter * mainSpliter = new QSplitter;
    mainSpliter->setOrientation(Qt::Vertical);
    mainSpliter->addWidget(browserContainer);
    mainSpliter->addWidget(tabContainer);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(mainSpliter);

    QWidget *mainContainer = new QWidget(this);
    mainContainer->setLayout(mainLayout);
    setCentralWidget(mainContainer);

    //    setLayout(mainLayout);
    setWindowTitle("Key browser");

    connect(lead1Browser, SIGNAL( anchorClicked( QUrl ) ),
            this, SLOT( clickedCouplet(QUrl ) ));
    connect(lead2Browser, SIGNAL( anchorClicked( QUrl ) ),
            this, SLOT( clickedCouplet(QUrl ) ));
    connect(pathTab, SIGNAL( cellClicked(int, int) ),
            this, SLOT( clickedPath(int, int) ));
    connect(remainingTab, SIGNAL( cellClicked(int, int) ),
            this, SLOT( clickedRemaining(int, int) ));
    connect(excludedTab, SIGNAL( cellClicked(int, int) ),
            this, SLOT( clickedExcluded(int, int) ));

    coupletList = inList;
    endpointList = coupletList->getEndpointList();
    filePath = coupletList->getFilePath();

    goToNumber(1);
}

void dkView::goToNumber(int inNumber)
{
    number = inNumber;
    currCouplet = coupletList->at(number-1);

    QString tmp = currCouplet.getLead1html(filePath);
    lead1Browser->setHtml(currCouplet.getLead1html(filePath));
    lead2Browser->setHtml(currCouplet.getLead2html(filePath));

    // path
    QList<QString>  path = currCouplet.getLeadChain();
    pathTab->setRowCount(path.size());
    for(int i = 0; i < path.size(); ++i)
    {
        pathTab->setItem(i, 0, new QTableWidgetItem(path[i]));
    }

    // remaining
    QList<bool> remainingList = coupletList->getRemaining(number);
    int count = 0;
    for(int i = 0; i < remainingList.size(); ++i)
    {
        if(remainingList[i])
            ++count;
    }

    remainingTab->setRowCount(count);
    excludedTab->setRowCount(endpointList.size() - count);

    int tabCount = 0;
    int excludedCount = 0;
    for(int i = 0; i < endpointList.size(); ++i)
    {
        if(remainingList[i])
            remainingTab->setItem(tabCount++, 0, new QTableWidgetItem(endpointList[i]));
        else
            excludedTab->setItem(excludedCount++, 0, new QTableWidgetItem(endpointList[i]));
    }
}

void dkView::goToEndpoint(bool first)
{
    QString lastPath;
    if(first)
    {
        QString endpoint = currCouplet.getEndpoint1();
        lead1Browser->setText(endpoint);
        lead2Browser->setText("");
        lastPath = currCouplet.getLead1();
    }
    else
    {
        QString endpoint = currCouplet.getEndpoint2();
        lead2Browser->setText(endpoint);
        lead1Browser->setText("");
        lastPath = currCouplet.getLead2();
    }

    int newRow = pathTab->rowCount();
    pathTab->insertRow(newRow);
    pathTab->setItem(newRow, 0, new QTableWidgetItem(lastPath));
}

void dkView::clickedCouplet(QUrl inUrl)
{
    QString path =  inUrl.path();
    if(path == "lead1")
    {
        goToNumber(currCouplet.getPointer1());
    }
    else if(path == "lead2")
    {
        goToNumber(currCouplet.getPointer2());
    }
}

void dkView::clickedPath(int row, int col)
{
    QList<int>  pointerChain = currCouplet.getPointerChain();
    goToNumber(pointerChain[row]);
    --col; // to prevent error messages
}

void dkView::clickedRemaining(int row, int col)
{
    QString theItemString = remainingTab->item(row, col)->text();
    goToItemTxt(theItemString);
}

void dkView::clickedExcluded(int row, int col)
{
    QString theItemString = excludedTab->item(row, col)->text();
    goToItemTxt(theItemString);
}

void dkView::goToItemTxt(const QString &theItemString)
{
    QStringList theList = theItemString.split("-");
    QString theString = theList.last().simplified();
    int theNumber = theString.toInt();
    if(theNumber < 1 || theNumber > coupletList->getMaxNumber())
        return;
    int theIndex = coupletList->getIndexWithNumber(theNumber);
    goToNumber(theIndex+1);
}
