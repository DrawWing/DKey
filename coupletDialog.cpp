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

#include "coupletDialog.h"

#include <QtWidgets>

coupletDialog::coupletDialog(QWidget *parent)
    : QDialog(parent)
{
    createWidget();
}

coupletDialog::coupletDialog(dkCouplet * inCouplet, int to, QWidget *parent)
    : QDialog(parent)
{
    createWidget();

    pointer1->setMinimum(1);
    pointer1->setMaximum(to);
    pointer2->setMinimum(1);
    pointer2->setMaximum(to);

    thisCouplet = inCouplet;
    fillData();
}

void coupletDialog::setEndpoints1()
{
    if(radioRef1->isChecked())
    {
        pointer1->setVisible(true);
        endpoint1->setVisible(false);
    }
    else
    {
        pointer1->setVisible(false);
        endpoint1->setVisible(true);
    }
}

void coupletDialog::setEndpoints2()
{
    if(radioRef2->isChecked())
    {
        pointer2->setVisible(true);
        endpoint2->setVisible(false);
    }
    else
    {
        pointer2->setVisible(false);
        endpoint2->setVisible(true);
    }
}

void coupletDialog::createWidget()
{
    okButton = new QPushButton(tr("OK"));
    cancelButton = new QPushButton(tr("Cancel"));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addStretch(1);

    // lead1
    radioRef1 = new QRadioButton(tr("&Reference"));
    radioEnd1 = new QRadioButton(tr("&Endpoint"));

    pointer1 = new QSpinBox;
    endpoint1 = new QLineEdit;

    QHBoxLayout *pointer1Layout = new QHBoxLayout;
    pointer1Layout->addWidget(pointer1);
    pointer1Layout->addStretch(1);

    QGridLayout *grid1 = new QGridLayout;
    grid1->addWidget(radioRef1, 0, 0);
    grid1->addWidget(radioEnd1, 1, 0);
    grid1->addLayout(pointer1Layout, 0, 1);
    grid1->addWidget(endpoint1, 1, 1);

    QGroupBox *lead1Box = new QGroupBox(tr("First lead"));
    lead1Text = new QTextEdit;

    QVBoxLayout *lead1Layout = new QVBoxLayout;
    lead1Layout->addWidget(lead1Text);
    lead1Layout->addLayout(grid1);
    lead1Layout->addStretch(1);
    lead1Box->setLayout(lead1Layout);

    // lead2
    radioRef2 = new QRadioButton(tr("&Reference"));
    radioEnd2 = new QRadioButton(tr("&Endpoint"));

    pointer2 = new QSpinBox;
    endpoint2 = new QLineEdit;

    QHBoxLayout *pointer2Layout = new QHBoxLayout;
    pointer2Layout->addWidget(pointer2);
    pointer2Layout->addStretch(1);

    QGridLayout *grid2 = new QGridLayout;
    grid2->addWidget(radioRef2, 0, 0);
    grid2->addWidget(radioEnd2, 1, 0);
    grid2->addLayout(pointer2Layout, 0, 1);
    grid2->addWidget(endpoint2, 1, 1);

    QGroupBox *lead2Box = new QGroupBox(tr("Second lead"));
    lead2Text = new QTextEdit;

    QVBoxLayout *lead2Layout = new QVBoxLayout;
    lead2Layout->addWidget(lead2Text);
    lead2Layout->addLayout(grid2);
    lead2Layout->addStretch(1);
    lead2Box->setLayout(lead2Layout);

    // main layout
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(lead1Box);
    mainLayout->addWidget(lead2Box);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    radioRef1->setChecked(true);
    endpoint1->setVisible(false);
    radioRef2->setChecked(true);
    endpoint2->setVisible(false);

    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(radioRef1, SIGNAL(toggled(bool)), this, SLOT(setEndpoints1()));
    connect(radioEnd1, SIGNAL(toggled(bool)), this, SLOT(setEndpoints1()));
    connect(radioRef2, SIGNAL(toggled(bool)), this, SLOT(setEndpoints2()));
    connect(radioEnd2, SIGNAL(toggled(bool)), this, SLOT(setEndpoints2()));

    setWindowTitle(tr("Edit couplet"));
    resize(600, 300);
}

void coupletDialog::fillData()
{
    lead1Text->setText(thisCouplet->getLead1());
    lead2Text->setText(thisCouplet->getLead2());

    QString endpoint = thisCouplet->getEndpoint1();
    int pointer = thisCouplet->getPointer1();
    if(endpoint.isEmpty())
    {
        radioRef1->setChecked(true);
        pointer1->setValue(pointer);
    }
    else
    {
        radioEnd1->setChecked(true);
        endpoint1->setText(endpoint);
    }
    setEndpoints1();

    endpoint = thisCouplet->getEndpoint2();
    pointer = thisCouplet->getPointer2();
    if(endpoint.isEmpty())
    {
        radioRef2->setChecked(true);
        pointer2->setValue(pointer);
    }
    else
    {
        radioEnd2->setChecked(true);
        endpoint2->setText(endpoint);
    }
    setEndpoints2();
}

void coupletDialog::accept()
{
    // Update the couplet
    thisCouplet->setLead1(lead1Text->toPlainText());
    thisCouplet->setLead2(lead2Text->toPlainText());

    if(radioRef1->isChecked())
    {
        thisCouplet->setPointer1(pointer1->value());
    }
    else
    {
        thisCouplet->setEndpoint1(endpoint1->text());
    }

    if(radioRef2->isChecked())
    {
        thisCouplet->setPointer2(pointer2->value());
    }
    else
    {
        thisCouplet->setEndpoint2(endpoint2->text());
    }

    //call base class implementation
    QDialog::accept();
}
