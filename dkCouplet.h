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

#ifndef DKCOUPLET_H
#define DKCOUPLET_H

#include <QString>
#include <QStringList>
#include "dkString.h"

class dkCouplet
{
public:
    dkCouplet();
    dkCouplet(const QStringList &inTxt);
    void clear();
    bool isEmpty() const;
    void fromDkTxt(const QStringList &inList);
    void readDkTxtLine(const QString &inTxt, bool first);
    void import1number(const QStringList &inTxt);
    void importTxt2(QStringList &inTxt);
    void importTxtLine2(QString &inTxt, bool first);
    int getNumber() const;
    QList<int> getPointerChain() const;
    QString getLead1() const;
    QString getLead2() const;
    QString getTxt() const;
    QString getLead1txt() const;
    QString getLead2txt() const;
    int getPointer1() const;
    int getPointer2() const;
    QString getEndpoint1() const;
    QString getEndpoint2() const;
    QString getError() const;
    int size() const;
    void setFrom(int inVal);
    void setNumber(int inVal);
    void setPointerChain(QList<int> & inList);
    void setLead1(QString inTxt);
    void setLead2(QString inTxt);
    void setPointer1(int inVal);
    void setPointer2(int inVal);
    void setEndpoint1(QString inTxt);
    void setEndpoint2(QString inTxt);
    void setFigList1(QStringList & inList);
    void setFigList2(QStringList & inList);
    QString getDkTxt() const;
    QString getHtml() const;
    QString getHtmlTable(QString path = QString()) const;
    QString findErrors() const;
    void findFigs(QString & path);
    void stepDownAdr(int thd);
    void stepUpAdr(int thd);
    void swapLeads();

private:
    QStringList findFigs(QString &inTxt, QString & path) const;
//    QString findEndPart(QString & inString);
//    QString findStartPart(QString & inString);
//    QString frontDigits(const QString & inTxt) const;
    QString removeAB(QString & inTxt, bool first) const;

    int number;
    int from; //number of couplet refereint to this
    QList<int>  pointerChain;
    dkString lead1;
    int pointer1;
    dkString endpoint1;
    QStringList figList1;
    dkString lead2;
    int pointer2;
    dkString endpoint2;
    QStringList figList2;

    QString error;
};

#endif // DKCOUPLET_H
