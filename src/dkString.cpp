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

#include <QStringList>
#include <QRegularExpression>
#include <QDomDocument>

#include "dkString.h"

const QString dkString::htmlBr = "<br />";

dkString::dkString()
    : QString()
{
}

dkString::dkString(const QString & str)
    : QString(str)
{
}

bool dkString::startsWithDigit() const
{
    for(int i = 0; i < size(); ++i){
        QChar c = at(i);
        if(c.isLetter())
            return false;
        else if(c.isDigit())
            return true;
    }
    return false;
}

dkString dkString::findEndPart() const
{
    QStringList stringList = split("\t",Qt::SkipEmptyParts);
    if(stringList.size() > 1)
    {
        dkString end = stringList.at(stringList.size()-1);
        return end;
    }
    else
    {
        // split into words separated by spaces and non-words
        static QRegularExpression regex("\\W+");
        stringList = split(regex, Qt::SkipEmptyParts);
        dkString end = stringList.at(stringList.size()-1);
        return end;
    }
}

// findFrontNonLeters
dkString dkString::findFrontPart() const
{
    dkString start;
    for(int i = 0; i < size(); ++i){
        QChar c = at(i);
        if(c.isLetter())
            break;
        else //if(c.isDigit())
            start.append(c);
    }
    return start;
}

dkString dkString::frontDigits() const
{
    dkString outTxt;
    for(int i = 0; i < size(); ++i){
        QChar c = at(i);
        if(c.isDigit())
            outTxt.push_back(c);
        else
            return outTxt;
    }
    return outTxt;
}

dkString dkString::endDigits() const
{
    dkString outTxt;
    for(int i = size()-1; i > -1; --i){
        QChar c = at(i);
        if(c.isDigit())
            outTxt.push_front(c);
        else
            return outTxt;
    }
    return outTxt;
}

void dkString::removeFrontNonLetter()
{
    int count = 0;
    for(int i = 0; i < size(); ++i){
        QChar c = at(i);
        if(c.isLetter())
            break;
        else
            ++count;
    }
    remove(0,count);
}

void dkString::removeEndNonLetterAndDigit()
{
    int count = 0;
    for(int i = size()-1; i > -1; --i){
        QChar c = at(i);
        if(c.isLetter() || c.isDigit() || c == ')')
//            if(c.isLetter() || c.isDigit())
            break;
        else
            ++count;
    }
    chop(count);
}

void dkString::removeFrontNonLetterAndDigit()
{
    int count = 0;
    for(int i = 0; i < size(); ++i){
        QChar c = at(i);
        if(c.isLetter() || c.isDigit())
            break;
        else
            ++count;
    }
    remove(0,count);
}

void dkString::removeFrontDigit()
{
    int count = 0;
    for(int i = 0; i < size(); ++i){
        QChar c = at(i);
        if(c.isDigit())
            ++count;
        else
            break;
    }
    remove(0,count);
}

// remove functions crushes if string is too short
void dkString::chopFront(int n)
{
    if(size() > n)
        remove(0, n);
}

dkString dkString::getRtf() const
{
    QString inTxt = *this;
    inTxt.replace("\\", "\\\\");
    inTxt.replace("{", "\\{");
    inTxt.replace("}", "\\}");
    //    inTxt.replace(htmlBr," ");
    inTxt.replace(htmlBr,"\\line ");

    // without xml tag surrounding the text it is not working
    inTxt.prepend("<dummy>");
    inTxt.append("</dummy>");
    QDomDocument xml;
    xml.setContent(inTxt);
    QDomNodeList spanList = xml.elementsByTagName("span");
    for(int i = 0; i < spanList.size(); ++i)
    {
        QDomNode theNode = spanList.at(i);
        QDomElement theElement = theNode.toElement();
        if(theElement.isNull())
            continue;
        QString styleAttribute = theElement.attribute("style");
        if(styleAttribute.isEmpty())
            continue;
        QStringList styleList = styleAttribute.split(";");
        bool italic = false;
        bool bold = false;
        bool underline = false;
        for(int j = 0; j < styleList.size(); ++j)
        {
            QString theStyle = styleList.at(j);
            theStyle = theStyle.simplified();
            if(theStyle == "font-style:italic")
                italic = true;
            else if(theStyle == "font-weight:600")
                bold = true;
            else if(theStyle == "text-decoration: underline")
                underline = true;
        }
        if(italic || bold || underline)
        {
            QString theText = theElement.text();
            if(italic)
            {
                theText.prepend("\\i ");
                theText.append(" \\i0");
            }
            if(bold)
            {
                theText.prepend("\\b ");
                theText.append(" \\b0");
            }
            if(underline)
            {
                theText.prepend("\\ul ");
                theText.append(" \\ul0");
            }
            xml.documentElement().elementsByTagName("span").at(i).firstChild().setNodeValue(theText);
        }
    }

    dkString outTxt = xml.toString(-1);
    outTxt = outTxt.toPlainText();
    return outTxt;
}

