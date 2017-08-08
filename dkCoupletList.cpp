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

#include <QFile>
#include <QTextStream>

#include "dkCoupletList.h"
#include "dkString.h"

dkCoupletList::dkCoupletList()
{
}

dkCouplet dkCoupletList::at(int i) const
{
    return thisList.at(i);
}

dkCouplet dkCoupletList::getCoupletWithNumber(int number) const
{
    for(int i = 0; i < thisList.size(); ++i)
    {
        dkCouplet theCouplet = thisList[i];
        int theNumber = theCouplet.getNumber();
        if(theNumber == number)
            return theCouplet;
    }
    dkCouplet emptyCouplet;
    return emptyCouplet;
}

int dkCoupletList::getIndexWithNumber(int number) const
{
    for(int i = 0; i < thisList.size(); ++i)
    {
        dkCouplet theCouplet = thisList[i];
        int theNumber = theCouplet.getNumber();
        if(theNumber == number)
            return i;
    }
    return -1;
}

//int dkCoupletList::getMaxNumber() const
//{
//    int max = 0;
//    for(int i = 0; i < thisList.size(); ++i)
//    {
//        dkCouplet theCouplet = thisList[i];
//        int theNumber = theCouplet.getNumber();
//        if(theNumber > max)
//            max = theNumber;
//    }
//    return max;
//}

void dkCoupletList::setCouplet(const dkCouplet &inCouplet, int i)
{
    if(i > thisList.size())
        return;
    thisList[i] = inCouplet;
}

int dkCoupletList::size() const
{
    return thisList.size();
}

void dkCoupletList::clear()
{
    thisList.clear();
    intro.clear();
    introSize = 0;
    maxNumber = -1;
    error.clear();
}

void dkCoupletList::fromTxt(QStringList &inTxtList)
{
    clear();

    // extract intro
    int lineNo;
    QStringList coupletTxt;
    for(lineNo = 0; lineNo < inTxtList.size(); ++lineNo)
    {
        dkString line = inTxtList[lineNo];
//        if(startsWithNumber(line))
        if(line.startsWithDigit())
        {
            coupletTxt.push_back(line);
            ++lineNo;
            break;
        }
        else
            intro.append(line);
    }

    // extract key
    for(; lineNo < inTxtList.size(); ++lineNo)
    {
        dkString line = inTxtList[lineNo];

        if(line.startsWithDigit())
        {
            QString digits = line.frontDigits();

            appendCouplet(coupletTxt, lineNo);
            coupletTxt.clear();
            coupletTxt.push_back(line);
        }
        else
            coupletTxt.append(line);
    }
    appendCouplet(coupletTxt, lineNo);

}

void dkCoupletList::importTxt(QStringList & inTxtList)
{
    clear();

    findIntro(inTxtList);

    QList<int> numbers = findStartNumbers(inTxtList);
    if(numbers[0] == numbers[1])
        parse2numberKey(inTxtList);
    else
        parse1numberKey(inTxtList);
}

void dkCoupletList::parse1numberKey(QStringList & inTxtList)
{
    QStringList coupletTxt;

    dkString line = inTxtList[introSize];
    if(line.startsWithDigit())
    {
        coupletTxt.push_back(line);
    }
    else
        return; // key needst to start with number

    int firstLine = introSize + 1;
    for(int lineNo = introSize + 1; lineNo < inTxtList.size(); ++lineNo)
    {
        dkString line = inTxtList[lineNo];

        if(line.startsWithDigit())
        {
            QString digits = line.frontDigits();

            appendCouplet(coupletTxt, firstLine);
            coupletTxt.clear();
            coupletTxt.push_back(line);
            firstLine = lineNo;
        }
        else
            coupletTxt.append(line);
    }
    appendCouplet(coupletTxt, firstLine);
}

