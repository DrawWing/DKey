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

#ifndef DKFORMAT_H
#define DKFORMAT_H

#include <QList>
#include <QString>
#include <QDir>

#include "dkTermList.h"
#include "dkCoupletList.h"
#include "dkCouplet.h"

class dkFormat
{
public:
    dkFormat();
    void clear();
    void setFilePath(const QString & inTxt);
    void setGlossary(dkTermList * inList);
    void setEndpoints(dkTermList * inList);
    void setFigures(dkTermList * inList);
    QString keyHtml(dkCoupletList & inList, bool withPath = true);
    QString keyHtmlLst(dkCoupletList & inList);
    // QString keyMd(dkCoupletList & inList, bool withPath = true);
    QString coupletHtml(dkCouplet &theCouplet, bool withPath = true);
    QString coupletHtmlLst(dkCouplet &theCouplet);
    // QString coupletMd(dkCouplet &theCouplet, bool withPath = true);
    QString glossaryHtml(bool withPath = true);
    QString endpointsHtml(bool withPath = true);
    QString figuresHtml(bool withPath = true);
    QString glossHtml(int nr, bool withPath = true);
    QString imagesHtml(QString &inTxt, bool withPath);
    QString imgHtml(const QString &inName, bool withPath = true) const;
//    QString linkGlossary(QString &inHtmlTxt);
//    QString linkEndpoints(QString &inHtmlTxt);
    QString addLinks(QString &inHtmlTxt);
    void addFigLinks(QString &inHtmlTxt);

private:
    QStringList findFigs(QString &inTxt);
    QString figExist(QDir &keyDir, QString & inTxt) const;
//    QString figExist(QString & inTxt) const;
    void findFigFiles();
    void linkGlossaryKey(QString &keyTxt, int keyIndex, QString &htmlTxt);
//    bool isKeyPresent1(QStringList &keyList, QStringList &inList);
//    bool isKeyPresent(QStringList &keyList, QStringList &inList);
//    bool isKey(QStringList &keyList, QStringList &inList, int position);
//    void linkKey(QStringList &keyList, int keyIndex, QStringList &inList);

    QString filePath;
    dkTermList * glossary;
    dkTermList * endpoints;
    dkTermList * figures;
    QStringList figFiles;
    QString error;
};

#endif // DKFORMAT_H
