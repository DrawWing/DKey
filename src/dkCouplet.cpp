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
#include <QTextDocument>

#include "dkCouplet.h"
#include "dkString.h"

dkCouplet::dkCouplet()
{
    clear();
}

// dummy couple with number controlled by copuletList
dkCouplet::dkCouplet(const int inNumber)
{
    clear();
    number = inNumber;
    lead1 = QObject::tr("Lead 1.");
    endpoint1 = QObject::tr("Endpoint 1.");
    lead2 = QObject::tr("Lead 2.");
    endpoint2 = QObject::tr("Endpoint 2.");
}

dkCouplet::dkCouplet(const QStringList &inTxt)
{
    clear();
    if(inTxt.size() == 0)
        return;

    //first line
    dkString line = inTxt[0];
    dkString start = line.findFrontPart();
    int startSize = start.size();
    start.removeFrontNonLetterAndDigit();
    dkString startDigits = start.frontDigits();

    bool ok;
    int startNumber = startDigits.toInt(&ok);
    if(ok)
        number = startNumber;
    else
        number = -1;

    dkString end = line.findEndPart();
    int endSize = end.size(); // for choping
    end.removeFrontNonLetterAndDigit();
    end.removeEndNonLetterAndDigit();

    int endNumber = end.toInt(&ok);
    if(ok)
    {
        pointer1 = endNumber;
        endpoint1 = "";
    }else{
        pointer1 = -1;
        endpoint1 = end;
    }

    dkString txt = line;
    txt.chop(endSize);
    txt.chopFront(startSize);
    lead1 = txt.trimmed();

    //lead2
    if(inTxt.size() == 1)
    {
        lead2 = "";
        pointer2 = -1;
        endpoint2 = "";
        return;
    }
    else if(inTxt.size() == 2)
    {
        line = inTxt[1].trimmed();
    }
    else
    {
        line.clear();
        for(int i = 1; i < inTxt.size(); ++i)
        {
            line.append(inTxt[i].trimmed()); //remove line feed and cr
            line.append(" ");
        }
        line = line.trimmed();
    }

    if(line.isEmpty())
        return;
    start = line.findFrontPart();
    end = line.findEndPart();
    endSize = end.size(); // for choping
    end.removeFrontNonLetterAndDigit();
    end.removeEndNonLetterAndDigit();

    endNumber = end.toInt(&ok);
    if(ok)
    {
        pointer2 = endNumber;
        endpoint2 = "";
    }else{
        pointer2 = -1;
        endpoint2 = end;
    }

    txt = line;
    txt.chop(endSize);
    txt.chopFront(start.size());
    lead2 = txt.trimmed();
}

void dkCouplet::clear()
{
    number = -1;
    from.clear();
    pointerChain.clear();
    leadChain.clear();
    lead1 = "";
    pointer1 = -1;
    endpoint1 = "";
    figList1.clear();
    lead2 = "";
    pointer2 = -1;
    endpoint2 = "";
    figList2.clear();
    error.clear();
}

bool dkCouplet::isEmpty() const
{
    if( number < 1)
        return true;
    else
        return false;
}

void dkCouplet::fromDkTxt(const QStringList &inList)
{
    clear();

    if(inList.size() != 2)
    {
        error += QObject::tr("Number of lines other than 2. \n");
        return;
    }

    QString line1 = inList[0];
    readDkTxtLine(line1, true);
    QString line2 = inList[1];
    readDkTxtLine(line2, false);
}

void dkCouplet::readDkTxtLine(const QString &inTxt, bool first)
{

    QStringList inList = inTxt.split("\t");
    if(inList.size() != 3)
    {
        error += QObject::tr("Number of tab characterst other than 2. \n");
        return;
    }

    QString start = inList[0];
    bool ok;
    if(first)
    {
        int startNumber = start.toInt(&ok);
        if(ok)
            number = startNumber;
        else
        {
            error += QObject::tr("Couplet does not start with number. \n");
            return;
        }
    }
    else
    {
        if(start != "-")
        {
            error += QObject::tr("Second line does not start with -. \n");
            return;
        }
    }

    QString lead = inList[1];
    lead = lead.trimmed();
    if(first)
        lead1 = lead;
    else
        lead2 = lead;

    QString end = inList[inList.size()-1];
    int endNumber = end.toInt(&ok);
    if(ok)
        if(first)
            pointer1 = endNumber;
        else
            pointer2 = endNumber;
    else
    {
        if(first)
            endpoint1 = end;
        else
            endpoint2 = end;
    }
}

