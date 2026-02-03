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

#include "dkTerm.h"

dkTerm::dkTerm()
{
    clear();
}

dkTerm::dkTerm(const QString &inTxt)
{
    QStringList inList = inTxt.split("\t",Qt::SkipEmptyParts);
    if(inList.size() == 0)
        return;
    else if(inList.size() == 1)
    {
        definition = inList[0];
    }
    else if(inList.size() == 2)
    {
        QString inSynonyms = inList[0];
        synonyms = inSynonyms.split(",", Qt::SkipEmptyParts);
        definition = inList[1];
    }
    else
    {
        QString inSynonyms = inList[0];
        synonyms = inSynonyms.split(",", Qt::SkipEmptyParts);
        for(int i = 1; i < inList.size(); ++i)
        {
            definition.append(inList[i]);
            definition.append(" ");
        }
    }
}

void dkTerm::clear()
{
    synonyms.clear();
    definition = "";
    error.clear();
}

void dkTerm::fromDkXml(const QDomElement &inElement, int ver)
{
    clear();

//    QString numberTxt = inElement.attribute("number");
//    bool ok;
//    number = numberTxt.toInt(&ok);
//    if(!ok)
//    {
//        error += QObject::tr("Error in reading term number. \n");
//        return;
//    }

    QDomNodeList inElemList = inElement.childNodes();
    for(int i = 0; i < inElemList.size(); ++i){
        QDomNode aNode = inElemList.at(i);
        QDomElement anElement = aNode.toElement();
        if(anElement.isNull())
            continue;
        if(anElement.tagName() == "synonyms")
        {
            QString synonymsTxt = anElement.text();
            setSynonyms(synonymsTxt);
        }
        else if(anElement.tagName() == "definition")
        {
            dkString inTxt = anElement.text();
            if(ver > 2)
                inTxt = inTxt.md2html();
            definition = inTxt;
        }
    }
}

QString dkTerm::getHtml(int number) const
{
    QString htmlTxt;
    htmlTxt += QStringLiteral("<p id=\"g%1\" class=\"glossary\">%1. %2</p>\n").arg(number).arg(definition);
    return htmlTxt;
}

QString dkTerm::getDefinition(QString &inKey) const
{
    for(int i = 0; i < synonyms.size(); ++i)
    {
        QString theKey = synonyms[i];
        if(theKey == inKey)
            return definition;
    }
    return QString();
}

//int dkTerm::getNumber() const
//{
//    return number;
//}

QStringList dkTerm::getSynonymsList() const
{
    return synonyms;
}

// keywords separated by comma only
QString dkTerm::getSynonyms1() const
{
    QString outTxt = synonyms.join(",");
    return outTxt;
}

// keywords separated by comma and space
QString dkTerm::getSynonymsTxt() const
{
    QString outTxt = synonyms.join(", ");
    return outTxt;
}

QString dkTerm::getDefinition() const
{
    return definition;
}

QString dkTerm::getTxt() const
{
    QString outTxt = getSynonymsTxt();
    outTxt += " - ";
    outTxt += definition.toPlainText();
    return outTxt;
}

QString dkTerm::exportTxt() const
{
    QString outTxt = getSynonyms1();
    outTxt += "\t";
    outTxt += definition;
    return outTxt;
}

QString dkTerm::getDkXml() const
{
    QString outTxt = QStringLiteral("<term>\n");
    outTxt += QStringLiteral("<synonyms>%1</synonyms>\n").arg(getSynonyms1());
    outTxt += QStringLiteral("<definition>%1</definition>\n").arg(definition.html2mdXml());
    outTxt += "</term>\n";
    return outTxt;
}

//void dkTerm::setNumber(int inVal)
//{
//    number = inVal;
//}

void dkTerm::setSynonyms(QStringList inList)
{
    synonyms = inList;
}

void dkTerm::setSynonyms(QString inTxt)
{
    QStringList inList = inTxt.split(",", Qt::SkipEmptyParts);
    for(int i = 0; i < inList.size(); ++i)
        inList[i] = inList[i].simplified();
    synonyms = inList;
}

void dkTerm::setDefinition(QString inTxt)
{
    definition = inTxt;
}

bool dkTerm::isEmpty() const
{
    if(synonyms.size() == 0 && definition.isEmpty())
        return true;
    else
        return false;
}

bool dkTerm::contains(QString inKey)
{
    for(int i = 0; i < synonyms.size(); ++i)
    {
        QString theKey = synonyms[i];
        if(theKey == inKey)
            return true;
    }
    return false;
}

bool dkTerm::operator < ( const dkTerm &p1 ) const
{
    if(synonyms.size() == 0)
        return true;
    QString string0 = synonyms[0];
    QStringList list1 = p1.getSynonymsList();
    if(list1.size() == 0)
        return false;
    QString string1 = list1[0];
    return (string0 < string1);
}

