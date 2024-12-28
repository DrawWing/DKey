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

#include <QImageReader>
#include <QTextDocumentFragment> // for toPlainText

#include "dkFormat.h"

dkFormat::dkFormat()
{
    figures = new dkTermList;
    glossary = new dkTermList;
}

void dkFormat::clear()
{
    filePath.clear();
    figFiles.clear();
    error.clear();
}

void dkFormat::setFilePath(const QString & inTxt)
{
    if(inTxt.isEmpty())
        return;
    filePath = inTxt;
    if(filePath[filePath.size()-1] != '/')
        filePath += "/";
}

void dkFormat::setGlossary(dkTermList * inList)
{
    glossary = inList;
}

void dkFormat::setEndpoints(dkTermList * inList)
{
    endpoints = inList;
}

void dkFormat::setFigures(dkTermList * inList)
{
    figures = inList;
}

QString dkFormat::keyHtml(dkCoupletList & inList, bool withPath)
{
    QString htmlTxt = "\n<h3 id=\"k0\">Taxonomic key</h3>\n";

    for(int i = 0; i < inList.size(); ++i)
    {
        dkCouplet theCouplet = inList.at(i);
        htmlTxt += coupletHtml(theCouplet, withPath);
//        theCouplet.findFigs(filePath);
////        htmlTxt += theCouplet.getHtmlImg(filePath, withPath);
//        QString lead1 = theCouplet.getLead1();
//        QString lead2 = theCouplet.getLead2();
//        QString endpoint1 = theCouplet.getEndpoint1();
//        QString endpoint2 = theCouplet.getEndpoint2();
//        int pointer1 = theCouplet.getPointer1();
//        int pointer2 = theCouplet.getPointer2();

//        htmlTxt += QStringLiteral("<br /><div id=\"k%1\">%1%2</div>").arg(theCouplet.getNumber()).arg(theCouplet.previousTxt());
//        htmlTxt += "<table border=\"1\" cellpadding=\"10\" cellspacing=\"0\" style=\"border-collapse: collapse\" bordercolor=\"#111111\" width=\"100%\">";
//        htmlTxt += "<tr>";
//        htmlTxt += "<td align=\"left\" valign=\"top\" width=\"50%\">\n";
//        if(endpoint1.isEmpty())
//            htmlTxt += QStringLiteral("%1<br><a href=\"#k%2\">%2</a><br />\n").arg(lead1).arg(pointer1);
//        else
//            htmlTxt += QStringLiteral("%1<br><b>%2</b><br />\n").arg(lead1).arg(endpoint1);

//        htmlTxt += imagesHtml(lead1, withPath);
////        QStringList figList = findFigs(lead1);
////        for(int j = 0; j < figList.size(); ++j)
////            htmlTxt += imgHtml(figList[j], withPath);

//        htmlTxt += "</td>";

//        htmlTxt += "<td align=\"left\" valign=\"top\" width=\"50%\">\n";
//        if(endpoint2.isEmpty())
//            htmlTxt += QStringLiteral("%1<br><a href=\"#k%2\">%2</a><br />\n").arg(lead2).arg(pointer2);
//        else
//            htmlTxt += QStringLiteral("%1<br><b>%2</b><br />\n").arg(lead2).arg(endpoint2);

//        htmlTxt += imagesHtml(lead2, withPath);
////        figList = findFigs(lead2);
////        for(int j = 0; j < figList.size(); ++j)
////            htmlTxt += imgHtml(figList[j], withPath);

//        htmlTxt += "</td>";

//        htmlTxt += "</tr>\n";
//        htmlTxt += "</table>\n";
    }
    return htmlTxt;
}

