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
#include <QVector>
#include <QRegularExpression>


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
    if(number < 1 || number > thisList.size() - 1)
        return dkCouplet();

    // if numbered consequitevely number = index +1
    if(thisList[number-1].getNumber() == number)
    {
        return thisList[number-1];
    }

    for(int i = 0; i < thisList.size(); ++i)
    {
        if(thisList[i].getNumber() == number)
        {
            return thisList[i];
        }
    }
    return dkCouplet();
}

int dkCoupletList::getIndexWithNumber(int number) const
{
    // if key is consequtive get the quick result
    if(number > 0 && number < thisList.size()+1)
    {
        if(thisList[number-1].getNumber() == number)
            return number-1;
    }

    // kes is not consequitevely numberd
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

            // appendCouplet(coupletTxt);
            dkCouplet newCouplet(coupletTxt);
            thisList.push_back(newCouplet);

            coupletTxt.clear();
            coupletTxt.push_back(line);
        }
        else
            coupletTxt.append(line);
    }
    // appendCouplet(coupletTxt);
    dkCouplet newCouplet(coupletTxt);
    thisList.push_back(newCouplet);

    findMaxNumber();
}

void dkCoupletList::importTxt(QStringList & inTxtList)
{
    clear();

    findIntro(inTxtList);

    QList<int> numbers = findStartNumbers(inTxtList);
    if(numbers.size() == 0)
    {
        error = "No couplets were found.";
        return;
    }
    if(numbers.size() == 1) // only one couplet
        parse1numberKey(inTxtList);
    else if(numbers[0] == numbers[1])
        parse2numberKey(inTxtList);
    else if(isKeyIndented(inTxtList))
        parseIndentedKey(inTxtList);
    else
        parse1numberKey(inTxtList);

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
            dkCouplet newCouplet(coupletTxt);
            thisList.push_back(newCouplet);

            coupletTxt.clear();
            coupletTxt.push_back(line);
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

void dkCoupletList::fromDkXml(const QDomElement &inElement)
//void dkCoupletList::fromDkXml(const QDomDocument xmlDoc)
{
    clear();

//    QString elementName = "DKey";
//    QDomNode dkeyNode = xmlDoc.namedItem(elementName);
//    QDomElement dkeyElement = dkeyNode.toElement();
//    if ( dkeyElement.isNull() )
//    {
//        error = QObject::tr("No <%1> element found in the XML file!").arg(elementName);
//        return;
//    }
////    QString versionTxt = docElement.attribute("version");
////    double version = versionTxt.toDouble();

//    elementName = "key";
//    QDomNode keyNode = xmlDoc.namedItem(elementName);
//    QDomElement keyElement = dkeyNode.namedItem(elementName).toElement();
//    if ( keyElement.isNull() )
//    {
//        error = QObject::tr("No <%1> element found in the XML file!").arg(elementName);
//        return;
//    }
    QDomNodeList keyChildList = inElement.childNodes();
    for(int i = 0; i < keyChildList.size(); ++i){
        QDomNode coupletNode = keyChildList.at(i);
        QDomElement coupletElement = coupletNode.toElement();
        if(coupletElement.isNull())
            continue;
        if(coupletElement.tagName() == "couplet")
        {
            dkCouplet newCouplet;
            newCouplet.fromDkXml(coupletElement);

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
    findMaxNumber();
}

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

QString dkCoupletList::getIntro() const
{
    return intro;
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

QString dkCoupletList::getDkXml() const
{
//    QString outTxt = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
//    outTxt += "<DKey version=\"1.0\">\n"; //add version from qapplication
    QString outTxt = "<key>\n";
    for(int i = 0; i < thisList.size(); ++i)
    {
        dkCouplet theCouplet = thisList[i];
        QString theTxt = theCouplet.getDkXml();
        outTxt.append(theTxt);
    }
    outTxt += "</key>\n";
//    outTxt += "</DKey>\n";

    return outTxt;
}

QString dkCoupletList::getRtf() const
{
    QString outTxt = "{\\rtf1\\ansi"; // rtf version and encoding
    outTxt += "{\\fonttbl {\\f0 Courier;}}"; // font
    outTxt += "\\paperw11906\\paperh16838"; // A4 paper size
    outTxt += "\\margl1417\\margr1417\\margt1417\\margb1417"; // margings 25 mm
    outTxt += "\\fi-1000\\li1000"; // hanging indent 17.63 mm
    outTxt += "\\tx1000\\tqr\\tldot\\tx8931\n"; // first tab at 17.63 mm, second tab to the right with leading dots at right margin
    for(int i = 0; i < thisList.size(); ++i)
    {
        QString theTxt = thisList[i].getRtf();
        outTxt += theTxt;
    }
    outTxt += "}";

    outTxt.replace("&lt;","<");
    outTxt.replace("&gt;",">");
    outTxt.replace("&quot;","\"");
    outTxt.replace("&amp;","&");

//    outTxt.replace("<br />","\\line ");
//    outTxt.replace("<span style=\" font-style:italic;\">","\\i ");
//    outTxt.replace("</span>","\\i0 ");

    return outTxt;
}

QString dkCoupletList::getTxt() const
{
    QString outTxt;
    for(int i = 0; i < thisList.size(); ++i)
    {
        QString theTxt = thisList[i].getTxt();
        outTxt += theTxt;
    }
    return outTxt;
}

QString dkCoupletList::getHtml() const
{
    QString htmlTxt = "<head><meta charset=\"UTF-8\"/></head>\n";
    htmlTxt += "<style>\n.couplet1 {\n  padding-left: 50px;\n  text-indent: -50px;\n  margin-bottom: 0px;\n}\n";
    htmlTxt += ".couplet2 {\n  padding-left: 50px;\n  text-indent: -50px;\n  margin-top: 0px;\n}\n</style>\n";

    for(int i = 0; i < thisList.size(); ++i)
    {
        dkCouplet theCouplet = thisList[i];
        QString theHtml = theCouplet.getHtml();
        htmlTxt.append(theHtml);
    }

    return htmlTxt;
}

QString dkCoupletList::getHtmlTabulator() const
{
    QString htmlTxt = "<head><meta charset=\"UTF-8\"/></head>\n";
    htmlTxt += "<pre>\n";

    for(int i = 0; i < thisList.size(); ++i)
    {
        dkCouplet theCouplet = thisList[i];
        QString theHtml = theCouplet.getHtmlTabulator();
        htmlTxt.append(theHtml);
    }

    htmlTxt += "</pre>\n";
    return htmlTxt;
}

QString dkCoupletList::getHtmlTab() const
{
    QString htmlTxt = "<head><meta charset=\"UTF-8\"/></head>\n";
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

QString dkCoupletList::getHtmlImg(bool withPath)
{
    findFigs();

    QString htmlTxt;
//    QString htmlTxt = "<head><meta charset=\"UTF-8\"/></head>\n";
    for(int i = 0; i < thisList.size(); ++i)
    {
        htmlTxt += thisList[i].getHtmlImg(filePath, withPath);
    }
    return htmlTxt;
}

QStringList dkCoupletList::getFigList() const
{
    return figList;
}

QStringList dkCoupletList::getEndpointList() const
{
    return endpointList;
}

QStringList dkCoupletList::getTagList() const
{
    return tagList;
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

QStringList dkCoupletList::findFigs()
{
    figList.clear();
    error.clear();
    for(int i = 0; i < thisList.size(); ++i)
    {
        figList += thisList[i].findFigs(filePath);
        error += thisList[i].getError();
    }
    return figList;
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
            outList.push_back(thisList[j].getNumber());
        thePointer = thisList[j].getPointer2();
        if(thePointer == number)
            outList.push_back(thisList[j].getNumber());
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

// find tags which are strings surrounded by []
QStringList dkCoupletList::findTags()
{
    tagList.clear();
    for(int i = 0; i < thisList.size(); ++i)
    {
        int thePointer = thisList[i].getPointer1();
        if( thePointer != -1 )
        {
            dkString theLead = thisList[i].getLead1txt();
            theLead = theLead.toPlainText();
            QRegularExpression tagRegExp("\\[*\\]");
            int tagIndex = theLead.lastIndexOf(tagRegExp);
            if(tagIndex != -1)
            {
                int openingIndex = theLead.lastIndexOf("[");
                ++openingIndex;
                int closingIndex = theLead.lastIndexOf("]");
                QString tag = theLead.mid(openingIndex,closingIndex-openingIndex);
                tag = tag.simplified();
                QString indexed = QString("%1 - %2").arg(tag).arg(thePointer);
                tagList.push_back(indexed);
            }
        }

        thePointer = thisList[i].getPointer2();
        if( thePointer != -1 )
        {
            dkString theLead = thisList[i].getLead2();
            theLead = theLead.toPlainText();
            QRegularExpression tagRegExp("\\[*\\]");
            int tagIndex = theLead.lastIndexOf(tagRegExp);
            if(tagIndex != -1)
            {
                int openingIndex = theLead.lastIndexOf("[");
                ++openingIndex;
                int closingIndex = theLead.lastIndexOf("]");
                QString tag = theLead.mid(openingIndex,closingIndex-openingIndex);
                tag = tag.simplified();
                QString indexed = QString("%1 - %2").arg(tag).arg(thePointer);
                tagList.push_back(indexed);
            }
        }
    }
    tagList.sort();
    tagList.removeDuplicates();
    return tagList;
}

// isNumberingOK needs to be true
void dkCoupletList::findRemaining(int inNumber, QList<bool> & outList) const
{
    int i;
    i = inNumber-1;

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
// qt6    QList<bool> falseList(endpointList.size(), false);
//    outList = falseList;

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

// find chainList recursively
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
                //correct cyclic error reporting
//                error += QString(QObject::tr("Error: numbering is cyclic between couplets: %1 and %2.\n"))
//                        .arg(currNumber).arg(prevNumber);
                break; // cyclic numbering
            }
            QString leadTxt;
            if(currNumber == prevPointer1)
            {
                leadTxt = prevCouplet.getLead1txt();
            }
            else
            {
                leadTxt = prevCouplet.getLead2txt();
            }
            QString longLead = QString ("%1. %2").arg(prevCouplet.getNumber()).arg(leadTxt);
            path.push_back(longLead);

            chainList.push_back(prevNumber);
            pointerChain(prevNumber, chainList, path);
            break;
        }
    }
}

// find tree in newick format
QString dkCoupletList::newick()
{
    QString outTxt = newick(0);
    outTxt += ";";
    return outTxt;
}

// find tree in newick format
QString dkCoupletList::newick(int currNumber)
{
    QString outTxt = "(";
    dkCouplet theCouplet = thisList[currNumber];

    int theNumber = theCouplet.getNumber();
    QString numberTxt = QString::number(theNumber);

    int thePointer1 = theCouplet.getPointer1();
    if(thePointer1 == -1)
    {
        outTxt += QStringLiteral("\"%1 - %2\"").arg(theCouplet.getEndpoint1()).arg(numberTxt);
    }
    else
    {
        int theIndex = getIndexWithNumber(thePointer1);
        if(theIndex == -1)
            outTxt += "error";
        else
        {
            QList<int> theFrom = thisList[theIndex].getFrom();
            if(theFrom.size() == 1)
                outTxt += newick(theIndex);
            else if(theFrom[0] == theNumber)
                outTxt += newick(theIndex);
            else
                outTxt += QStringLiteral("go from node %1 to node %2").arg(theNumber).arg(thePointer1);
        }
    }
    outTxt += ",";

    int thePointer2 = theCouplet.getPointer2();
    if(thePointer2 == -1)
    {
        outTxt += QStringLiteral("\"%1 - %2\"").arg(theCouplet.getEndpoint2()).arg(numberTxt);
    }
    else
    {
        int theIndex = getIndexWithNumber(thePointer2);
        if(theIndex == -1)
            outTxt += "error";
        else
        {
            QList<int> theFrom = thisList[theIndex].getFrom();
            if(theFrom.size() == 1)
                outTxt += newick(theIndex);
            else if(theFrom[0] == theNumber)
                outTxt += newick(theIndex);
            else
                outTxt += QStringLiteral("go from node %1 to node %2").arg(theNumber).arg(thePointer2);
        }
    }
    outTxt += ")";
    outTxt += numberTxt;

    return outTxt;
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

void dkCoupletList::arrange()
{
    QVector<int> treeLgh = findTreeLength();
    QList< dkCouplet > newList;
    arrangeCouplets(1, newList, treeLgh);
    thisList = newList;
}

// couplet numbers need to be consequtive; index = number -1
QVector<int> dkCoupletList::findTreeLength()
{
    QVector<int> outList(thisList.size(), -1);
    // at start filled with -1

    bool progress = true;
    while(progress)
    {
        progress = false;
        for(int i = thisList.size()-1; i > -1; --i)
        {
            if(outList[i] > -1)
                continue;
            else
                progress = true;

            dkCouplet theCouplet = thisList[i];

            int size1 = -1;
            int thePointer1 = theCouplet.getPointer1();
            if(thePointer1 > -1)
            {
                if(outList[thePointer1 - 1] > -1)
                    size1 = outList[thePointer1 - 1] + 1;
            }
            else
            {
                size1 = 0;
            }

            int size2 = -1;
            int thePointer2 = theCouplet.getPointer2();
            if(thePointer2 > -1)
            {
                if(outList[thePointer2 - 1] > -1)
                    size2 = outList[thePointer2 - 1] + 1;
            }
            else
            {
                size2 = 0;
            }

            if(size1 == -1 || size2 == -1)
                continue;
            else if(size1 > size2)
                outList[i] = size1;
            else
                outList[i] = size2;
        }
    }
    return outList;
}

void dkCoupletList::arrangeCouplets(int currNumber, QList< dkCouplet > & newList, QVector<int> &treeLgh)
{
    dkCouplet theCouplet = getCoupletWithNumber(currNumber);
    if(theCouplet.isEmpty())
        return; // couplet with the number was not found

    int size1;
    int thePointer1 = theCouplet.getPointer1();
    if(thePointer1 == -1)
        size1 = 0;
    else
        size1 = treeLgh[thePointer1 - 1] + 1;

    int size2;
    int thePointer2 = theCouplet.getPointer2();
    if(thePointer2 == -1)
        size2 = 0;
    else
        size2 = treeLgh[thePointer2 - 1] + 1;

    if(size1 > size2)
        theCouplet.swapLeads();

    newList.push_back(theCouplet);
    thePointer1 = theCouplet.getPointer1(); // in case of swapping
    if(thePointer1 > -1)
        arrangeCouplets(thePointer1, newList, treeLgh);
    thePointer2 = theCouplet.getPointer2(); // in case of swapping
    if(thePointer2 > -1)
        arrangeCouplets(thePointer2, newList, treeLgh);
}

bool dkCoupletList::isNumberingOK()
{
    error.clear();
    for(int i = 0; i < thisList.size(); ++i)
    {
        if(thisList[i].getNumber() != i+1)
        {
            error = QString(QObject::tr("Numbering is not consequtive. You need to renumber the key.\n"));
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
            error += QString(QObject::tr("Warning: more than one couplet refers to couplet %1.\n"))
                    .arg(thisList[i].getNumber());
    }

    if(error.isEmpty())
        return true;
    else
        return false;
}

bool dkCoupletList::isCircularityOK()
{
    findFrom();
    error.clear();
    for(int i = 0; i < thisList.size(); ++i)
    {
        int thePointer1 = thisList[i].getPointer1();
        if(thePointer1 != -1)
        {
            int theIndex = getIndexWithNumber(thePointer1);
            if(theIndex < i)
            {
                if( isNumberInChain(i, thePointer1))
                {
                    error += QString(QObject::tr("Error: numbering is cyclic between couplets: %1 and %2.\n"))
                            .arg(thisList[i].getNumber()).arg(thePointer1);
                    return false; // it is not safe to continue
                }

            }
        }
        int thePointer2 = thisList[i].getPointer2();
        if(thePointer2 != -1)
        {
            int theIndex = getIndexWithNumber(thePointer2);
            if(theIndex < i)
            {
                if( isNumberInChain(i, thePointer2))
                {
                    error += QString(QObject::tr("Error: numbering is cyclic between couplets: %1 and %2.\n"))
                            .arg(thisList[i].getNumber()).arg(thePointer2);
                    return false; // it is not safe to continue
                }
            }
        }
    }

    return true;
}

bool dkCoupletList::isEndpointOK()
{
    QStringList endList = findEndpoints();
    error.clear();

    for(int i = 0; i < endList.size(); ++i)
    {
        int count = endList.count(endList[i]);

        if( count > 1)
            error += QString(QObject::tr("Warning: endpoint \"%1\" occures %2 times.\n"))
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
                error += QString(QObject::tr("Couplet %1: first lead refers to non existant couplet number.\n"))
                        .arg(thisList[i].getNumber());
            else if(theIndex == i)
                error += QString(QObject::tr("Couplet %1: first lead refers to the same couplet.\n"))
                        .arg(thisList[i].getNumber());

        }
        int thePointer2 = thisList[i].getPointer2();
        if(thePointer2 != -1)
        {
            int theIndex = getIndexWithNumber(thePointer2);
            if(theIndex == -1)
                error += QString(QObject::tr("Couplet %1: second lead refers to non existant couplet number.\n"))
                        .arg(thisList[i].getNumber());
            else if(theIndex == i)
                error += QString(QObject::tr("Couplet %1: second lead refers to the same couplet.\n"))
                        .arg(thisList[i].getNumber());
        }
    }

    if(error.isEmpty())
        return true;
    else
        return false;
}

// find circularity when multiple references are present
bool dkCoupletList::isNumberInChain(int index, int inNumber)
{
    QList<int> fromList = thisList[index].getFrom();
    for(int i = 0; i < fromList.size(); ++i)
    {
        if(fromList[i] == inNumber)
            return true;

        int nextIndex = getIndexWithNumber(fromList[i]);
        if(isNumberInChain(nextIndex, inNumber))
            return true;
    }
    return false;
}

bool dkCoupletList::isPointerIncreasing()
{
    error.clear();
    for(int i = 0; i < thisList.size(); ++i)
    {
        int thePointer1 = thisList[i].getPointer1();
        if(thePointer1 != -1)
        {
            int theIndex = getIndexWithNumber(thePointer1);
            if(theIndex < i)
            {
                error += QString(QObject::tr("Warning: first lead in couplet %1 refers to an earlier couplet.\n"))
                        .arg(thisList[i].getNumber());
            }
        }
        int thePointer2 = thisList[i].getPointer2();
        if(thePointer2 != -1)
        {
            int theIndex = getIndexWithNumber(thePointer2);
            if(theIndex < i)
            {
                error += QString(QObject::tr("Warning: second lead in couplet %1 refers to an earlier couplet.\n"))
                        .arg(thisList[i].getNumber());
            }
            if(thePointer1 == thePointer2)
                error += QString(QObject::tr("Warning: both leads in couplet %1 have the same pointer.\n"))
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
            error += QString(QObject::tr("Couplet %1 has no reference in other couplets.\n"))
                    .arg(thisList[i].getNumber());
        else if(!pointerChain.contains(1))
//            else if(pointerChain.last() != 1)
            error += QString(QObject::tr("Couplet %1 cannot be reached from couplet 1.\n"))
                    .arg(thisList[i].getNumber());
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
                error += QString(QObject::tr("The same couplet number: %1 in rows %2 and %3.\n"))
                        .arg(currNumber).arg(i+1).arg(j+1);
            }
        }
    }
    if(error.isEmpty())
        return true;
    else
        return false;
}

// startNumber is for coppy and append
bool dkCoupletList::reNumber(int startNumber)
{
    if(!isNumberUnique())
        return false;

    for(int i = 0, newNumber = startNumber; i < thisList.size(); ++i, ++newNumber)
    {
        dkCouplet currCouplet = thisList.at(i);
        int currNumber = currCouplet.getNumber();
        if(currNumber == newNumber)
        {
            continue;
        }
        int nextIndex = getIndexWithNumber(newNumber);
        // there can be only one the same number because isNumerUnique
        if(nextIndex > -1)
        {
            ++maxNumber;
            thisList[nextIndex].setNumber(maxNumber);
            updatePointers(newNumber, maxNumber);
        }
        thisList[i].setNumber(newNumber);
        if(newNumber > maxNumber)
            maxNumber = newNumber;
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

int dkCoupletList::getLastNumber(const QString & inString)
{
    QStringList theList = inString.split("-");
    QString theString = theList.last().simplified();
    int theNumber = theString.toInt();
    if(theNumber < 1 || theNumber > maxNumber)
        return -1;
    else
        return theNumber;
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