void dkString::removeHtml()
{
    replace(" />","\n");
    static QRegularExpression regex("<[^>]*>");
    remove(regex);
    replace("&lt;","<");
    replace("&gt;",">");
    replace("&quot;","\"");
    replace("&amp;","&");
}

dkString dkString::cleanHtml() const
{
    QString inTxt = *this;

    QDomDocument xml;
    xml.setContent(inTxt);

//    QDomNodeList pNodeList = xml.elementsByTagName("p");
//    int n = pNodeList.size();
//    if(pNodeList.size() == 0)
//        return QString();
//    QDomNode theNode = pNodeList.at(0);
//    QString outTxt = theNode.firstChild().nodeValue();
//    QDomElement pElement = theNode.toElement();
//    pElement.removeAttribute("style");
//    QDomDocument pDoc;
//    // Create new document for html export
//    QDomNode importedNode = pDoc.importNode(theNode, true);
//    pDoc.appendChild(importedNode);
//    QString toTxt = pDoc.toString(-1);
//    QDomElement theElement = theNode.toElement();
//    QString tmp = theElement.text();
//    return outTxt;

    QDomElement htmlElement = xml.firstChildElement("html");
    QDomElement bodyElement = htmlElement.firstChildElement("body");
//    QDomElement pElement = bodyElement.firstChildElement("p");
//    if(pElement.isNull())
//        return QString();
    //    pElement.removeAttribute("style");
    //    QDomDocument pDoc;    // Create new document for html export
    //    QDomNode importedNode = pDoc.importNode(pElement, true);
    //    pDoc.appendChild(importedNode);

    QDomDocument pDoc;    // Create new document for html export
    QDomNodeList pList = bodyElement.elementsByTagName("p");
    for(int i = 0; i < pList.size(); ++i)
    {
        QDomNode pNode = pList.at(i);
        QDomElement pElement = pNode.toElement();
        if(pElement.isNull())
            continue;
        pElement.removeAttribute("style");
        QDomNode importedNode = pDoc.importNode(pElement, true);
        pDoc.appendChild(importedNode);
    }


    QString outTxt = pDoc.toString(-1);
    outTxt = outTxt.remove("<p>");
    outTxt = outTxt.replace("</p>", htmlBr);
    if(outTxt.endsWith(htmlBr))
        outTxt.chop(htmlBr.length());

    return outTxt;
}

//dkString dkString::cleanHtml() const
//{
//    dkString outTxt;
//    unsigned n = 4;
//    //remove first n lines
//    // number of lines to remove changed from 4 in Qt5 to 6 in Qt6
//    QStringList inList = split('\n');
//    if(inList.size() < n)
//        return outTxt;
//    for(unsigned i = 0; i < n; ++i)
//    {
//        inList.pop_front();
//    }
//    // do not separate with '\n'
//    outTxt = inList.join("");

//    // remove last </body></html>
//    outTxt.chop(14);

//    // convert <p> to  />
//    outTxt.remove(QRegularExpression("<p [^>]*>"));
//    outTxt.replace("</p>"," />");
//    // remove last  />
//    if(outTxt.right(6) == " />")
//        outTxt.chop(6);

//    return outTxt;
//}

dkString dkString::toPlainText() const
{
    dkString plainText = QTextDocumentFragment::fromHtml( *this ).toPlainText();
    return plainText;
}

dkString & dkString::operator=( const dkString & str){
    this->QString::operator=(str);
    return *this;
}

dkString & dkString::operator=( const QString & str){
    this->QString::operator=(str);
    return *this;
}