QString dkFormat::coupletHtml(dkCouplet &theCouplet, bool withPath)
{
    QString lead1 = theCouplet.getLead1();
    addFigLinks(lead1);
    QString lead2 = theCouplet.getLead2();
    addFigLinks(lead2);
    QString endpoint1 = theCouplet.getEndpoint1();
    QString endpoint2 = theCouplet.getEndpoint2();
    int pointer1 = theCouplet.getPointer1();
    int pointer2 = theCouplet.getPointer2();

    QString htmlTxt;
    htmlTxt += QStringLiteral("<div id=\"k%1\">%1%2</div>").arg(theCouplet.getNumber()).arg(theCouplet.previousTxt());
    htmlTxt += "<table border=\"1\" cellpadding=\"10\" cellspacing=\"0\" style=\"border-collapse: collapse\" bordercolor=\"#111111\" width=\"100%\">";
    htmlTxt += "<tr>";
    htmlTxt += "<td align=\"left\" valign=\"top\" width=\"50%\">\n";
    if(endpoint1.isEmpty())
        htmlTxt += QStringLiteral("%1<br><a href=\"#k%2\">%2</a><br />\n").arg(lead1).arg(pointer1);
    else
        htmlTxt += QStringLiteral("%1<br><b>%2</b><br />\n").arg(lead1).arg(endpoint1);

    htmlTxt += imagesHtml(lead1, withPath);
    htmlTxt += "</td>";

    htmlTxt += "<td align=\"left\" valign=\"top\" width=\"50%\">\n";
    if(endpoint2.isEmpty())
        htmlTxt += QStringLiteral("%1<br><a href=\"#k%2\">%2</a><br />\n").arg(lead2).arg(pointer2);
    else
        htmlTxt += QStringLiteral("%1<br><b>%2</b><br />\n").arg(lead2).arg(endpoint2);

    htmlTxt += imagesHtml(lead2, withPath);
    htmlTxt += "</td>";
    htmlTxt += "</tr>";
    htmlTxt += "</table><br />\n";
    return htmlTxt;
}

//QString dkFormat::glossaryHtml(dkTermList & inList, bool withPath)
QString dkFormat::glossaryHtml(bool withPath)
{
    if(glossary->size() == 0)
        return QString();

    QString htmlTxt = "\n<h3 id=\"g0\">Glossary</h3>\n";

    for(int i = 0; i < glossary->size(); ++i)
//        for(int i = 0; i < inList.size(); ++i)
    {
//        dkTerm theTerm = inList.at(i);
        dkTerm theTerm = glossary->at(i);
        QString theTxt = theTerm.getDefinition();

        htmlTxt += QStringLiteral("<p id=\"g%1\" class=\"glossary\">%1. %2</p>\n").arg(i+1).arg(theTxt);

        htmlTxt += imagesHtml(theTxt, withPath);
//        QStringList figList = findFigs(theTxt);
//        for(int j = 0; j < figList.size(); ++j)
//            htmlTxt += imgHtml(figList[j], withPath);
    }

    return htmlTxt;
}

QString dkFormat::endpointsHtml(bool withPath)
{
    if(endpoints->size() == 0)
        return QString();

    QString htmlTxt = "\n<h3 id=\"e0\">Endpoints</h3>\n";
    for(int i = 0; i < endpoints->size(); ++i)
    {
        dkTerm theTerm = endpoints->at(i);
        QString theTxt = theTerm.getDefinition();

        htmlTxt += QStringLiteral("<p id=\"e%1\" class=\"glossary\">%1. %2</p>\n").arg(i+1).arg(theTxt);

        htmlTxt += imagesHtml(theTxt, withPath);
    }

    return htmlTxt;
}

QString dkFormat::figuresHtml(bool withPath)
{
    if(figures->size() == 0)
        return QString();

    QString htmlTxt = "\n<h3 id=\"f0\">Figures</h3>\n";
    for(int i = 0; i < figures->size(); ++i)
    {
        dkTerm theTerm = figures->at(i);
        QString theTxt = theTerm.getDefinition();

        htmlTxt += QStringLiteral("<p id=\"f%1\" class=\"glossary\">%1. %2</p>\n").arg(i+1).arg(theTxt);

        htmlTxt += imagesHtml(theTxt, withPath);
    }

    return htmlTxt;
}

//used only in dkBrowser and dkEditor
QString dkFormat::glossHtml(int nr, bool withPath)
{
    dkTerm theTerm = glossary->at(nr);
    QString theTxt = theTerm.getDefinition();

    QString htmlTxt = QStringLiteral("<p id=\"g%1\" class=\"glossary\">%1. %2</p>\n").arg(nr+1).arg(theTxt);

    htmlTxt = glossary->addLinks(htmlTxt);

    htmlTxt += imagesHtml(theTxt, withPath);
    return htmlTxt;
}

QString dkFormat::imagesHtml(QString &inTxt, bool withPath)
{
    QString outTxt;
    QStringList figList = findFigs(inTxt);
    for(int j = 0; j < figList.size(); ++j)
        outTxt += imgHtml(figList[j], withPath);
    return outTxt;
}