void dkCouplet::fromDkXml(const QDomElement &inElement)
{
    clear();

    QString numberTxt = inElement.attribute("number");
    bool ok;
    number = numberTxt.toInt(&ok);
    if(!ok)
    {
        error += QObject::tr("Error in reading couplet number. \n");
        return;
    }

    QDomNode leadNode = inElement.firstChild();
    QDomElement leadElem = leadNode.toElement();
    if(leadElem.isNull() || leadElem.tagName() != "lead")
    {
        error += QObject::tr("Error in reading first lead of couplet: %1. \n").arg(number);
        return;
    }
    readXmlLead(leadElem, true);

    leadNode = leadNode.nextSibling();
    leadElem = leadNode.toElement();
    if(leadElem.isNull() || leadElem.tagName() != "lead")
    {
        error += QObject::tr("Error in reading second lead of couplet: %1. \n").arg(number);
        return;
    }
    readXmlLead(leadElem, false);
}

void dkCouplet::readXmlLead(const QDomElement &inElement, bool first, int ver)
{
    QDomNodeList inElemList = inElement.childNodes();
    for(int i = 0; i < inElemList.size(); ++i){
        QDomNode aNode = inElemList.at(i);
        QDomElement anElement = aNode.toElement();
        if(anElement.isNull())
            continue;
        if(anElement.tagName() == "text")
        {
            dkString inTxt = anElement.text();
            if(ver > 2)
                inTxt = inTxt.md2html();
            if(first)
                lead1 = inTxt;
            else
                lead2 = inTxt;
        }
        else if(anElement.tagName() == "pointer")
        {
            QString pointerTxt = anElement.text();
            bool ok;
            int pointer = pointerTxt.toInt(&ok);
            if(ok)
            {
                if(first)
                {
                    pointer1 = pointer;
                    endpoint1 = "";
                }
                else
                {
                    pointer2 = pointer;
                    endpoint2 = "";
                }
            } else
                error += QObject::tr("Pointer cannot be converted to number in couplet: %1. \n").arg(number);
        }
        else if(anElement.tagName() == "endpoint")
        {
            dkString inTxt = anElement.text();
            if(ver > 2)
                inTxt = inTxt.md2html();
            if(first)
            {
                endpoint1 = inTxt;
                pointer1 = -1;
            }
            else
            {
                endpoint2 = inTxt;
                pointer2 = -1;
            }
        }
    }
}

void dkCouplet::import1number(const QStringList &inTxt)
{
    clear();

    if(inTxt.size() == 0)
        return;

    //first line
    dkString line = inTxt[0];
    dkString start = line.findFrontPart();
    dkString startDigits = start.frontDigits();

    bool ok;
    int startNumber = startDigits.toInt(&ok);
    if(ok)
        number = startNumber;
    else
        number = -1;

    dkString end = line.findEndPart();
    dkString endDigits = end.frontDigits();

    int endNumber = end.toInt(&ok);
    if(ok)
    {
        pointer1 = endNumber;
        endpoint1 = "";
    }else{
        pointer1 = -1;
        endpoint1 = end;
    }

    dkString txt = line;
    txt.chop(end.size());
    txt.chopFront(start.size());
    lead1 = txt.trimmed();

    //lead2
    if(inTxt.size() == 1)
    {
        lead2 = "";
        pointer2 = -1;
        endpoint2 = "";
        return;
    } else if(inTxt.size() == 2)
    {
        line = inTxt[1].trimmed();
    } else
    {
        line.clear();
        for(int i = 1; i < inTxt.size(); ++i)
        {
            line.append(inTxt[i].trimmed()); //remove line feed and cr
            line.append(" ");
        }
        line = line.trimmed();
    }

    start = line.findFrontPart();
    end = line.findEndPart();

    endNumber = end.toInt(&ok);
    if(ok)
    {
        pointer2 = endNumber;
        endpoint2 = "";
    }else{
        pointer2 = -1;
        endpoint2 = end;
    }

    txt = line;
    txt.chop(end.size());
    txt.chopFront(start.size());
    lead2 = txt.trimmed();
}

