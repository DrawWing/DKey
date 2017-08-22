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
    clear();
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

void dkCoupletList::setCouplet(const dkCouplet &inCouplet, int i)
{
    if(i > thisList.size())
        return;
    thisList[i] = inCouplet;

    if(inCouplet.getNumber() > maxNumber)
        maxNumber = inCouplet.getNumber();
}

void dkCoupletList::setFilePath(const QString & inTxt)
{
    filePath = inTxt;
    if(filePath[filePath.size()-1] != '/')
        filePath += "/";

}

int dkCoupletList::size() const
{
    return thisList.size();
}

void dkCoupletList::clear()
{
    thisList.clear();
    endpointList.clear();
    intro.clear();
    introSize = 0;
    maxNumber = 0;
    filePath.clear();
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

//            appendCouplet(coupletTxt);
            dkCouplet newCouplet(coupletTxt);
            thisList.push_back(newCouplet);

            coupletTxt.clear();
            coupletTxt.push_back(line);
        }
        else
            coupletTxt.append(line);
    }
//    appendCouplet(coupletTxt);
    dkCouplet newCouplet(coupletTxt);
    thisList.push_back(newCouplet);

    findMaxNumber();
}

void dkCoupletList::importTxt(QStringList & inTxtList)
{
    clear();

    findIntro(inTxtList);

    QList<int> numbers = findStartNumbers(inTxtList);
    if(numbers.size() < 2)
        return;
    if(numbers[0] == numbers[1])
        parse2numberKey(inTxtList);
    else
    {
        if(isKeyIndented(inTxtList))
            parseIndentedKey(inTxtList);
        else
            parse1numberKey(inTxtList);
    }

    findMaxNumber();
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

    for(int lineNo = introSize + 1; lineNo < inTxtList.size(); ++lineNo)
    {
        dkString line = inTxtList[lineNo];

        if(line.startsWithDigit())
        {
            QString digits = line.frontDigits();

            dkCouplet newCouplet(coupletTxt);
            thisList.push_back(newCouplet);

            coupletTxt.clear();
            coupletTxt.push_back(line);
//            firstLine = lineNo;
        }
        else
            coupletTxt.append(line);
    }
    dkCouplet newCouplet(coupletTxt);
    thisList.push_back(newCouplet);
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
                dkCouplet newCouplet;
                newCouplet.importTxt2(coupletTxt);
                thisList.push_back(newCouplet);

                coupletTxt.clear();
                coupletTxt.push_back(line);
                prevNumber = currNumber;
//                firstLine = lineNo;
            }
        }
        else
            coupletTxt.push_back(line);
    }
    dkCouplet newCouplet;
    newCouplet.importTxt2(coupletTxt);
    thisList.push_back(newCouplet);
}

void dkCoupletList::parseIndentedKey(QStringList &inTxtList)
{
    for(int lineNo = introSize; lineNo < inTxtList.size(); ++lineNo)
    {
        dkString line = inTxtList[lineNo];

        line.removeFrontNonLetterAndDigit();
        dkString digits1 = line.frontDigits();
        int number1 = digits1.toInt();

        line.removeFrontDigit();
        line.removeFrontNonLetterAndDigit();
        dkString digits2 = line.frontDigits();
        int number2 = digits2.toInt();

        if( number1 == 0 || number2 == 0 ) // conversion not succesfull
            continue;

        if( number1 > number2 ) // second lead
        {
            int theIndex = getIndexWithNumber(number2);
            if( theIndex > -1)
                thisList[theIndex].appendIndented(inTxtList[lineNo], number1);
            else
            {
                dkCouplet newCouplet;
                newCouplet.setNumber(number2);
                newCouplet.appendIndented(inTxtList[lineNo], number1);
                thisList.push_back(newCouplet);
            }
        }
        else
        {
            dkCouplet newCouplet;
            newCouplet.importIndented(inTxtList[lineNo], number1);
            thisList.push_back(newCouplet);
        }
    }
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

    findMaxNumber();
}

//void dkCoupletList::appendCouplet(const QStringList & inTxt)
////void dkCoupletList::appendCouplet(const QStringList & inTxt, int lineNo)
//{
//    dkCouplet newCouplet(inTxt);
//    thisList.push_back(newCouplet);

