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

#ifndef DKTERMLIST_H
#define DKTERMLIST_H

#include <QList>
#include <QString>
#include <QDomElement>

#include "dkTerm.h"
#include "dkStringInt.h"

class dkTermList
{
public:
    dkTermList();
    dkTerm at(int i) const;
    void setTerm(const dkTerm & inTerm, int i);
    void setTag(const QString &inTxt);
    int size() const;
    void clear();
    void fromDkXml(const QDomElement &inElement, int ver = 1);
    void importTxt(QStringList & inTxtList);
    QString exportTxt();
    QString getTag() const;
    QString getDkXml() const;
    QString getHtml() const;
    QString getDefinition(QString &inKey) const;
    QString getDefinitionTxt(QString &inKey) const;
    QString linkGlossary(QString &inTxt) const;
    QList< dkStringInt > sortBySize() const;
    void sort();
    int contains(QString inKey);

    void push_back(dkTerm inTerm);
    void insertAt(int i, dkTerm inTerm);
    void insertDummyAt(int i);
    void removeAt(int i);

    QString addLinks(QString &inHtmlTxt);
    bool isKeyPresent(QStringList &keyList, QStringList &inList);
    void linkKey(QStringList &keyList, int keyIndex, QStringList &inList);


private:
    QList< dkTerm > thisList;
    QString tag;
    QString error;

};

#endif // DKTERMLIST_H