// import couplet with number at the begining of both leads
void dkCouplet::importTxt2( QStringList &inTxt)
{
    clear();

    if(inTxt.size() == 0)
        return;
    else if(inTxt.size() == 1)
        importTxtLine2(inTxt[0], true);
    else if(inTxt.size() == 2)
    {
        importTxtLine2(inTxt[0], true);
        importTxtLine2(inTxt[1], false);
    }
    else
    {
        int lineNo = 1;
        for(int i = 1; i < inTxt.size(); ++i)
        {
            dkString txt;
            txt = inTxt[i];
            if(txt.startsWithDigit())
            {
                lineNo = i;
                break;
            }
        }

        QString line1;
        for(int i = 0; i < lineNo; ++i)
        {
            line1.append(inTxt[i]);
            line1.append(" ");
        }
        importTxtLine2(line1, true);
        QString line2;
        for(int i = lineNo; i < inTxt.size(); ++i)
        {
            line2.append(inTxt[i]);
            line2.append(" ");
        }
        importTxtLine2(line2, false);
    }
}

void dkCouplet::importTxtLine2(QString & inTxt, bool first)
{
    dkString line = inTxt.trimmed();
    if(line.size() == 0)
        return;
    dkString start = line.findFrontPart();
    dkString startDigits = start.frontDigits();

    if(first)
    {
        bool ok;
        int startNumber = startDigits.toInt(&ok);
        if(ok)
            number = startNumber;
        else
            number = 0; // -1 is empty couplet
    }

    dkString end = line.findEndPart();

    bool ok;
    int endNumber = end.toInt(&ok);
    if(ok)
    {
        if(first)
        {
            pointer1 = endNumber;
            endpoint1 = "";
        }
        else
        {
            pointer2 = endNumber;
            endpoint2 = "";
        }
    }else{
        if(first)
        {
            pointer1 = -1;
            endpoint1 = end;
        }
        else
        {
            pointer2 = -1;
            endpoint2 = end;
        }
    }

    dkString txt = line;
    txt.chop(end.size());
    txt.chopFront(start.size());
    txt = removeAB(txt, first);
    if(first)
        lead1 = txt.trimmed();
    else
        lead2 = txt.trimmed();
}

void dkCouplet::importIndented(const QString &inTxt, int firstNumber)
{
    dkString line = inTxt;

    // find parts before line is modified
    dkString start = line.findFrontPart();

    number = firstNumber;

    if(line.contains("\t"))
    {
        dkString end = line.findEndPart();
        endpoint1 = end.trimmed();
        pointer1 = -1;
        line.chop(end.size());
        line.chopFront(start.size());
        lead1 = line.trimmed();
    }
    else
    {
        pointer1 = firstNumber + 1;
        endpoint1 = "";
        line.chopFront(start.size());
        lead1 = line.trimmed();
    }
}

// import sencond lead of indented key
void dkCouplet::appendIndented(const QString &inTxt, int lineNumber)
{
    dkString line = inTxt;

    dkString start = line.findFrontPart();

    if(line.contains("\t"))
    {
        dkString end = line.findEndPart();
        endpoint2 = end.trimmed();
        pointer2 = -1;
        line.chop(end.size());
        line.chopFront(start.size());
        lead2 = line.trimmed();
    }
    else
    {
        pointer2 = lineNumber + 1;
        endpoint2 = "";
        line.chopFront(start.size());
        lead2 = line.trimmed();
    }
}