void dkCoupletList::parse2numberKey(QStringList &inTxtList)
{
    QStringList coupletTxt;
    int prevNumber;

    dkString line = inTxtList[introSize];
    if(line.startsWithDigit())
    {
        QString digits = line.frontDigits();
        prevNumber = digits.toInt();
        coupletTxt.push_back(line);
    }
    else
        return; // key needst to start with number

    int firstLine = introSize;
    for(int lineNo = introSize + 1; lineNo < inTxtList.size(); ++lineNo)
    {
        line = inTxtList[lineNo];

        if(line.startsWithDigit())
        {
            QString digits = line.frontDigits();
            int currNumber = digits.toInt();
            if(currNumber == prevNumber)
                coupletTxt.append(line);
            else
            {
                //    appendCouplet(coupletTxt, firstLine);
                    dkCouplet newCouplet;
                    newCouplet.importTxt2(coupletTxt);
                    thisList.push_back(newCouplet);
                coupletTxt.clear();
                coupletTxt.push_back(line);
                prevNumber = currNumber;
                firstLine = lineNo;
            }
        }
        else
            coupletTxt.push_back(line);
    }
//    appendCouplet(coupletTxt, firstLine);
    dkCouplet newCouplet;
    newCouplet.importTxt2(coupletTxt);
    thisList.push_back(newCouplet);
}

void dkCoupletList::fromDkTxt(const QString & fileName)
{
    clear();

    QFile inFile(fileName);
    if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream inStream(&inFile);

    // read intro and first couplet
    while (!inStream.atEnd()) {

        dkString line = inStream.readLine();

        if(line.startsWithDigit())
//        if(startsWithNumber(line))
        {
            // read first couplet
            QStringList coupletTxt;
            coupletTxt.push_back(line);
            line = inStream.readLine();
            coupletTxt.push_back(line);
            dkCouplet newCouplet;
            newCouplet.fromDkTxt(coupletTxt);
            QString coupletError = newCouplet.getError();
            if(coupletError.isEmpty())
                thisList.push_back(newCouplet);
            else
            {
                error = coupletError;
                return;
            }

            break;
        }
        else
            intro.append(line);
    }

    // read the rest
    while (!inStream.atEnd()) {
        QStringList coupletTxt;
        QString line = inStream.readLine();
        coupletTxt.push_back(line);
        line = inStream.readLine();
        coupletTxt.push_back(line);
        dkCouplet newCouplet;
        newCouplet.fromDkTxt(coupletTxt);
        QString coupletError = newCouplet.getError();
        if(coupletError.isEmpty())
            thisList.push_back(newCouplet);
        else
        {
            error = coupletError;
            return;
        }
    }
}

void dkCoupletList::appendCouplet(const QStringList & inTxt, int lineNo)
{
    dkCouplet newCouplet(inTxt);
    thisList.push_back(newCouplet);

    QString coupletError = newCouplet.findErrors();
    if(coupletError.isEmpty())
        return;
    int coupletNumber = newCouplet.getNumber();
    QString errorPrefix;
    if(coupletNumber == -1)
        errorPrefix = QString("Line %1").arg(lineNo);
    else
        errorPrefix = QString("Line %1, couplet %2\n").arg(lineNo).arg(coupletNumber);
    error.append(errorPrefix);
    error.append(coupletError);

}

QList< dkCouplet > dkCoupletList::getList() const
{
    return thisList;
}

void dkCoupletList::findMaxNumber()
{
    if(thisList.size() == 0)
    {
        maxNumber = -1;
        return;
    }

    maxNumber = thisList[0].getNumber();

    for(int i = 1; i < thisList.size(); ++i)
    {
        if(thisList[i].getNumber() > maxNumber)
            maxNumber = thisList[i].getNumber();
    }
}

int dkCoupletList::getMaxNumber() const
{
    return maxNumber;
}

QString dkCoupletList::getError() const
{
//    QString error;
//    for(int i = 0; i < thisList.size(); ++i)
//    {
//        QString coupletError = thisList[i].findErrors();
//        if(coupletError.isEmpty())
//            continue;
//        int coupletNumber = newCouplet.getNumber();
//        QString errorPrefix;
//        if(coupletNumber == -1)
//            errorPrefix = QString("Line %1").arg(lineNo);
//        else
//            errorPrefix = QString("Line %1, couplet %2\n").arg(lineNo).arg(coupletNumber);
//        error.append(errorPrefix);
//        error.append(coupletError);
//    }
    return error;
}

QString dkCoupletList::getDkTxt() const
{
    QString outTxt;
    for(int i = 0; i < thisList.size(); ++i)
    {
        dkCouplet theCouplet = thisList[i];
        QString theTxt = theCouplet.getDkTxt();
        outTxt.append(theTxt);
    }
    return outTxt;
}

QString dkCoupletList::getHtml() const
{
    QString htmlTxt;
    for(int i = 0; i < thisList.size(); ++i)
    {
        dkCouplet theCouplet = thisList[i];
        QString theHtml = theCouplet.getHtml();
        htmlTxt.append(theHtml);
    }
    return htmlTxt;
}

