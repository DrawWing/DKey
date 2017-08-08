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
    QString getError() const;
    QString getDkTxt() const;
    QString getHtml() const;
    QString getHtmlTable(QString path = QString()) const;
    void findIntro(QStringList & inTxtList);
    void findFigs(QString &path);
    void findFrom();
    QList<int> findFrom(int index) const;
    void push_back(dkCouplet inCouplet);
    void copyAt(int i, dkCouplet inCouplet);
    void insertAt(int i, dkCouplet inCouplet);
    void insertDummyAt(int i);
    void removeAt(int i);
//    void stepDownAdr(int thd);
//    void stepUpAdr(int thd);
    void pointerChain(int currIndex, int currNumber, QList<int> & chainList);
    void findPointerChains();
    void arrangeCouplets(int currNumber, QList<dkCouplet> &newList);
    bool isNumberUnique();
    bool reNumber();
    void updatePointers(int from, int to);

private:
    void appendCouplet(const QStringList & inTxt, int lineNo);
    QList<int> findStartNumbers(const QString & fileName) const;
    QList<int> findStartNumbers(QStringList & inTxtList) const;

    QList< dkCouplet > thisList;
    QString intro;
    int introSize;
    int maxNumber;
    QString error;
};

#endif // DKCOUPLETLIST_H
