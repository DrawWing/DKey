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

#include "dkTermList.h"
#include <QRegularExpression>

dkTermList::dkTermList()
{
    clear(); // ???
}

dkTerm dkTermList::at(int i) const
{
    return thisList.at(i);
}

void dkTermList::setTerm(const dkTerm &inTerm, int i)
{
    if(i > thisList.size())
        return;
    thisList[i] = inTerm;
}

void dkTermList::setTag(const QString &inTxt)
{
    tag = inTxt;
}

int dkTermList::size() const
{
    return thisList.size();
}

void dkTermList::clear()
{
    thisList.clear();
    tag.clear();
    error.clear();
}

void dkTermList::fromDkXml(const QDomElement &inElement, int ver)
{
    clear();

    QDomNodeList inElemList = inElement.childNodes();
    for(int i = 0; i < inElemList.size(); ++i){
        QDomNode theNode = inElemList.at(i);
        QDomElement theElement = theNode.toElement();
        if(theElement.isNull())
            continue;
        if(theElement.tagName() == "term")
        {
            dkTerm newTerm;
            newTerm.fromDkXml(theElement, ver);
            thisList.push_back(newTerm);
        }
    }
}

void dkTermList::importTxt(QStringList &inTxtList)
{
    for(int i = 0; i < inTxtList.size(); ++i)
    {
        dkTerm inTerm(inTxtList[i]);
        if(!inTerm.isEmpty())
            thisList.push_back(inTerm);
    }
}

QString dkTermList::exportTxt()
{
    QString outTxt;
    for(int i = 0; i < thisList.size(); ++i)
    {
        dkTerm theTerm = thisList[i];
        outTxt.append(theTerm.exportTxt());
        outTxt.append("\n");
    }
    return outTxt;
}

QString dkTermList::getTag() const
{
    return tag;
}

QString dkTermList::getDkXml() const
{
    QString outTxt;
    if(thisList.size() == 0)
        return outTxt;

    outTxt += QStringLiteral("<%1>\n").arg(tag);
    for(int i = 0; i < thisList.size(); ++i)
    {
        dkTerm theTerm = thisList[i];
        QString theTxt = theTerm.getDkXml();
        outTxt.append(theTxt);
    }
    outTxt += QStringLiteral("</%1>\n").arg(tag);

    return outTxt;
}

QString dkTermList::getHtml() const
{
    QString htmlTxt = "Glossary\n";

    for(int i = 0; i < thisList.size(); ++i)
    {
        dkTerm theTerm = thisList[i];
        QString theHtml = theTerm.getHtml(i+1);
        htmlTxt.append(theHtml);
    }

    return htmlTxt;
}

QString dkTermList::getDefinition(QString &inKey) const
{
    for(int i = 0; i < thisList.size(); ++i)
    {
        QString outTxt = thisList[i].getDefinition(inKey);
        if(!outTxt.isNull())
            return outTxt;
    }
    return QString();
}

QString dkTermList::getDefinitionTxt(QString &inKey) const
{
    dkString outTxt = getDefinition(inKey);
    if(outTxt.isNull())
        return QString();
    outTxt = outTxt.toPlainText();
    return outTxt;
}

QString dkTermList::linkGlossary(QString &inTxt) const
{
    for(int i = 0; i < thisList.size(); ++i)
    {
        dkTerm theTerm = thisList[i];
        QStringList synonymsList = theTerm.getSynonymsList();
        for(int j = 0; j < synonymsList.size(); ++j)
        {
            QString theSynonym = synonymsList[j];
            theSynonym = theSynonym.simplified();
            QString linkHtml = QStringLiteral("<a href=\"#g%1\">%2</a>").arg(i+1).arg(theSynonym);
            inTxt = inTxt.replace(theSynonym,linkHtml,Qt::CaseSensitive); // better modify what is found
//            inTxt = inTxt.replace(theSynonym,linkHtml,Qt::CaseInsensitive);
        }
    }
    return inTxt;
}

QList< dkStringInt > dkTermList::sortBySize() const
{
    QList< dkStringInt > list1;
    QList< dkStringInt > list2;
    QList< dkStringInt > list3;
    QList< dkStringInt > list4;

    for(int i = 0; i < thisList.size(); ++i)
    {
        dkTerm theTerm = thisList[i];
        QStringList synList = theTerm.getSynonymsList();
        for(int j = 0; j < synList.size(); ++j)
        {
            QString key = synList[j];
            dkStringInt keyInt(key, i+1); // glossary numbers start with 1
            QStringList keyList = key.split(QRegularExpression("\\W+")); // the same split as in dkFormat
            int keySize = keyList.size();
            switch ( keySize ) {
            case 0:
                // skip empty keywords
                break;
            case 1:
                list1.push_back(keyInt);
                break;
            case 2:
                list2.push_back(keyInt);
                break;
            case 3:
                list3.push_back(keyInt);
                break;
            default:
                list4.push_back(keyInt);
                break;
            }
        }
    }

    // start from longest
    list4.append(list3);
    list4.append(list2);
    list4.append(list1);
    return list4;
}

void dkTermList::sort()
{
    std::sort(thisList.begin(), thisList.end());
}