QList<int> dkCouplet::getFrom() const
{
    return from;
}

int dkCouplet::getNumber() const
{
    return number;
}

QList<int> dkCouplet::getPointerChain() const
{
    return pointerChain;
}

QList<QString> dkCouplet::getLeadChain() const
{
    return leadChain;
}

QString dkCouplet::getLead1() const
{
    return lead1;
}

QString dkCouplet::getLead2() const
{
    return lead2;
}

// get text only for QTableWidget
QString dkCouplet::getLead1txt() const
{
    QString outTxt;
    if(pointer1 > 0)
        outTxt = QString ("%1 - %2").arg(lead1.toPlainText()).arg(pointer1);
    else
        outTxt = QString ("%1 - %2").arg(lead1.toPlainText()).arg(endpoint1.toPlainText());
    return outTxt;
}

// get text only for QTableWidget
QString dkCouplet::getLead2txt() const
{
    QString outTxt;
    if(pointer2 > 0)
        outTxt = QString ("%1 - %2").arg(lead2.toPlainText()).arg(pointer2);
    else
        outTxt = QString ("%1 - %2").arg(lead2.toPlainText()).arg(endpoint2.toPlainText());
    return outTxt;
}

QString dkCouplet::getLead1html(QString path) const
{
    QString outTxt;
    if(endpoint1.isEmpty())
        outTxt = QString ("%1. %2<br><a href=\"lead1\">Go to couplet: %3</a>")
                .arg(number).arg(lead1).arg(pointer1);
    else
        outTxt = QString ("%1. %2<br><b>%3</b>").arg(number).arg(lead1).arg(endpoint1);

    if(figList1.size() > 0)
        outTxt += dkString::htmlBr;
    for(int j = 0; j < figList1.size(); ++j)
        outTxt += imgHtml(path, figList1[j]);

    return outTxt;
}

QString dkCouplet::getLead2html(QString path) const
{
    QString outTxt;
    if(endpoint2.isEmpty())
        outTxt = QString ("%1. %2<br><a href=\"lead2\">Go to couplet: %3</a>")
                .arg(number).arg(lead2).arg(pointer2);
    else
        outTxt = QString ("%1. %2<br><b>%3</b>").arg(number).arg(lead2).arg(endpoint2);

    if(figList2.size() > 0)
        outTxt += dkString::htmlBr;
    for(int j = 0; j < figList2.size(); ++j)
        outTxt += imgHtml(path, figList2[j]);

    return outTxt;
}

int dkCouplet::getPointer1() const
{
    return pointer1;
}

int dkCouplet::getPointer2() const
{
    return pointer2;
}

QString dkCouplet::getEndpoint1() const
{
    return endpoint1;
}

QString dkCouplet::getEndpoint2() const
{
    return endpoint2;
}

QString dkCouplet::getError() const
{
    return error;
}

void dkCouplet::setFrom(QList<int> & inList)
{
    from = inList;
}

// use of this function should be careful because maxNumber can change
void dkCouplet::setNumber(int inVal)
{
    number = inVal;
}

QString dkCouplet::getDkTxt() const
{
    QString outTxt;

    if(endpoint1.isEmpty())
        outTxt += QStringLiteral("%1\t%2\t%3\n").arg(number).arg(lead1).arg(pointer1);
    else
        outTxt += QStringLiteral("%1\t%2\t%3\n").arg(number).arg(lead1).arg(endpoint1);

    if(endpoint2.isEmpty())
        outTxt += QStringLiteral("-\t%1\t%2\n").arg(lead2).arg(pointer2);
    else
        outTxt += QStringLiteral("-\t%1\t%2\n").arg(lead2).arg(endpoint2);

    return outTxt;
}