QString dkCoupletList::getHtmlTable(QString path) const
{
    QString htmlTxt;
    for(int i = 0; i < thisList.size(); ++i)
    {
        dkCouplet theCouplet = thisList[i];
        QString theHtml = theCouplet.getHtmlTable(path);
        htmlTxt.append(theHtml);
    }

    return htmlTxt;
}

void dkCoupletList::findIntro(QStringList & inTxtList)
{
    introSize = 0;
    for(int lineNo = 0; lineNo < inTxtList.size(); ++lineNo)
    {
        dkString line = inTxtList[lineNo];
        if(line.startsWithDigit())
        {
            return;
        }
        else
            ++introSize;
    }
    return;
}

void dkCoupletList::findFigs(QString & path)
{
    for(int i = 0; i < thisList.size(); ++i)
    {
        thisList[i].findFigs(path);
    }
}

QList<int> dkCoupletList::findStartNumbers(const QString &fileName) const
{
    QList<int> outList;

    QFile inFile(fileName);
    if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return outList;

    QTextStream inStream(&inFile);

    // read intro and first couplet
    while (!inStream.atEnd()) {
        dkString line = inStream.readLine();

        if(line.startsWithDigit())
        {
            bool ok;
            dkString digits = line.frontDigits();
            int number = digits.toInt(&ok);
            if(ok)
                outList.push_back(number);
        }
    }
    return outList;
}

QList<int> dkCoupletList::findStartNumbers(QStringList & inTxtList) const
{
    QList<int> outList;
    for(int lineNo = 0; lineNo < inTxtList.size(); ++lineNo)
    {
        dkString line = inTxtList[lineNo];

        if(line.startsWithDigit())
        {
            bool ok;
            dkString digits = line.frontDigits();
            int number = digits.toInt(&ok);
            if(ok)
                outList.push_back(number);
        }
    }
    return outList;
}

void dkCoupletList::findFrom()
{
//    coupletList[0].setFrom(-1); // allready set in constructor
    for(int i = 1; i < thisList.size(); ++i)
    {
        QList<int> theList = findFrom(i);
        if(theList.size() > 0)
            thisList[i].setFrom(theList[0]);
    }
}

// return a list of pointers to couplets refering to couplet index
QList<int>  dkCoupletList::findFrom(int index) const
{
    QList<int> outList;
    for(int j = 0; j < thisList.size(); ++j)
    {
        int thePointer = thisList[j].getPointer1();
        if(thePointer == index)
            outList.push_back(j);
        thePointer = thisList[j].getPointer2();
        if(thePointer == index)
            outList.push_back(j);
    }
    return outList;
}

void dkCoupletList::push_back(dkCouplet inCouplet)
{
    thisList.push_back(inCouplet);
}

// couplet number is updated
// good for copy and append: copy-paste
void dkCoupletList::copyAt(int i, dkCouplet inCouplet)
{
    findMaxNumber();
    // !!! maxNumber should be uptodate

    ++maxNumber;
    inCouplet.setNumber(maxNumber);
    thisList.insert(i, inCouplet);
}

// couplet number is not modified
// good for move: cut-paste
void dkCoupletList::insertAt(int i, dkCouplet inCouplet)
{
    thisList.insert(i, inCouplet);
}

void dkCoupletList::insertDummyAt(int i)
{
    findMaxNumber();
    // !!! maxNumber should be uptodate

    ++maxNumber;
    dkCouplet dummyCouplet(maxNumber);
    thisList.insert(i, dummyCouplet);
}

void dkCoupletList::removeAt(int i)
{
    thisList.removeAt(i);
}

//// decreases addresses
//void dkCoupletList::stepDownAdr(int thd)
//{
//    for(int i = 0; i < thisList.size(); ++i)
//        thisList[i].stepDownAdr(thd);
//}

//// increase addresses
//void dkCoupletList::stepUpAdr(int thd)
//{
//    for(int i = 0; i < thisList.size(); ++i)
//        thisList[i].stepUpAdr(thd);
//}

//QString dkCoupletList::frontDigits(const QString & inTxt) const
//{
//    QString outTxt;
//    for(int i = 0; i < inTxt.size(); ++i){
//        QChar c = inTxt.at(i);
//        if(c.isDigit())
//            outTxt.push_back(c);
//        else
//            return outTxt;
//    }
//    return outTxt;
//}

