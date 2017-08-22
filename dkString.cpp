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

#include "dkString.h"

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
//    QStringList stringList = split("\t",QString::SkipEmptyParts);
//    dkString end;
//    end = stringList.at(stringList.size()-1);
//    return end;
    QStringList stringList = split("\t",QString::SkipEmptyParts);
    if(stringList.size() > 1)
    {
        dkString end = stringList.at(stringList.size()-1);
        return end;
    }
    else
    {
        // split into words separated by spaces and non-words
        stringList = split(QRegExp("\\W+"),QString::SkipEmptyParts);
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
        else
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

//int dkString::frontNumber() const
//{
//    dkString digits = frontDigits();
//    bool ok;
//    int outVal = digits.toInt(&ok);
//    if(ok)
//        return outVal;
//    else
//        return -1;
//}

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

QString dkString::getRtf() const
{
    QString outTxt = *this;
    outTxt.replace("\\", "\\\\");
    outTxt.replace("{", "\\{");
    outTxt.replace("}", "\\}");
    return outTxt;
}

dkString & dkString::operator=( const dkString & str){
  this->QString::operator=(str);
  return *this;
}

dkString & dkString::operator=( const QString & str){
  this->QString::operator=(str);
  return *this;
}
