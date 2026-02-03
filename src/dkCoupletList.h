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
#include <QDomDocument>

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
    void parseIndentedKey(QStringList & inTxtList);
    // void fromDkTxt(const QString & fileName);
    void fromDkXml(const QDomElement &inElement, int ver = 1);

    QList< dkCouplet > getList() const;
    void findMaxNumber();
    int getMaxNumber() const;
    QString getFilePath() const;
    QString getError() const;
    QString getIntro() const;
    QString getDkTxt() const;
    QString getDkXml() const;
    QString getRtf() const;
    QString getTxt() const;
    QString getHtml() const;
    QString getHtmlTabulator() const;
    QString getHtmlTab() const;
    QString getHtmlImg(bool withPath = true);
    QStringList getFigList() const;
    QStringList getEndpointList() const;
    QStringList getTagList() const;
    void findIntro(QStringList & inTxtList);
    QStringList findFigs();
    void findFrom();
    QList<int> findFrom(int number) const;
    QStringList findEndpoints();
    QStringList findTags();
    void findRemaining(int inNumber, QList<bool> & outList) const;
    QList<bool> getRemaining(int inNumber) const;
    void push_back(dkCouplet inCouplet);
    void insertAt(int i, dkCouplet inCouplet);
    void insertDummyAt(int i);
    void removeAt(int i);
    void pointerChain(int currNumber, QList<int> & chainList, QList<QString> &path);
    QString newick();
    void findPointerChains();
    QVector<int> findTreeLength();
    void arrange();
    void arrangeCouplets(int currNumber, QList<dkCouplet> &newList, QVector<int> &treeLgh);
    bool isNumberingOK();
    bool isContentOK();
    bool isFromSingle();
    bool isCircularityOK();
    bool isEndpointOK();
    bool isPointerOK();
    bool isNumberInChain(int index, int inNumber);
    bool isPointerIncreasing();
    bool isPointerChainOK();
    bool isNumberUnique();
    bool reNumber(int startNumber = 1);
    void updatePointers(int from, int to);
    int getLastNumber(const QString &inString);

private:
    QList<int> findStartNumbers(const QString & fileName) const;
    QList<int> findStartNumbers(QStringList & inTxtList) const;
    bool isKeyIndented(QStringList &inTxtList);
    QString newick(int currNumber);

    QList< dkCouplet > thisList;
    QStringList figList; // for error checking
    QStringList endpointList;
    QStringList tagList;
    QString intro;
    int introSize;
    int maxNumber;
    QString filePath;
    QString error;
};

#endif // DKCOUPLETLIST_H