QString dkFormat::imgHtml(const QString & inName, bool withPath) const
{
    const int imgMaxSize = 401;

    QString htmlTxt;
    QString imgFileName = filePath + inName;
    QImageReader imgReader(imgFileName);
    int x = imgReader.size().width();
    int y = imgReader.size().height();

    QString outName;
    if(withPath)
        outName = imgFileName;
    else
        outName = inName;

    QString key = inName;
    key.remove(0,4);
    key.chop(4);

    dkString figTitle = figures->getDefinitionTxt(key);

    htmlTxt = QStringLiteral("<img border=\"1\" src=\"%1\"").arg(outName);
    if(!figTitle.isEmpty())
        htmlTxt += QStringLiteral(" title=\"%1\"").arg(figTitle);

    if( x < imgMaxSize && y < imgMaxSize)
        htmlTxt += QStringLiteral(">\n");
    else if( x > y)
        htmlTxt += QStringLiteral(" width=\"%1\">\n").arg(imgMaxSize);
    else
        htmlTxt += QStringLiteral(" height=\"%1\">\n").arg(imgMaxSize);

    return htmlTxt;
}

// include qdir
QStringList dkFormat::findFigs(QString & inTxt)
{
    QStringList outList;

    QString figPrefix = "fig.";

    QDir keyDir(filePath);

    // split text using brackets in order to restrict search for figures
    QStringList bracetList = inTxt.split("(", Qt::SkipEmptyParts, Qt::CaseInsensitive);
    for(int i = 0; i < bracetList.size(); ++i)
    {
        QString theString = bracetList.at(i);
        if(!theString.contains(figPrefix, Qt::CaseInsensitive))
            continue;

        // if immedietly after brace ther is "fig." there can be list of images
        if(theString.startsWith(figPrefix, Qt::CaseInsensitive) && i > 0)
        {
            QStringList closingBracetList = theString.split(")", Qt::SkipEmptyParts, Qt::CaseInsensitive);
            theString = closingBracetList[0]; // take only the part before closing parenthesis
            // split the string into an alternating sequence of non-word and word tokens
            QStringList subStrList = theString.split(QRegularExpression("\\b"));

            for(int j = 3; j < subStrList.size(); ++j) // start with third 0-separator, 1-fig, 2-separator
            {
                QString subString = subStrList.at(j);
                subString = subString.simplified();
                subString.prepend("fig-");

                QString fileName = figExist(keyDir, subString);
                if(fileName.isEmpty())
                {
//                    error += QString("Image %1 from couplet number %2 was not found.\n").arg(subString).arg(number);
                } else
                {
                    if(!outList.contains(fileName))
                        outList.push_back(fileName);
                }
                ++j; //sip separator
            }
        }
        else // find only one imgage file
        {
            QStringList subStrList = theString.split(QRegularExpression("\\W+")); // only words no separators

            for(int j = 0; j < subStrList.size(); ++j)
            {
                QString subString = subStrList.at(j);
                if(subString == "fig" || subString == "Fig")
                {
                    subString = subStrList.at(j+1);
                    subString.prepend("fig-");
                    QString fileName = figExist(keyDir, subString);
                    if(!fileName.isEmpty())
                    {
                        if(!outList.contains(fileName))
                            outList.push_back(fileName);
                    }
                }
            }
        }
    }

    return outList;
}

// slower that checking QStringList
QString dkFormat::figExist(QDir & keyDir, QString & inTxt) const
{
    QStringList extensionList;
    extensionList << ".png" << ".jpg";
    for(int i = 0; i < extensionList.size(); ++i)
    {
        QString baseName = inTxt;
        baseName.append(extensionList[i]);
        QFileInfo figFileInfo(keyDir, baseName);
        if(figFileInfo.exists())
            return baseName;
    }
    return QString();
}

// include qdir
void dkFormat::findFigFiles()
{
    figFiles.clear();

    QDir inDir(filePath);
    QFileInfoList localFileInfoList = inDir.entryInfoList(QStringList("fig-*"), QDir::Files, QDir::Name );
    for (int i = 0; i < localFileInfoList.size(); ++i) {
        QFileInfo localFileInfo = localFileInfoList.at(i);
        figFiles.push_back(localFileInfo.fileName());
    }
}