QString dkCouplet::getDkXml() const
{
    // QTextDocument doc;
    // doc.setHtml(lead1);
    // QString mdLead1 = doc.toMarkdown();
    // doc.setHtml(lead2);
    // QString mdLead2 = doc.toMarkdown();
    // doc.setHtml(endpoint1);
    // QString mdEndpoint1 = doc.toMarkdown();
    // doc.setHtml(endpoint2);
    // QString mdEndpoint2 = doc.toMarkdown();

    QString outTxt = QStringLiteral("<couplet number=\"%1\">\n").arg(number);

    if(endpoint1.isEmpty())
        outTxt += QStringLiteral("<lead> <text>%1</text> <pointer>%2</pointer> </lead>\n").arg(lead1.html2md())
                      .arg(pointer1);
    else
        outTxt += QStringLiteral("<lead> <text>%1</text> <endpoint>%2</endpoint> </lead>\n").arg(lead1.html2md())
                      .arg(endpoint1.html2md());

    if(endpoint2.isEmpty())
        outTxt += QStringLiteral("<lead> <text>%1</text> <pointer>%2</pointer> </lead>\n").arg(lead2.html2md())
                      .arg(pointer2);
    else
        outTxt += QStringLiteral("<lead> <text>%1</text> <endpoint>%2</endpoint> </lead>\n").arg(lead2.html2md())
                      .arg(endpoint2.html2md());

    outTxt += "</couplet>\n";
    return outTxt;
}

QString dkCouplet::getRtf() const
{
    // couplet number
    QString outTxt = QString::number(number);
    // number of previous couplet
    if(from.size() > 0)
    {
        outTxt += QString("(%1").arg(from.at(0));
        for(int i = 1; i < from.size(); ++i)
            outTxt += QStringLiteral(", %1").arg(from.at(i));
        outTxt += ")";
    }
//    // if the couplet label is long add space otherwise add tabulator
//    if(outTxt.size() > 5)
//        outTxt += QString(" ");
//    else
        outTxt += QString("\\tab ");

    if(endpoint1.isEmpty())
        outTxt += QString("%1\\tab %2\\par\n").arg(lead1.getRtf()).arg(pointer1);
    else
        outTxt += QString("%1\\tab %2\\par\n").arg(lead1.getRtf()).arg(endpoint1.getRtf());

    if(endpoint2.isEmpty())
        outTxt += QString("-\\tab %1\\tab %2\\par\n").arg(lead2.getRtf()).arg(pointer2);
    else
        outTxt += QString("-\\tab %1\\tab %2\\par\n").arg(lead2.getRtf()).arg(endpoint2.getRtf());

    return outTxt;
}

QString dkCouplet::getTxt() const
{
    // couplet number
    QString outTxt = QString::number(number);
    // number of previous couplet
    if(from.size() > 0)
    {
        outTxt += QString("(%1").arg(from.at(0));
        for(int i = 1; i < from.size(); ++i)
            outTxt += QStringLiteral(", %1").arg(from.at(i));
        outTxt += ")";
    }

    dkString simpleTxt = lead1;
    simpleTxt.replace(dkString::htmlBr, " ");
    simpleTxt.removeHtml();
    outTxt += QString("\t%1\t").arg(simpleTxt);

    if(endpoint1.isEmpty())
        outTxt += QString("%1\n").arg(pointer1);
    else
    {
        dkString simpleTxt = endpoint1;
        simpleTxt.replace(dkString::htmlBr, " ");
        simpleTxt.removeHtml();
        outTxt += QString("%1\n").arg(simpleTxt);
    }

    simpleTxt = lead2;
    simpleTxt.replace(dkString::htmlBr, " ");
    simpleTxt.removeHtml();
    outTxt += QString("-\t%1\t").arg(simpleTxt);

    if(endpoint2.isEmpty())
        outTxt += QString("%1\n").arg(pointer2);
    else
    {
        dkString simpleTxt = endpoint2;
        simpleTxt.replace(dkString::htmlBr, " ");
        simpleTxt.removeHtml();
        outTxt += QString("%1\n").arg(simpleTxt);
    }

//    if(endpoint2.isEmpty())
//        outTxt += QString("-\t%1\t%2\n").arg(lead2).arg(pointer2);
//    else
//    {
//        dkString simpleTxt = endpoint2;
//        simpleTxt.removeHtml();
//        outTxt += QString("-\t%1\t%2\n").arg(lead2).arg(simpleTxt);
//    }

    return outTxt;
}

