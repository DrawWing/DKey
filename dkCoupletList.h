#ifndef DKCOUPLETLIST_H
#define DKCOUPLETLIST_H

#include <QList>
#include <QString>
//#include <QStringList>

#include "dkCouplet.h"

class dkCoupletList
{
public:
    dkCoupletList();
    dkCouplet at(int i) const;
    dkCouplet getCoupletWithNumber(int number) const;
    void setCouplet(const dkCouplet & inCouplet, int i);
    int size() const;
    void clear();
    void fromTxt(QStringList & inTxtList);
    void importTxt(QStringList & inTxtList);
    void parse1numberKey(QStringList & inTxtList);
    void parse2numberKey(QStringList & inTxtList);
    void fromDkTxt(const QString & fileName);
    QList< dkCouplet > getList() const;
    QString getError() const;
    QString getDkTxt() const;
    QString getHtml() const;
    QString getHtmlTable(QString path = QString()) const;
    void findIntro(QStringList & inTxtList);
    void findFigs(QString &path);
    void findFrom();
    QList<int> findFrom(int index) const;
    void insertAt(int i, dkCouplet & inCouplet);
    void removeAt(int i);
    void stepDownAdr(int thd);
    void stepUpAdr(int thd);
//    QString frontDigits(const QString & inTxt) const;
    void pointerChain(int currIndex, int currNumber, QList<int> & chainList);
    void findPointerChains();
    void arrangeCouplets(int currNumber, QList<dkCouplet> &newList);

private:
    void appendCouplet(const QStringList & inTxt, int lineNo);
    QList<int> findStartNumbers(const QString & fileName) const;
    QList<int> findStartNumbers(QStringList & inTxtList) const;
    //    bool startsWithNumber(const QString & inTxt) const;

    QList< dkCouplet > thisList;
    QString intro;
    int introSize;
    QString error;
};

#endif // DKCOUPLETLIST_H