//// old version. too slow
//QString dkFormat::linkGlossary(dkTermList & termList, QString &htmlTxt)
//{
//    QStringList htmlList = htmlTxt.split('\n');

//    QList< dkStringInt > keyList = termList.sortBySize();
//    for(int i = 0; i < keyList.size(); ++i)
//    {
//        dkStringInt theKey = keyList[i];
//        QString keyString = theKey.getString();
//        int keyInt = theKey.getInt();
//        for(int j = 0; j < htmlList.size(); ++j)
//        {
//            linkGlossaryKey(keyString, keyInt, htmlList[j]);
//        }
//    }

//    QString outTxt = htmlList.join('\n');
//    return outTxt;
//}

// moved to dkTermList
//QString dkFormat::linkGlossary(QString &inHtmlTxt)
//{
//    QStringList inHtmlList = inHtmlTxt.split('\n');

//    QList< dkStringInt > keyList = glossary->sortBySize();

//    for(int i = 0; i < inHtmlList.size(); ++i)
//    {
//        QString htmlTxt = inHtmlList[i];
//        QString plainTxt = QTextDocumentFragment::fromHtml( htmlTxt ).toPlainText();
//        QStringList plainList = plainTxt.split(QRegExp("\\W+")); // only words no separators
//        QStringList htmlList = htmlTxt.split(QRegExp("\\b")); // words and separators

//        for(int j = 0; j < keyList.size(); ++j)
//        {
//            dkStringInt theKey = keyList[j];
//            QString keyString = theKey.getString();
//            int keyInt = theKey.getInt();


//            QStringList kList = keyString.split(QRegExp("\\W+")); // only words no separators
//            if(kList.size() == 0)
//                continue;

//            if(isKeyPresent(kList, plainList))
//                linkKey(kList, keyInt, htmlList);
//        }

//        inHtmlList[i] = htmlList.join("");
//    }

//    QString outTxt = inHtmlList.join('\n');
//    return outTxt;
//}

// moved to dkTermList
//QString dkFormat::linkEndpoints(QString &inHtmlTxt)
//{
//    QStringList inHtmlList = inHtmlTxt.split('\n');

//    QList< dkStringInt > keyList = endpoints->sortBySize();

//    for(int i = 0; i < inHtmlList.size(); ++i)
//    {
//        QString htmlTxt = inHtmlList[i];
//        QString plainTxt = QTextDocumentFragment::fromHtml( htmlTxt ).toPlainText();
//        QStringList plainList = plainTxt.split(QRegExp("\\W+")); // only words no separators
//        QStringList htmlList = htmlTxt.split(QRegExp("\\b")); // words and separators

//        for(int j = 0; j < keyList.size(); ++j)
//        {
//            dkStringInt theKey = keyList[j];
//            QString keyString = theKey.getString();
//            int keyInt = theKey.getInt();


//            QStringList kList = keyString.split(QRegExp("\\W+")); // only words no separators
//            if(kList.size() == 0)
//                continue;

//            if(isKeyPresent(kList, plainList))
//                linkKey(kList, keyInt, htmlList);
//        }

//        inHtmlList[i] = htmlList.join("");
//    }

//    QString outTxt = inHtmlList.join('\n');
//    return outTxt;
//}

// moved to dkTermList
//// inList is plain text list of words only
//bool dkFormat::isKeyPresent(QStringList &keyList, QStringList &inList)
//{
//    if(inList.size() < keyList.size())
//        return false; // inList to short

//    for(int j = 0; j < keyList.size(); ++j)
//    {
//        if(!inList.contains(keyList[j], Qt::CaseInsensitive))
//            return false;
//    }

//    for(int i = 0; i < inList.size(); ++i)
//    {
//        if(inList.size() < i + keyList.size())
//            return false; // inList to short
//        for(int j = 0; j < keyList.size(); ++j)
//        {
//            if(inList[i+j] != keyList[j])
//                break;
//        }
//        return true;
//    }
//    return false;
//}

//bool dkFormat::isKey(QStringList &keyList, QStringList &inList, int position)
//{
//    if(inList.size() < position + keyList.size())
//        return false; // inList to short
//    for(int i = 0; i < keyList.size(); ++i)
//    {
//        if(inList[position+i] != keyList[i])
//            return false;
//    }
//    return true;
//}

// moved to dkTermList
//void dkFormat::linkKey(QStringList &keyList, int keyIndex, QStringList &inList)
//{
//    if(inList.size() < keyList.size())
//        return; // inList to short