// format number of previous couplet
QString dkCouplet::previousTxt() const
{
    QString fromTxt;
    if(from.size() == 0)
    {
        fromTxt = "";
    }
    else
    {
        fromTxt = QStringLiteral("(<a href=\"#k%1\">%1</a>").arg(from.at(0));
        for(int i = 1; i < from.size(); ++i)
            fromTxt += QStringLiteral(", <a href=\"#k%1\">%1</a>").arg(from.at(i));
        fromTxt += ")";
    }
    return fromTxt;
}

QString dkCouplet::getHtml() const
{
    QString htmlTxt;

    if(endpoint1.isEmpty())
        htmlTxt += QStringLiteral("<p id=\"k%1\" class=\"couplet1\">%1%2 %3 — <a href=\"#k%4\">%4</a></p>\n").arg(number).arg(previousTxt()).arg(lead1).arg(pointer1);
    else
        htmlTxt += QStringLiteral("<p id=\"k%1\" class=\"couplet1\">%1%2 %3 — %4</p>\n").arg(number).arg(previousTxt()).arg(lead1).arg(endpoint1);

    if(endpoint2.isEmpty())
        htmlTxt += QStringLiteral("<p class=\"couplet2\">- %1 — <a href=\"#k%2\">%2</a></p>\n").arg(lead2).arg(pointer2);
    else
        htmlTxt += QStringLiteral("<p class=\"couplet2\">- %1 — %2</p>\n").arg(lead2).arg(endpoint2);

    return htmlTxt;
}

QString dkCouplet::getHtmlTabulator() const
{
    QString htmlTxt;

    QString fromTxt;
    if(from.size() == 0)
    {
        fromTxt = "";
    }
    else
    {
        fromTxt = QStringLiteral("(%1").arg(from.at(0));
        for(int i = 1; i < from.size(); ++i)
            fromTxt += QStringLiteral(", %1").arg(from.at(i));
        fromTxt += ")";
    }

    if(endpoint1.isEmpty())
        htmlTxt += QStringLiteral("%1%2 \t%3 \t%4\n").arg(number).arg(fromTxt).arg(lead1).arg(pointer1);
    else
        htmlTxt += QStringLiteral("%1%2 \t%3 \t%4\n").arg(number).arg(fromTxt).arg(lead1).arg(endpoint1);

    if(endpoint2.isEmpty())
        htmlTxt += QStringLiteral("- \t%1 \t%2\n").arg(lead2).arg(pointer2);
    else
        htmlTxt += QStringLiteral("- \t%1 \t%2\n").arg(lead2).arg(endpoint2);

    return htmlTxt;
}

QString dkCouplet::getHtmlTab() const
{
    QString htmlTxt;

    htmlTxt += "<tr>";
    htmlTxt += "<td align=\"left\" valign=\"top\" width=\"3%\">";
    htmlTxt += QString("<div id=\"k%1\">%1%2</div>").arg(number).arg(previousTxt());
    htmlTxt += "</td>";

    htmlTxt += "<td align=\"left\" valign=\"top\" width=\"97%\">";
    if(endpoint1.isEmpty())
        htmlTxt += QString("%1 <div style=\"float:right\"><a href=\"#k%2\">%2</a></div>").arg(lead1).arg(pointer1);
    else
        htmlTxt += QString("%1 <div style=\"float:right\">%2</div>").arg(lead1).arg(endpoint1);
    htmlTxt += "</td>";
    htmlTxt += "</tr>\n";

    htmlTxt += "<tr>";
    htmlTxt += "<td align=\"left\" valign=\"top\" width=\"3%\">";
    htmlTxt += QString("-");
    htmlTxt += "</td>";

    htmlTxt += "<td align=\"left\" valign=\"top\" width=\"97%\">";
    if(endpoint2.isEmpty())
        htmlTxt += QString("%1 <div style=\"float:right\"><a href=\"#k%2\">%2</a></div>").arg(lead2).arg(pointer2);
    else
        htmlTxt += QString("%1 <div style=\"float:right\">%2</div>").arg(lead2).arg(endpoint2);
    htmlTxt += "</td>";
    htmlTxt += "</tr>\n";

    return htmlTxt;
}

