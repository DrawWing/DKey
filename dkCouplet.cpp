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

#include <QDir>

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
    //    lead1 = "Lead 1.";
    //    endpoint1 = "Endpoint 1.";
    //    lead2 = "Lead 2.";
    //    endpoint2 = "Endpoint 2.";

    lead1 = QObject::tr("Lead 1.");
    endpoint1 = QObject::tr("Endpoint 1.");
    lead2 = QObject::tr("Lead 2.");
    endpoint2 = QObject::tr("Endpoint 2.");
}

dkCouplet::dkCouplet(const QStringList &inTxt)
{
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
    dkString endDigits = end.endDigits();
    if(endDigits.size() == 0){
        pointer1 = -1;
        endpoint1 = end.trimmed();
    }else{
        int endNumber = endDigits.toInt(&ok);
        pointer1 = endNumber;
        endpoint1 = "";
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

    start = line.findFrontPart();
    end = line.findEndPart();
    endDigits = end.endDigits();
    if(endDigits.size() == 0){
        pointer2 = -1;
        endpoint2 = end.trimmed();
    }else{
        int endNumber = endDigits.toInt(&ok);
        pointer2 = endNumber;
        endpoint2 = "";
    }
    txt = line;
    txt.chop(end.size());
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
    if( number == -1)
        return false;
    else
        return true;
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
    if(endDigits.size() == 0){
        pointer1 = -1;
        endpoint1 = end.trimmed();
    }else{
        int endNumber = endDigits.toInt(&ok);
        pointer1 = endNumber;
        endpoint1 = "";
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
    endDigits = end.frontDigits();
    if(endDigits.size() == 0){
        pointer2 = -1;
        endpoint2 = end.trimmed();
    }else{
        int endNumber = endDigits.toInt(&ok);
        pointer2 = endNumber;
        endpoint2 = "";
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
    dkString endDigits = end.frontDigits();
    if(endDigits.size() == 0){
        if(first)
        {
            endpoint1 = end.trimmed();
            pointer1 = -1;
        }
        else
        {
            endpoint2 = end.trimmed();
            pointer2 = -1;
        }
    }else{
        int endNumber = endDigits.toInt();
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

QString dkCouplet::getTxt() const
{
    QString outTxt;
    if(endpoint1.isEmpty())
        outTxt = QString ("%1\t%2").arg(lead1).arg(pointer1);
    else
        outTxt = QString ("%1\t%2").arg(lead1).arg(endpoint1);

    outTxt += "\n";

    if(endpoint2.isEmpty())
        outTxt += QString ("%1\t%2").arg(lead2).arg(pointer2);
    else
        outTxt += QString ("%1\t%2").arg(lead2).arg(endpoint2);

    return outTxt;
}

QString dkCouplet::getLead1txt() const
{
    QString outTxt;
    if(endpoint1.isEmpty())
        outTxt = QString ("%1\t%2").arg(lead1).arg(pointer1);
    else
        outTxt = QString ("%1\t%2").arg(lead1).arg(endpoint1);
    return outTxt;
}

QString dkCouplet::getLead2txt() const
{
    QString outTxt;
    if(endpoint2.isEmpty())
        outTxt = QString ("%1\t%2").arg(lead2).arg(pointer2);
    else
        outTxt = QString ("%1\t%2").arg(lead2).arg(endpoint2);
    return outTxt;
}

QString dkCouplet::getLead1html(QString path) const
{
    QString outTxt;
    if(endpoint1.isEmpty())
        outTxt = QString ("<a href=\"lead1\">%1</a>").arg(lead1);
    else
        outTxt = QString ("%1<br><b>%2</b>").arg(lead1).arg(endpoint1);

    if(figList1.size() > 0)
        outTxt += QString("<br>");
    for(int j = 0; j < figList1.size(); ++j)
        outTxt += QString("<img border=\"1\" src=\"%1\">").arg(path + figList1[j]);

    return outTxt;
}

QString dkCouplet::getLead2html(QString path) const
{
    QString outTxt;
    if(endpoint2.isEmpty())
        outTxt = QString ("<a href=\"lead2\">%1</a>").arg(lead2);
    else
        outTxt = QString ("%1<br><b>%2</b>").arg(lead2).arg(endpoint2);

    if(figList2.size() > 0)
        outTxt += QString("<br>");
    for(int j = 0; j < figList2.size(); ++j)
        outTxt += QString("<img border=\"1\" src=\"%1\">").arg(path + figList2[j]);

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

// use of this function shoulb be careful because maxNumber can change
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

QString dkCouplet::getRtf() const
{
    QString outTxt;

    if(endpoint1.isEmpty())
        outTxt += QString("%1\\tab %2\\tab %3\\par\n").arg(number).arg(lead1.getRtf()).arg(pointer1);
    else
        outTxt += QString("%1\\tab %2\\tab %3\\par\n").arg(number).arg(lead1.getRtf()).arg(endpoint1.getRtf());

    if(endpoint2.isEmpty())
        outTxt += QString("-\\tab %1\\tab %2\\par\n").arg(lead2.getRtf()).arg(pointer2);
    else
        outTxt += QString("-\\tab %1\\tab %2\\par\n").arg(lead2.getRtf()).arg(endpoint2.getRtf());

    return outTxt;
}

QString dkCouplet::getHtml() const
{
    QString htmlTxt;

    if(endpoint1.isEmpty())
        htmlTxt += QStringLiteral("<b id=\"k%1\">%1</b>. %2 <a href=\"#k%3\">%3</a><br>").arg(number).arg(lead1).arg(pointer1);
    else
        htmlTxt += QStringLiteral("<b id=\"k%1\">%1</b>. %2 <b>%3</b><br>").arg(number).arg(lead1).arg(endpoint1);

    if(endpoint2.isEmpty())
        htmlTxt += QStringLiteral("<b>-</b> %1 <a href=\"#k%2\">%2</a><br>").arg(lead2).arg(pointer2);
    else
        htmlTxt += QStringLiteral("<b>-</b> %1 <b>%2</b><br>").arg(lead2).arg(endpoint2);

    return htmlTxt;
}

QString dkCouplet::getHtmlTab() const
{
    QString htmlTxt;

    htmlTxt += "<tr>";
    htmlTxt += "<td align=\"left\" valign=\"top\" width=\"3%\">";
    htmlTxt += QString("<div id=\"k%1\">%1</div>").arg(number);
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

QString dkCouplet::getHtmlImg(QString path) const
{
    QString htmlTxt;

    htmlTxt = QStringLiteral("<br><b id=\"k%1\">%1</b>.<br>").arg(number);
    htmlTxt += "<table border=\"1\" cellpadding=\"10\" cellspacing=\"0\" style=\"border-collapse: collapse\" bordercolor=\"#111111\" width=\"100%\">\n";
    htmlTxt += "<tr>";
    htmlTxt += "<td align=\"left\" valign=\"top\" width=\"50%\">";
    if(endpoint1.isEmpty())
        htmlTxt += QStringLiteral("%1<br><a href=\"#k%2\">%2</a><br>").arg(lead1).arg(pointer1);
    else
        htmlTxt += QStringLiteral("%1<br><b>%2</b><br>").arg(lead1).arg(endpoint1);
    for(int j = 0; j < figList1.size(); ++j)
        htmlTxt += QStringLiteral("<img border=\"1\" src=\"%1\">").arg(path + figList1[j]);
    htmlTxt += "</td>";

    htmlTxt += "<td align=\"left\" valign=\"top\" width=\"50%\">";
    if(endpoint2.isEmpty())
        htmlTxt += QStringLiteral("%1<br><a href=\"#k%2\">%2</a><br>").arg(lead2).arg(pointer2);
    else
        htmlTxt += QStringLiteral("%1<br><b>%2</b><br>").arg(lead2).arg(endpoint2);
    for(int j = 0; j < figList2.size(); ++j)
        htmlTxt += QStringLiteral("<img border=\"1\" src=\"%1\">").arg(path + figList2[j]);
    htmlTxt += "</td>";
    htmlTxt += "</tr>\n";
    htmlTxt += "</table>\n";

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

void dkCouplet::findFigs(QString & path)
{
    figList1 = findFigs(lead1, path);
    figList2 = findFigs(lead2, path);
}

QStringList dkCouplet::findFigs(QString & inTxt, QString & path) const
{
    QStringList outList;

    std::vector< int > figIndex;
    QString figPrefix = "fig.";

    QDir keyDir(path);

    QStringList strList = inTxt.split(figPrefix, QString::SkipEmptyParts, Qt::CaseInsensitive);
    for(int i = 1; i < strList.size(); ++i) // start with 1; 0 is first part
    {
        QString theString = strList.at(i);
        theString = theString.simplified(); // or trimmed
        // split the string into an alternating sequence of non-word and word tokens
        QStringList subStrList = theString.split(QRegExp("\\b"));

        for(int j = 1; j < subStrList.size(); ++j) // start with 1; 0 is empty string
        {
            QString subString = subStrList.at(j);
            subString = subString.simplified();
            subString.prepend("fig-");
            subString.append(".png");
            QFileInfo figFileInfo(keyDir, subString);
            if(figFileInfo.exists())
                outList.push_back(subString);
            else
                break;
            ++j; //sip separator
        }
    }

    return outList;
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

