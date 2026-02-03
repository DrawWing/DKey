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

#ifndef DKSTRING_H
#define DKSTRING_H

#include <QString>
#include <QTextDocumentFragment> // for toPlainText

class dkString : public QString
{
public:
    dkString();
    dkString(const QString & str );
    bool startsWithDigit() const;
    dkString findEndPart() const;
    dkString findFrontPart() const;
    dkString frontDigits() const;
    dkString endDigits() const;
    void removeFrontNonLetter();
    void removeEndNonLetterAndDigit();
    void removeFrontNonLetterAndDigit();
    void removeFrontDigit();
    void chopFront(int n);
    dkString getRtf() const;
    void removeHtml();
    dkString cleanHtml() const;
    dkString toPlainText() const;
    dkString html2mdXml() const;
    dkString html2md() const;
    dkString md2html() const;
    static const QString htmlBr;

    dkString(const dkString& other);
    dkString & operator=( const dkString & other);
    dkString & operator=( const QString & );
    // dkString(const dkString& other) = default;  // ✅ explicit, safe
    // dkString& operator=(const dkString& other) = default;
};

#endif // DKSTRING_H