////    QString coupletError = newCouplet.findErrors();
////    if(coupletError.isEmpty())
////        return;
////    int coupletNumber = newCouplet.getNumber();
////    QString errorPrefix;
////    if(coupletNumber == -1)
////        errorPrefix = QString("Line %1").arg(lineNo);
////    else
////        errorPrefix = QString("Line %1, couplet %2\n").arg(lineNo).arg(coupletNumber);
////    error.append(errorPrefix);
////    error.append(coupletError);

//}

QList< dkCouplet > dkCoupletList::getList() const
{
    return thisList;
}

void dkCoupletList::findMaxNumber()
{
    if(thisList.size() == 0)
    {
        maxNumber = 0;
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

QString dkCoupletList::getFilePath() const
{
    return filePath;
}

QString dkCoupletList::getError() const
{
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

QString dkCoupletList::getRtf() const
{
//    QString outTxt = "{\\rtf1\\ansi\\tx426\\tqr\\tldot\\tx8931\n";
    QString outTxt = "{\\rtf1\\ansi"; // rtf version and encoding
    outTxt += "\\paperw11906\\paperh16838"; // A4 paper size
    outTxt += "\\margl1417\\margr1417\\margt1417\\margb1417"; // margings 25 mm
    outTxt += "\\fi-567\\li567"; // hanging indent 10 mm
    outTxt += "\\tx567\\tqr\\tldot\\tx8931\n"; // first tab at 10 mm, second tab to the right with leading dots at right margin
    for(int i = 0; i < thisList.size(); ++i)
    {
        QString theTxt = thisList[i].getRtf();
        outTxt += theTxt;
    }
    outTxt += "}";
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

QString dkCoupletList::getHtmlTab() const
{
    QString htmlTxt;
    htmlTxt += "<table border=\"1\" cellpadding=\"10\" cellspacing=\"0\" style=\"border-collapse: collapse\" bordercolor=\"#111111\" width=\"100%\">\n";

    for(int i = 0; i < thisList.size(); ++i)
    {
        dkCouplet theCouplet = thisList[i];
        QString theHtml = theCouplet.getHtmlTab();
        htmlTxt.append(theHtml);
    }

    htmlTxt += "</table>\n";
    return htmlTxt;
}

QString dkCoupletList::getHtmlImg(bool withPath) const
{
    QString htmlTxt;
    for(int i = 0; i < thisList.size(); ++i)
    {
        QString theHtml;
        if(withPath)
            theHtml = thisList[i].getHtmlImg(filePath);
        else
            theHtml = thisList[i].getHtmlImg();
        htmlTxt += theHtml;
    }
    return htmlTxt;
}

QStringList dkCoupletList::getEndpointList() const
{
    return endpointList;
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

//void dkCoupletList::findFigs(QString & path)
//{
//    for(int i = 0; i < thisList.size(); ++i)
//    {
//        thisList[i].findFigs(path);
//    }
//}

void dkCoupletList::findFigs()
{
    for(int i = 0; i < thisList.size(); ++i)
    {
        thisList[i].findFigs(filePath);
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
    for(int i = 1; i < thisList.size(); ++i)
    {
        int theNumber = thisList[i].getNumber();
        QList<int> theList = findFrom(theNumber);
        thisList[i].setFrom(theList);
    }
}

// return a list of pointers to couplets refering to couplet index
// can be combined with findFrom
QList<int>  dkCoupletList::findFrom(int number) const
{
    QList<int> outList;
    for(int j = 0; j < thisList.size(); ++j)
    {
        int thePointer = thisList[j].getPointer1();
        if(thePointer == number)
            outList.push_back(j);
        thePointer = thisList[j].getPointer2();
        if(thePointer == number)
            outList.push_back(j);
    }
    return outList;
}

// outList is for finding errors and warnings
// endpointList is with added couplet number for interactive key
QStringList dkCoupletList::findEndpoints()
{
    endpointList.clear();
    QStringList outList;
    for(int i = 0; i < thisList.size(); ++i)
    {
        QString theEndpoint = thisList[i].getEndpoint1();
        int theNumber = thisList[i].getNumber();
        if( !theEndpoint.isEmpty() )
        {
            outList.push_back(theEndpoint);
            QString indexed = QString("%1 - %2").arg(theEndpoint).arg(theNumber);
            endpointList.push_back(indexed);
        }
        theEndpoint = thisList[i].getEndpoint2();
        if( !theEndpoint.isEmpty() )
        {
            outList.push_back(theEndpoint);
            QString indexed = QString("%1 - %2").arg(theEndpoint).arg(theNumber);
            endpointList.push_back(indexed);
        }
    }
    endpointList.sort();
    return outList;
}

void dkCoupletList::findRemaining(int inNumber, QList<bool> & outList) const
{
    int i;
//    if(isNumberingOK())
        i = inNumber-1;
//    else
//        theIndex = getIndexWithNumber(inNumber);

        QString theEndpoint1 = thisList[i].getEndpoint1();
        if( !theEndpoint1.isEmpty() )
        {
            QString indexed = QString("%1 - %2").arg(theEndpoint1).arg(thisList[i].getNumber());
            int index = endpointList.indexOf(indexed);
            if(index > -1)
                outList[index] = true;
        }
        else
            findRemaining(thisList[i].getPointer1(), outList);

        QString theEndpoint2 = thisList[i].getEndpoint2();
        if( !theEndpoint2.isEmpty() )
        {
            QString indexed = QString("%1 - %2").arg(theEndpoint2).arg(thisList[i].getNumber());
            int index = endpointList.indexOf(indexed);
            if(index > -1)
                outList[index] = true;
        }
        else
            findRemaining(thisList[i].getPointer2(), outList);

}

QList<bool> dkCoupletList::getRemaining(int inNumber) const
{
    QList<bool> outList;
    if(endpointList.size() == 0)
        return outList;

    int i = inNumber - 1;
    std::list< bool > stdList (endpointList.size(),false);
    outList = QList<bool>::fromStdList(stdList);

    QString theEndpoint1 = thisList[i].getEndpoint1();
    if( !theEndpoint1.isEmpty() )
    {
        QString indexed = QString("%1 - %2").arg(theEndpoint1).arg(thisList[i].getNumber());
        int index = endpointList.indexOf(indexed);
        if(index > -1)
            outList[index] = true;
    }
    else
        findRemaining(thisList[i].getPointer1(), outList);

    QString theEndpoint2 = thisList[i].getEndpoint2();
    if( !theEndpoint2.isEmpty() )
    {
        QString indexed = QString("%1 - %2").arg(theEndpoint2).arg(thisList[i].getNumber());
        int index = endpointList.indexOf(indexed);
        if(index > -1)
            outList[index] = true;
    }
    else
        findRemaining(thisList[i].getPointer2(), outList);

    return outList;
}

void dkCoupletList::push_back(dkCouplet inCouplet)
{
    thisList.push_back(inCouplet);

    if(inCouplet.getNumber() > maxNumber)
        maxNumber = inCouplet.getNumber();
}

// couplet number is updated
// good for copy and append: copy-paste
void dkCoupletList::copyAt(int i, dkCouplet inCouplet)
{
    ++maxNumber;
    inCouplet.setNumber(maxNumber);
    thisList.insert(i, inCouplet);
}

// couplet number is not modified
// good for move: cut-paste
void dkCoupletList::insertAt(int i, dkCouplet inCouplet)
{
    thisList.insert(i, inCouplet);

    if(inCouplet.getNumber() > maxNumber)
        maxNumber = inCouplet.getNumber();
}

void dkCoupletList::insertDummyAt(int i)
{
    ++maxNumber;
    dkCouplet dummyCouplet(maxNumber);
    thisList.insert(i, dummyCouplet);
}

void dkCoupletList::removeAt(int i)
{
    thisList.removeAt(i);
}

//void dkCoupletList::pointerChain(int currNumber, QList<int> &chainList)
//{
//    // couplet numbers need to be unique
//    for(int i = 0; i < thisList.size(); ++i)
//    {
//        dkCouplet prevCouplet = thisList[i];
//        int prevNumber = prevCouplet.getNumber();
//        int prevPointer1 = prevCouplet.getPointer1();
//        int prevPointer2 = prevCouplet.getPointer2();
//        if(currNumber == prevPointer1 || currNumber == prevPointer2)
//        {
//            if(chainList.contains(prevNumber))
//            {
//                chainList.push_back(prevNumber);
//                break; // cyclic numbering
//            }
//            chainList.push_back(prevNumber);
//            pointerChain(prevNumber, chainList);
//            break;
//        }
//    }
//}

void dkCoupletList::pointerChain(int currNumber, QList<int> &chainList,
                                 QList<QString> &path)
{
    // couplet numbers need to be unique
    for(int i = 0; i < thisList.size(); ++i)
    {
        dkCouplet prevCouplet = thisList[i];
        int prevNumber = prevCouplet.getNumber();
        int prevPointer1 = prevCouplet.getPointer1();
        int prevPointer2 = prevCouplet.getPointer2();
        if(currNumber == prevPointer1 || currNumber == prevPointer2)
        {
            if(chainList.contains(prevNumber))
            {
                chainList.push_back(prevNumber);
                break; // cyclic numbering
            }
            if(currNumber == prevPointer1)
                path.push_back(prevCouplet.getLead1());
            else
                path.push_back(prevCouplet.getLead2());
            chainList.push_back(prevNumber);
            pointerChain(prevNumber, chainList, path);
            break;
        }
    }
}

void dkCoupletList::findPointerChains()
{
    for(int i = 0; i < thisList.size(); ++i)
    {
        int theNumber = thisList[i].getNumber();
        QList<int> chainList;
        QList<QString> path;
        pointerChain(theNumber, chainList, path);
        thisList[i].setPointerChain(chainList);
        thisList[i].setLeadChain(path);
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

bool dkCoupletList::isNumberingOK()
{
    error.clear();
    for(int i = 0; i < thisList.size(); ++i)
    {
        if(thisList[i].getNumber() != i+1)
        {
            error = QString("Numbering is not consequtive. You need to renumber the key.\n");
            return false;
        }
    }
    return true;
}

bool dkCoupletList::isContentOK()
{
    error.clear();
    for(int i = 0; i < thisList.size(); ++i)
    {
        bool ok = thisList[i].isContentOK();
        if(!ok)
            error += thisList[i].getError();
    }

    if(error.isEmpty())
        return true;
    else
        return false;
}

// return warning if a couplet is referenced more than one time
bool dkCoupletList::isFromSingle()
{
    findFrom();
    error.clear();

    for(int i = 0; i < thisList.size(); ++i)
    {
        QList<int> fromList = thisList[i].getFrom();
        if(fromList.size() > 1)
            error += QString("Warning: more than one couplet refers to couplet %1.\n")
                    .arg(thisList[i].getNumber());
    }

    if(error.isEmpty())
        return true;
    else
        return false;
}

bool dkCoupletList::isKeyCyclic()
{
    findFrom();
    error.clear();

    for(int i = 0; i < thisList.size(); ++i)
    {
        QList<int> fromList = thisList[i].getFrom();
        for(int j = 0; j < fromList.size(); ++j)
        {
            dkCouplet theCouplet = getCoupletWithNumber(fromList[j]);
            QList<int> chainList = theCouplet.getPointerChain();
            if(chainList.contains(thisList[i].getNumber()))
                error += QString("Warning: numbering is cyclic between couplets: %1 and %2;")
                        .arg(thisList[i].getNumber()).arg(theCouplet.getNumber());
        }
    }

    if(error.isEmpty())
        return true;
    else
        return false;
}

bool dkCoupletList::isEndpointOK()
{
    QStringList endList = findEndpoints();
    error.clear();

    for(int i = 0; i < endList.size(); ++i)
    {
        int count = endList.count(endList[i]);

        if( count > 1)
            error += QString("Warning: endpoint \"%1\" occures %2 times.\n")
                    .arg(endList[i]).arg(count);

        // removed the checked string to prevent multiple messages
        endList.removeAll(endList[i]);
    }

    if(error.isEmpty())
        return true;
    else
        return false;
}

bool dkCoupletList::isPointerOK()
{
    error.clear();
    for(int i = 0; i < thisList.size(); ++i)
    {
        int thePointer1 = thisList[i].getPointer1();
        if(thePointer1 != -1)
        {
            int theIndex = getIndexWithNumber(thePointer1);
            if(theIndex == -1)
                error += QString("Couplet %1: first lead refers to non existant couplet number.\n")
                        .arg(thisList[i].getNumber());
            else if(theIndex == i)
                error += QString("Couplet %1: first lead refers to the same couplet.\n")
                        .arg(thisList[i].getNumber());
//            else if(theIndex < i)
//                error += QString("Couplet %1: first lead refers to an earlier couplet.\n")
//                        .arg(thisList[i].getNumber());

        }
        int thePointer2 = thisList[i].getPointer2();
        if(thePointer2 != -1)
        {
            int theIndex = getIndexWithNumber(thePointer2);
            if(theIndex == -1)
                error += QString("Couplet %1: second lead refers to non existant couplet number.\n")
                        .arg(thisList[i].getNumber());
            else if(theIndex == i)
                error += QString("Couplet %1: second lead refers to the same couplet.\n")
                        .arg(thisList[i].getNumber());
//            else if(theIndex < i)
//                error += QString("Couplet %1: second lead refers to an earlier couplet.\n")
//                        .arg(thisList[i].getNumber());
//            if(thePointer1 == thePointer2)
//                error += QString("Couplet %1: both leads have the same reference.\n")
//                        .arg(thisList[i].getNumber());
        }
    }

    if(error.isEmpty())
        return true;
    else
        return false;
}

bool dkCoupletList::isPointerNoWarning()
{
    error.clear();
    for(int i = 0; i < thisList.size(); ++i)
    {
        int thePointer1 = thisList[i].getPointer1();
        if(thePointer1 != -1)
        {
            int theIndex = getIndexWithNumber(thePointer1);
            if(theIndex < i)
                error += QString("Warning: first lead in couplet %1 refers to an earlier couplet.\n")
                        .arg(thisList[i].getNumber());

        }
        int thePointer2 = thisList[i].getPointer2();
        if(thePointer2 != -1)
        {
            int theIndex = getIndexWithNumber(thePointer2);
            if(theIndex < i)
                error += QString("Warning: second lead in couplet %1 refers to an earlier couplet.\n")
                        .arg(thisList[i].getNumber());
            if(thePointer1 == thePointer2)
                error += QString("Warning: both leads in couplet %1 have the same pointer.\n")
                        .arg(thisList[i].getNumber());
        }
    }

    if(error.isEmpty())
        return true;
    else
        return false;
}

bool dkCoupletList::isPointerChainOK()
{
    findPointerChains();
    error.clear();
    for(int i = 1; i < thisList.size(); ++i)
    {
        QList<int> pointerChain = thisList[i].getPointerChain();
        if(pointerChain.size() == 0)
            error += QString("Couplet %1 has no reference in other couplets.\n")
                    .arg(thisList[i].getNumber());
        else if(pointerChain.last() != 1)
            error += QString("Couplet %1 cannot be reached from couplet 1.\n")
                    .arg(thisList[i].getNumber());

//        // is isPointerChainCyclic
//        for(int j = 0; j < pointerChain.size(); ++j)
//        {
//            int thePointer = pointerChain[j];
//            int count = pointerChain.count(thePointer);
//            if(count > 1)
//            {
//                int first = pointerChain.indexOf(thePointer);
//                int second = pointerChain.indexOf(thePointer, first+1);
//                if(first == -1 || second == -1)
//                    continue;
//                error += QString("Numbering is cyclic: ");
//                for(int k = first; k < second+1; ++k)
//                    error += QString("%1->").arg(pointerChain[k]);
//                error += QString(".\n");
//            }
//        }
    }

    if(error.isEmpty())
        return true;
    else
        return false;
}

bool dkCoupletList::isNumberUnique()
{
    error.clear();
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
                error += QString("The same couplet number: %1 in rows %2 and %3.\n")
                        .arg(currNumber).arg(i).arg(j);
            }
        }
    }
    if(error.isEmpty())
        return true;
    else
        return false;
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
    findMaxNumber();
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

bool dkCoupletList::isKeyIndented(QStringList & inTxtList)
{
    dkString line = inTxtList[introSize]; // first line

    line.removeFrontNonLetterAndDigit();
    if(!line.startsWithDigit())
        return false;
    line.removeFrontDigit();
    line.removeFrontNonLetterAndDigit();
    if(!line.startsWithDigit())
        return false;

    line = inTxtList[introSize+1]; // second line

    line.removeFrontNonLetterAndDigit();
    if(!line.startsWithDigit())
        return false;
    line.removeFrontDigit();
    line.removeFrontNonLetterAndDigit();
    if(!line.startsWithDigit())
        return false;

    return true;
}
