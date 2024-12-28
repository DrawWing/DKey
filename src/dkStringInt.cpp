#include "dkStringInt.h"

dkStringInt::dkStringInt()
{
    string = "";
    value = 0.0;
}

dkStringInt::dkStringInt(QString inString, int inValue)
{
    string = inString;
    value = inValue;
}

void dkStringInt::setString(QString inString)
{
    string = inString;
}

void dkStringInt::setInt(int inValue)
{
    value = inValue;
}

QString dkStringInt::getString() const
{
    return string;
}

int dkStringInt::getInt() const
{
    return value;
}

bool dkStringInt::operator < ( const dkStringInt &p1 ) const
{
    return (string < p1.getString());
}
