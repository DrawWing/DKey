#ifndef COUPLETDIALOG_H
#define COUPLETDIALOG_H

#include "dkCouplet.h"

#include <QDialog>

QT_BEGIN_NAMESPACE
class QLabel;
class QPushButton;
class QTextEdit;
class QLineEdit;
class QGroupBox;
class QSpinBox;
class QRadioButton;
QT_END_NAMESPACE

class coupletDialog : public QDialog
{
    Q_OBJECT

public:
    coupletDialog(QWidget *parent = 0);
    coupletDialog(dkCouplet * inCouplet, int from, int to, QWidget *parent = 0);

    QTextEdit *lead1Text;
    QTextEdit *lead2Text;

    QRadioButton *radioRef1;
    QRadioButton *radioEnd1;
    QRadioButton *radioRef2;
    QRadioButton *radioEnd2;

    QSpinBox *pointer1;
    QSpinBox *pointer2;
    QLineEdit *endpoint1;
    QLineEdit *endpoint2;

protected:
    void accept();

private slots:
    void setEndpoints1();
    void setEndpoints2();

private:
    void createWidget();
    void fillData();

    QLabel *lead1Label;
    QLabel *addressLabel;
    QPushButton *okButton;
    QPushButton *cancelButton;
    dkCouplet *thisCouplet;
};

#endif // COUPLETDIALOG_H