QString dkCouplet::getHtmlImg(const QString & path, bool withPath) const
{
    QString htmlTxt;

    htmlTxt = QStringLiteral("<div id=\"k%1\">%1%2</div>").arg(number).arg(previousTxt());
    htmlTxt += "<table border=\"1\" cellpadding=\"10\" cellspacing=\"0\" style=\"border-collapse: collapse\" bordercolor=\"#111111\" width=\"100%\">\n";
    htmlTxt += "<tr>";
    htmlTxt += "<td align=\"left\" valign=\"top\" width=\"50%\">";
    if(endpoint1.isEmpty())
        htmlTxt += QStringLiteral("%1<br><a href=\"#k%2\">%2</a><br />").arg(lead1).arg(pointer1);
    else
        htmlTxt += QStringLiteral("%1<br><b>%2</b><br />").arg(lead1).arg(endpoint1);
    for(int j = 0; j < figList1.size(); ++j)
        htmlTxt += imgHtml(path, figList1[j], withPath);
    htmlTxt += "</td>";

    htmlTxt += "<td align=\"left\" valign=\"top\" width=\"50%\">";
    if(endpoint2.isEmpty())
        htmlTxt += QStringLiteral("%1<br><a href=\"#k%2\">%2</a><br />").arg(lead2).arg(pointer2);
    else
        htmlTxt += QStringLiteral("%1<br><b>%2</b><br />").arg(lead2).arg(endpoint2);
    for(int j = 0; j < figList2.size(); ++j)
        htmlTxt += imgHtml(path, figList2[j], withPath);
    htmlTxt += "</td>";
    htmlTxt += "</tr>";
    htmlTxt += "</table><br />\n";

    return htmlTxt;
}

QString dkCouplet::imgHtml(const QString & path, const QString & inName, bool withPath) const
{
    const int imgMaxSize = 400;

    QString htmlTxt;
    QString imgFileName = path + inName;
    QImageReader imgReader(imgFileName);
    int x = imgReader.size().width();
    int y = imgReader.size().height();

    QString outName;
    if(withPath)
        outName = imgFileName;
    else
        outName = inName;

    if( x < imgMaxSize && y < imgMaxSize)
        htmlTxt = QStringLiteral("<img border=\"1\" src=\"%1\">").arg(outName);
    else if( x > y)
        htmlTxt = QStringLiteral("<img border=\"1\" src=\"%1\" width=\"%2\">").arg(outName).arg(imgMaxSize);
    else
        htmlTxt = QStringLiteral("<img border=\"1\" src=\"%1\" height=\"%2\">").arg(outName).arg(imgMaxSize);

    return htmlTxt;
}

void dkCouplet::setPointerChain(QList<int> & inList)
{
    pointerChain = inList;
}

void dkCouplet::setLeadChain(QList<QString> & inList)
{
    leadChain = inList;
}

void dkCouplet::setLead1(QString inTxt)
{
    lead1 = inTxt;
}

void dkCouplet::setLead2(QString inTxt)
{
    lead2 = inTxt;
}

void dkCouplet::setPointer1(int inVal)
{
    pointer1 = inVal;
    endpoint1 = "";
}

void dkCouplet::setPointer2(int inVal)
{
    pointer2 = inVal;
    endpoint2 = "";
}

void dkCouplet::setEndpoint1(QString inTxt)
{
    endpoint1 = inTxt;
    pointer1 = -1;
}

void dkCouplet::setEndpoint2(QString inTxt)
{
    endpoint2 = inTxt;
    pointer2 = -1;
}

void dkCouplet::setFigList1(QStringList & inList)
{
    figList1 = inList;
}

void dkCouplet::setFigList2(QStringList & inList)
{
    figList2 = inList;
}