//    bool isCode = false;
//    bool isLink = false;
//    int keyPosition = 0; // index of key in keywords consisting from more thoan one word
//    int startIndex = 0; // index of inList at which key starts
//    for(int i = 0; i < inList.size(); ++i)
//    {
////        QString theTxt = inList[i]; // for debuging
//        if(isCode) // inside html code, no plain text
//        {
//            if(inList[i].contains('>'))
//            {
//                if(i > 0)
//                    if(inList[i-1] == "a")
//                        isLink = false;

//                if(inList[i].contains('<')) // there can be both > and <
//                {
//                    if(i+1 < inList.size())
//                        if(inList[i+1] == "a")
//                            isLink = true;
//                }
//                else
//                    isCode = false;

//                continue;
//            }
//            continue;
//        }

//        if(inList[i].contains('<') && !inList[i].contains('>')) // html code started
//        {
//            isCode = true;
//            if(i+1 < inList.size())
//                if(inList[i+1] == "a")
//                    isLink = true;
//            continue;
//        }

//        if(isLink)
//            continue;

//        if(!(inList[i][0].isLetter() || inList[i][0].isDigit())) // is first character not letter or digit
//            continue; // separator

//        if(i > 0)
//            if(inList[i-1].contains('&')) // is this escape sequence?
//                continue;

//        if(inList[i].toLower() == keyList[keyPosition]) // comparison is case insensitive
//        {
//            if(keyPosition == 0)
//                startIndex = i; // keyword started

//            if(keyList.size() == keyPosition + 1)
//            {
//                // key was found
//                QString prefix = QStringLiteral("<a href=\"#g%1\">").arg(keyIndex);
//                inList[startIndex].prepend(prefix);
//                inList[i].append("</a>");

//                keyPosition = 0;
//                startIndex = 0; //reset key counter
//            }
//            else
//                ++keyPosition;
//        }
//        else
//            keyPosition = 0;

//    }
//    return;
//}

QString dkFormat::addLinks(QString &inHtmlTxt)
{
    QString outTxt;

    outTxt = glossary->addLinks(inHtmlTxt);
    outTxt = endpoints->addLinks(outTxt);
    return outTxt;
}

// add links to images in inHtmlTxt
// images are preceded by Fig. and definition consist of only on word
void dkFormat::addFigLinks(QString &inHtmlTxt)
{
    QStringList inHtmlList = inHtmlTxt.split('\n');
    for(int i = 0; i < inHtmlList.size(); ++i)
    {
        QString htmlTxt = inHtmlList[i];
        if(!htmlTxt.contains("Fig.", Qt::CaseInsensitive))
            continue;
        QStringList htmlList = htmlTxt.split("Fig.", Qt::KeepEmptyParts, Qt::CaseInsensitive);
        if(htmlList.size() < 2)
            continue;
        for(int j = 1; j < htmlList.size(); ++j)
        {
            if(htmlList[j-1].endsWith('(')) //there is bracket before Fig.
            {
                QString theHtml = htmlList[j];
                QStringList bracketList = theHtml.split(')');
                QStringList wordList = bracketList[0].split(QRegularExpression("\\b")); // words and separators
                for(int k = 1; k < wordList.size(); k+=2)
                {
                    int keyIndex = figures->contains(wordList[k]);
                    if(keyIndex > -1) // key was found
                    {
                        QString prefix = QStringLiteral("<a href=\"#f%1\">").arg(keyIndex);
                        wordList[k].prepend(prefix);
                        wordList[k].append("</a>");
                    }
                }
                bracketList[0] = wordList.join("");
                htmlList[j] = bracketList.join(')');
            } else // verify only one word
            {
                QString theHtml = htmlList[j];
                QStringList wordList = theHtml.split(QRegularExpression("\\b")); // words and separators
                int keyIndex = figures->contains(wordList[1]);
                if(keyIndex > -1) // key was found
                {
                    QString prefix = QStringLiteral("<a href=\"#f%1\">").arg(keyIndex);
                    wordList[1].prepend(prefix);
                    wordList[1].append("</a>");
                }
                htmlList[j] = wordList.join("");
            }
        }
        inHtmlList[i] = htmlList.join("Fig.");
    }

    inHtmlTxt = inHtmlList.join('\n');
//    return outTxt;
}


