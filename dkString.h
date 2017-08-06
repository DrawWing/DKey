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
//    int frontNumber() const;
    void removeFrontNonLetter();
    void chopFront(int n);

    dkString & operator=( const dkString & );
    dkString & operator=( const QString & );

};


#endif // DKSTRING_H