void dkCoupletList::pointerChain(int currIndex, int currNumber, QList<int> &chainList)
{
    // couplet numbers need to be in increasing order
    for(int i = 0; i < currIndex; ++i)
    {
        dkCouplet prevCouplet = thisList[i];
        int prevNumber = prevCouplet.getNumber();
        int prevPointer1 = prevCouplet.getPointer1();
        int prevPointer2 = prevCouplet.getPointer2();
        if(currNumber == prevPointer1 || currNumber == prevPointer2)
        {
            chainList.push_back(prevNumber);
            pointerChain(i, prevNumber, chainList);
            break;
        }
    }
}

void dkCoupletList::findPointerChains()
{
    for(int i = 0; i < thisList.size(); ++i)
    {
        dkCouplet theCouplet = thisList.at(i);
        int theNumber = theCouplet.getNumber();
        QList<int> chainList;
        pointerChain(i, theNumber, chainList);
        thisList[i].setPointerChain(chainList);
    }
}

void dkCoupletList::arrangeCouplets(int currNumber, QList< dkCouplet > & newList)
{
    dkCouplet theCouplet = getCoupletWithNumber(currNumber);
    if(theCouplet.isEmpty())
        return; // couplet with the number was not found
//    QString theEndpoint1 = theCouplet.getEndpoint1();
//    QString theEndpoint2 = theCouplet.getEndpoint2();
    int thePointer1 = theCouplet.getPointer1();
    int thePointer2 = theCouplet.getPointer2();

    if(thePointer1 > -1 && thePointer2 > -1)
    {
        dkCouplet couplet1 = getCoupletWithNumber(thePointer1);
        dkCouplet couplet2 = getCoupletWithNumber(thePointer2);
        QList<int> chainList1 = couplet1.getPointerChain();
        QList<int> chainList2 = couplet2.getPointerChain();
        if(chainList1.size() > chainList2.size())
            theCouplet.swapLeads(); // move endpoint to first lead
    }
    else if(thePointer1 > -1)
    {
        theCouplet.swapLeads(); // move endpoint to first lead
    }
    newList.push_back(theCouplet);
    thePointer1 = theCouplet.getPointer1(); // in case of swapping
    if(thePointer1 > -1)
        arrangeCouplets(thePointer1, newList);
    thePointer2 = theCouplet.getPointer2(); // in case of swapping
    if(thePointer2 > -1)
        arrangeCouplets(thePointer2, newList);
}

bool dkCoupletList::isNumberUnique()
{
    for(int i = 0; i < thisList.size(); ++i)
    {
        dkCouplet currCouplet = thisList.at(i);
        int currNumber = currCouplet.getNumber();
        for(int j = i+1; j < thisList.size(); ++j)
        {
            dkCouplet nextCouplet = thisList.at(j);
            int nextNumber = nextCouplet.getNumber();
            if(currNumber == nextNumber)
            {
                error = QString("The same couplet number: %1 in rows %2 and %3.")
                        .arg(currNumber).arg(i).arg(j);
                return false;
            }
        }
    }
    return true;
}

bool dkCoupletList::reNumber()
{
    if(!isNumberUnique())
        return false;

    findMaxNumber();
    for(int i = 0; i < thisList.size(); ++i)
    {
        dkCouplet currCouplet = thisList.at(i);
        int currNumber = currCouplet.getNumber();
        int newNumber = i+1; // indexes start with 0 numbers from1
        if(currNumber == newNumber)
            continue;
        int nextIndex = getIndexWithNumber(newNumber);
        // there can be only one the same number because isNumerUnique
        if(nextIndex > -1)
        {
            ++maxNumber;
            thisList[nextIndex].setNumber(maxNumber);
            updatePointers(newNumber, maxNumber);
        }
        thisList[i].setNumber(newNumber);
        updatePointers(currNumber, newNumber);
    }
    return true;
}

// isUniqueNumber is not checked it should be done earier
void dkCoupletList::updatePointers(int from, int to)
{
    if(from == to)
        return;
    if(to < 1 || to > maxNumber)
        return;
    for(int i = 0; i < thisList.size(); ++i)
    {
        if(thisList[i].getPointer1() == from)
            thisList[i].setPointer1(to);
        if(thisList[i].getPointer2() == from)
            thisList[i].setPointer2(to);
    }
}
