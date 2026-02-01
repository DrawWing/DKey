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

#ifndef DKTERM_H
#define DKTERM_H

#include <QString>
#include <QStringList>
#include <QDomElement>

#include "dkString.h"

class dkTerm
{
public:
    dkTerm();
    dkTerm(const QString &inTxt);
    void clear();
    void fromDkXml(const QDomElement &inElement, int ver = 1);
    void readXmlSynonyms(const QDomElement &inElement);

//    int getNumber() const;
    QStringList getSynonymsList() const;
    QString getSynonyms1() const;
    QString getSynonymsTxt() const;
    QString getDefinition() const;
    QString getTxt() const;
    QString exportTxt() const;
    QString getDkXml() const;
    QString getHtml(int number) const;
    QString getDefinition(QString &inKey) const;
//    void setNumber(int inVal);
    void setSynonyms(QStringList inList);
    void setSynonyms(QString inTxt);
    void setDefinition(QString inTxt);
    bool isEmpty() const;
    bool contains(QString inKey);
    bool operator<( const dkTerm &p1 ) const;

private:
//    int number;
    QStringList  synonyms;
    dkString definition;
    QString error;
};

#endif // DKTERM_H