// if thisList contains tag with synonym inKey return term index
// if no inKey was found return -1
int dkTermList::contains(QString inKey)
{
    for(int i = 0; i < thisList.size(); ++i)
    {
        dkTerm theTerm = thisList[i];
        if(theTerm.contains(inKey))
            return i+1;
    }
    return -1;
}

void dkTermList::push_back(dkTerm inTerm)
{
    thisList.push_back(inTerm);
}

// couplet number is not modified
// good for move: cut-paste
void dkTermList::insertAt(int i, dkTerm inTerm)
{
    thisList.insert(i, inTerm);
}

void dkTermList::insertDummyAt(int i)
{
    dkTerm dummyTerm;
    thisList.insert(i, dummyTerm);
}

void dkTermList::removeAt(int i)
{
    thisList.removeAt(i);
}

// add links to inHtmlTxt
QString dkTermList::addLinks(QString &inHtmlTxt)
{
    QList< dkStringInt > keyList = sortBySize();

    QStringList inHtmlList = inHtmlTxt.split('\n');
    for(int i = 0; i < inHtmlList.size(); ++i)
    {
        QString htmlTxt = inHtmlList[i];
        QString plainTxt = QTextDocumentFragment::fromHtml( htmlTxt ).toPlainText();
        QStringList plainList = plainTxt.split(QRegularExpression("\\W+")); // only words no separators
        QStringList htmlList = htmlTxt.split(QRegularExpression("\\b")); // words and separators

        for(int j = 0; j < keyList.size(); ++j)
        {
            dkStringInt theKey = keyList[j];
            QString keyString = theKey.getString();
            int keyInt = theKey.getInt();

            QStringList kList = keyString.split(QRegularExpression("\\W+")); // only words no separators
            if(kList.size() == 0)
                continue;

            if(isKeyPresent(kList, plainList))
                linkKey(kList, keyInt, htmlList);
        }

        inHtmlList[i] = htmlList.join("");
    }

    QString outTxt = inHtmlList.join('\n');
    return outTxt;
}

// inList is plain text list of words only
bool dkTermList::isKeyPresent(QStringList &keyList, QStringList &inList)
{
    if(inList.size() < keyList.size())
        return false; // inList to short

    for(int j = 0; j < keyList.size(); ++j)
    {
        if(!inList.contains(keyList[j], Qt::CaseInsensitive))
            return false;
    }

    for(int i = 0; i < inList.size(); ++i)
    {
        if(inList.size() < i + keyList.size())
            return false; // inList to short
        for(int j = 0; j < keyList.size(); ++j)
        {
            if(inList[i+j] != keyList[j])
                break;
        }
        return true;
    }
    return false;
}

void dkTermList::linkKey(QStringList &keyList, int keyIndex, QStringList &inList)
{
    if(inList.size() < keyList.size())
        return; // inList to short

    bool isCode = false;
    bool isLink = false;
    int keyPosition = 0; // index of key in keywords consisting from more thoan one word
    int startIndex = 0; // index of inList at which key starts
    for(int i = 0; i < inList.size(); ++i)
    {
        QString theTxt = inList[i]; // for debuging only

        if(isLink) // inside link
        {
            if(inList[i].contains("</a>")) // inserted link ended
            {
                isLink = false;
                continue;
            }
        }

        if(isCode) // inside html code, no plain text
        {
            if(inList[i].contains('>'))
            {
                if(i > 0)
                    if(inList[i-1] == "a")
                        isLink = false;

                if(inList[i].contains('<')) // there can be both > and <
                {
                    if(i+1 < inList.size())
                        if(inList[i+1] == "a")
                            isLink = true;
                }
                else
                    isCode = false;

                continue;
            }
            continue;
        }

        if(inList[i].startsWith("<a") ) // inserted link started
        {
            if(inList[i].contains("</a>")) // link ended in the same part
                continue;
            isLink = true;
            continue;
        }

//        if(inList[i].contains('<') && !inList[i].contains('>')) // html code started
//        {
//                isCode = true;
//            continue;
//        }
        if(inList[i].contains('<') && !inList[i].contains('>')) // html code started
        {
            isCode = true;
            if(i+1 < inList.size())
                if(inList[i+1] == "a")
                    isLink = true;
            continue;
        }

        if(isLink)
            continue;

        if(!(inList[i][0].isLetter() || inList[i][0].isDigit())) // is first character not letter or digit
            continue; // separator

        if(i > 0)
            if(inList[i-1].contains('&')) // is this escape sequence?
                continue;

        if(inList[i].toLower() == keyList[keyPosition]) // comparison is case insensitive
        {
            if(keyPosition == 0)
                startIndex = i; // keyword started

            if(keyList.size() == keyPosition + 1)
            {
                // key was found

                QString prefix;
                if(tag == "glossary")
                    prefix = QStringLiteral("<a href=\"#g%1\">").arg(keyIndex);
                else if(tag == "endpoints")
                    prefix = QStringLiteral("<a href=\"#e%1\">").arg(keyIndex);

                inList[startIndex].prepend(prefix);
                inList[i].append("</a>");

                keyPosition = 0;
                startIndex = 0; //reset key counter
            }
            else
                ++keyPosition;
        }
        else
            keyPosition = 0;

    }
    return;
}
