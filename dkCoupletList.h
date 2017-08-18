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

#ifndef DKCOUPLETLIST_H
#define DKCOUPLETLIST_H

#include <QList>
#include <QString>

#include "dkCouplet.h"

class dkCoupletList
{
public:
    dkCoupletList();
    dkCouplet at(int i) const;
    dkCouplet getCoupletWithNumber(int number) const;
    int getIndexWithNumber(int number) const;
    void setCouplet(const dkCouplet & inCouplet, int i);
    void setFilePath(const QString & inTxt);
    int size() const;
    void clear();
    void fromTxt(QStringList & inTxtList);
    void importTxt(QStringList & inTxtList);
    void parse1numberKey(QStringList & inTxtList);
    void parse2numberKey(QStringList & inTxtList);
    void fromDkTxt(const QString & fileName);
    QList< dkCouplet > getList() const;
    void findMaxNumber();
    int getMaxNumber() const;
    QString getFilePath() const;
    QString getError() const;
    QString getDkTxt() const;
    QString getHtml() const;
    QString getHtmlTable() const;
    QStringList getEndpointList() const;
    void findIntro(QStringList & inTxtList);
    void findFigs();
//    void findFigs(QString &path);
    void findFrom();
    QList<int> findFrom(int number) const;
    QStringList findEndpoints();
    void findRemaining(int inNumber, QList<bool> & outList) const;
    QList<bool> getRemaining(int inNumber) const;
    void push_back(dkCouplet inCouplet);
    void copyAt(int i, dkCouplet inCouplet);
    void insertAt(int i, dkCouplet inCouplet);
    void insertDummyAt(int i);
    void removeAt(int i);
    void pointerChain(int currNumber, QList<int> & chainList, QList<QString> &path);
    void findPointerChains();
    void arrangeCouplets(int currNumber, QList<dkCouplet> &newList);
    bool isNumberingOK();
    bool isContentOK();
    bool isFromOK();
    bool isEndpointOK();
    bool isPointerOK();
    bool isPointerChainOK();
    bool isNumberUnique();
    bool reNumber();
    void updatePointers(int from, int to);

private:
//    void appendCouplet(const QStringList & inTxt);
    QList<int> findStartNumbers(const QString & fileName) const;
    QList<int> findStartNumbers(QStringList & inTxtList) const;
    bool isKeyIndented(QStringList &inTxtList);

    QList< dkCouplet > thisList;
    QStringList endpointList;
    QString intro;
    int introSize;
    int maxNumber;
    QString filePath;
    QString error;
};

#endif // DKCOUPLETLIST_H
