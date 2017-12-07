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

#ifndef COUPLETDIALOG_H
#define COUPLETDIALOG_H

#include <QDialog>
QT_BEGIN_NAMESPACE
class QPushButton;
class QTextEdit;
class QLineEdit;
class QGroupBox;
class QSpinBox;
class QRadioButton;
QT_END_NAMESPACE

#include "dkCouplet.h"

class coupletDialog : public QDialog
{
    Q_OBJECT

public:
    coupletDialog(QWidget *parent = 0);
    coupletDialog(dkCouplet * inCouplet, int to, QWidget *parent = 0);

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
    bool isNumber(const QString & inTxt);

    QPushButton *okButton;
    QPushButton *cancelButton;

    dkCouplet *thisCouplet;
};

#endif // COUPLETDIALOG_H
