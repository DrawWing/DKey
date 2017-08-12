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
//    int frontNumber() const;
    void removeFrontNonLetter();
    void chopFront(int n);

    dkString & operator=( const dkString & );
    dkString & operator=( const QString & );

};


#endif // DKSTRING_H