QString dkCouplet::removeAB(QString & inTxt, bool first) const
{
    dkString outTxt;
    outTxt = inTxt;
    outTxt.removeFrontNonLetter();

    if(outTxt.size() < 2)
        return inTxt;
    QChar c1 = outTxt.at(1);
    if(c1.isLetter())
        return inTxt;

    QChar c0 = outTxt.at(0);
    c0.toLower();

    if(first)
    {
        if(c0 == 'a')
            outTxt.remove(0,1);
        else
            return inTxt;
    }
    else
    {
        if(c0 == 'b')
            outTxt.remove(0,1);
        else
            return inTxt;
    }
    outTxt.removeFrontNonLetter();
    c0 = outTxt.at(0);
    if(c0.isUpper())
    {
        QString outString;
        outString = outTxt;
        return outString;
    }
    else
        return inTxt;
}

QStringList dkCouplet::findFigs(QString & path)
{
    error.clear();
    figList1 = findFigs(lead1, path);
    figList2 = findFigs(lead2, path);
    QStringList outList = figList1 + figList2;
    return outList;
}

QStringList dkCouplet::findFigs(QString & inTxt, QString & path)
{
    QStringList outList;

    QString figPrefix = "fig.";

    QDir keyDir(path);

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
                    error += QString("Image %1 from couplet number %2 was not found.\n").arg(subString).arg(number);
                } else
                {
                    if(!outList.contains(fileName))
                        outList.push_back(fileName);
                }

                ++j; //sip separator
            }
        } else // find only one imgage file
        {
            // split the string into an alternating sequence of non-word and word tokens
            QStringList subStrList = theString.split(QRegularExpression("\\b"));

            int j = 1; // start with 1; 0 is empty string
            for(; j < subStrList.size(); ++j)
            {
                QString subString = subStrList.at(j);
                if(subString == "fig" || subString == "Fig")
                    break;
            }
            ++j; // skip separator
            ++j;
            if(j < subStrList.size())
            {
                QString subString = subStrList.at(j);
                subString = subString.simplified();
                subString.prepend("fig-");

                QString fileName = figExist(keyDir, subString);
                if(fileName.isEmpty())
                {
                    error += QString("Image %1 from couplet number %2 was not found.\n").arg(subString).arg(number);
                } else
                {
                    if(!outList.contains(fileName))
                        outList.push_back(fileName);
                }
            }
        }
    }

    return outList;
}

QString dkCouplet::figExist(QDir & keyDir, QString & inTxt) const
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

void dkCouplet::swapLeads()
{
    QString leadTmp = lead1;
    int pointerTmp = pointer1;
    QString endpointTmp = endpoint1;
    QStringList figListTmp = figList1;

    lead1 = lead2;
    pointer1 = pointer2;
    endpoint1 = endpoint2;
    figList1 = figList2;

    lead2 = leadTmp;
    pointer2 = pointerTmp;
    endpoint2 = endpointTmp;
    figList2 = figListTmp;
}

bool dkCouplet::isContentOK(void)
{
    error.clear();
    if(lead1.isEmpty() || lead2.isEmpty())
        error += QString(QObject::tr("Couplet %1: Empty lead.\n"))
                .arg(number);
    if(pointer1 == -1 && endpoint1.isEmpty())
        error += QString(QObject::tr("Couplet %1: Empty first reference.\n"))
                .arg(number);
    if(pointer2 == -1 && endpoint2.isEmpty())
        error += QString(QObject::tr("Couplet %1: Empty second reference.\n"))
                .arg(number);
    if(pointer1 == pointer2 && pointer1 != -1 && pointer2 != -1)
        error += QString(QObject::tr("Couplet %1: The same pointer in both leads.\n"))
                .arg(number);
    if(endpoint1 == endpoint2 && !endpoint1.isEmpty() && !endpoint2.isEmpty())
        error += QString(QObject::tr("Couplet %1: The same endpoint in both leads.\n"))
                .arg(number);

    if(error.isEmpty())
        return true;
    else
        return false;
}
