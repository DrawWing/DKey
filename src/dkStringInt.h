#ifndef DKSTRINGINT_H
#define DKSTRINGINT_H

#include <vector>
#include <QString>

class dkStringInt
{
public:
    dkStringInt();
    dkStringInt(QString inString, int inValue);
    void setString(QString inString);
    void setInt(int inValue);
    QString getString() const;
    int getInt() const;
    bool operator<( const dkStringInt &p1 ) const;
private:
    QString string;
    int value;
};

#endif // DKSTRINGINT_H
