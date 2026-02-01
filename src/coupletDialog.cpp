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

#include <QtWidgets>
#include <QTextCursor>
#include "coupletDialog.h"

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
    // italic
    actionTextItalic = new QAction(tr("&Italic"), this);
    actionTextItalic->setPriority(QAction::LowPriority);
    actionTextItalic->setShortcut(Qt::CTRL + Qt::Key_I);
    QFont italic;
    italic.setItalic(true);
    actionTextItalic->setFont(italic);
    connect(actionTextItalic, SIGNAL(triggered()), this, SLOT(textItalic()));
    addAction(actionTextItalic);
    actionTextItalic->setCheckable(true);

    // bold
    actionTextBold = new QAction(tr("&Bold"), this);
    actionTextBold->setPriority(QAction::LowPriority);
    actionTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
    QFont bold;
    bold.setBold(true);
    actionTextBold->setFont(bold);
    connect(actionTextBold, SIGNAL(triggered()), this, SLOT(textBold()));
    addAction(actionTextBold);
    actionTextBold->setCheckable(true);

    // underline
    actionTextUline = new QAction(tr("&Underline"), this);
    actionTextUline->setPriority(QAction::LowPriority);
    actionTextUline->setShortcut(Qt::CTRL + Qt::Key_U);
    QFont underline;
    underline.setUnderline(true);
    actionTextUline->setFont(underline);
    connect(actionTextUline, SIGNAL(triggered()), this, SLOT(textUline()));
    addAction(actionTextUline);
    actionTextUline->setCheckable(true);

    // clear formating
    actionTextClear = new QAction(tr("&Clear formating"), this);
    actionTextClear->setPriority(QAction::LowPriority);
    connect(actionTextClear, SIGNAL(triggered()), this, SLOT(clearFormat()));
    addAction(actionTextClear);

    okButton = new QPushButton(tr("OK"));
    cancelButton = new QPushButton(tr("Cancel"));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addStretch(1);

    // lead1
    radioRef1 = new QRadioButton(tr("&Pointer"));
    radioEnd1 = new QRadioButton(tr("&Endpoint"));

    pointer1 = new QSpinBox;

    endpoint1 = new QTextEdit;
    endpoint1->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    endpoint1->setMinimumHeight(25);
    endpoint1->setMaximumHeight(25);
    endpoint1->setBaseSize(200, 25);
    endpoint1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    endpoint1->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(endpoint1,SIGNAL(customContextMenuRequested(const QPoint&)),
            this,SLOT(showEnd1ContextMenu(const QPoint &)));
    connect(endpoint1, SIGNAL(currentCharFormatChanged(QTextCharFormat)),
            this, SLOT(currentCharFormatChanged(QTextCharFormat)));

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
    lead1Text->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(lead1Text,SIGNAL(customContextMenuRequested(const QPoint&)),
            this,SLOT(showLead1ContextMenu(const QPoint &)));
    connect(lead1Text, SIGNAL(currentCharFormatChanged(QTextCharFormat)),
            this, SLOT(currentCharFormatChanged(QTextCharFormat)));

    QVBoxLayout *lead1Layout = new QVBoxLayout;
    lead1Layout->addWidget(lead1Text);
    lead1Layout->addLayout(grid1);
    lead1Box->setLayout(lead1Layout);

    // lead2
    radioRef2 = new QRadioButton(tr("&Pointer"));
    radioEnd2 = new QRadioButton(tr("&Endpoint"));

    pointer2 = new QSpinBox;

    endpoint2 = new QTextEdit;
    endpoint2->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    endpoint2->setMinimumHeight(25);
    endpoint2->setMaximumHeight(25);
    endpoint2->setBaseSize(200, 25);
    endpoint2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    endpoint2->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(endpoint2,SIGNAL(customContextMenuRequested(const QPoint&)),
            this,SLOT(showEnd2ContextMenu(const QPoint &)));
    connect(endpoint2, SIGNAL(currentCharFormatChanged(QTextCharFormat)),
            this, SLOT(currentCharFormatChanged(QTextCharFormat)));

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
    lead2Text->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(lead2Text,SIGNAL(customContextMenuRequested(const QPoint&)),
            this,SLOT(showLead2ContextMenu(const QPoint &)));
    connect(lead2Text, SIGNAL(currentCharFormatChanged(QTextCharFormat)),
            this, SLOT(currentCharFormatChanged(QTextCharFormat)));

    QVBoxLayout *lead2Layout = new QVBoxLayout;
    lead2Layout->addWidget(lead2Text);
    lead2Layout->addLayout(grid2);
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

    readSettings();
}

void coupletDialog::fillData()
{
    setWindowTitle(QString ("Edit couplet number: %1").arg(thisCouplet->getNumber()));

    lead1Text->setHtml(thisCouplet->getLead1());
    lead2Text->setHtml(thisCouplet->getLead2());

    QString endpoint = thisCouplet->getEndpoint1();
    int pointer = thisCouplet->getPointer1();
    if(pointer > 0)
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
    if(pointer > 0)
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
    writeSettings();
    if( (isNumber(endpoint1->toPlainText()) && !radioRef1->isChecked())
            || (isNumber(endpoint2->toPlainText()) && !radioRef2->isChecked()) )
    {
        QMessageBox msgBox;
        msgBox.setText("Normally numbers are set to pointers not to endpoints.");
        msgBox.setInformativeText("Are you sure to set a number to an endpoint?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();
        if(ret == QMessageBox::No)
            return;
    }

    // Update the couplet
    dkString htmlTxt = lead1Text->toHtml();
    htmlTxt = htmlTxt.cleanHtml();
    thisCouplet->setLead1(htmlTxt);
    htmlTxt = lead2Text->toHtml();
    htmlTxt = htmlTxt.cleanHtml();
    thisCouplet->setLead2(htmlTxt);

    if(radioRef1->isChecked())
    {
        thisCouplet->setPointer1(pointer1->value());
    }
    else
    {
        htmlTxt = endpoint1->toHtml();
        htmlTxt = htmlTxt.cleanHtml();
        thisCouplet->setEndpoint1(htmlTxt);
    }

    if(radioRef2->isChecked())
    {
        thisCouplet->setPointer2(pointer2->value());
    }
    else
    {
        htmlTxt = endpoint2->toHtml();
        htmlTxt = htmlTxt.cleanHtml();
        thisCouplet->setEndpoint2(htmlTxt);
    }

    QDialog::accept();
}

bool coupletDialog::isNumber(const QString &inTxt)
{
    bool ok;
    inTxt.simplified().toInt(&ok);
    return ok;
}

void coupletDialog::currentCharFormatChanged(const QTextCharFormat &format)
{
    QFont theFont = format.font();
    actionTextItalic->setChecked(theFont.italic());
    actionTextBold->setChecked(theFont.bold());
    actionTextUline->setChecked(theFont.underline());
}

void coupletDialog::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(actionTextItalic->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void coupletDialog::textBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
}

void coupletDialog::textUline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(actionTextUline->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void coupletDialog::clearFormat()
{
    if(lead1Text->hasFocus())
        lead1Text->setHtml(lead1Text->toPlainText());
    else if(lead2Text->hasFocus())
        lead2Text->setHtml(lead2Text->toPlainText());
    else if(endpoint1->hasFocus())
        endpoint1->setHtml(endpoint1->toPlainText());
    else if(endpoint2->hasFocus())
        endpoint2->setHtml(endpoint2->toPlainText());
}

void coupletDialog::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    if(lead1Text->hasFocus())
    {
        QTextCursor cursor = lead1Text->textCursor();
        if(!cursor.hasSelection())
            cursor.select(QTextCursor::WordUnderCursor);
        cursor.mergeCharFormat(format);
        lead1Text->mergeCurrentCharFormat(format);
    }
    else if(lead2Text->hasFocus())
    {
        QTextCursor cursor = lead2Text->textCursor();
        if(!cursor.hasSelection())
            cursor.select(QTextCursor::WordUnderCursor);
        cursor.mergeCharFormat(format);
        lead2Text->mergeCurrentCharFormat(format);
    }
    else if(endpoint1->hasFocus())
    {
        QTextCursor cursor = endpoint1->textCursor();
        if(!cursor.hasSelection())
            cursor.select(QTextCursor::WordUnderCursor);
        cursor.mergeCharFormat(format);
        endpoint1->mergeCurrentCharFormat(format);
    }
    else if(endpoint2->hasFocus())
    {
        QTextCursor cursor = endpoint2->textCursor();
        if(!cursor.hasSelection())
            cursor.select(QTextCursor::WordUnderCursor);
        cursor.mergeCharFormat(format);
        endpoint2->mergeCurrentCharFormat(format);
    }
}

void coupletDialog::showLead1ContextMenu(const QPoint &pt)
{
    QMenu * menu = lead1Text->createStandardContextMenu();
    menu->removeAction(menu->actions().at(6)); // remove delete
    menu->addSeparator();
    menu->addAction(actionTextItalic);
    menu->addAction(actionTextBold);
    menu->addAction(actionTextUline);
    menu->addAction(actionTextClear);
    menu->exec(lead1Text->mapToGlobal(pt));
    delete menu;
}

void coupletDialog::showLead2ContextMenu(const QPoint &pt)
{
    QMenu * menu = lead2Text->createStandardContextMenu();
    menu->removeAction(menu->actions().at(6)); // remove delete
    menu->addSeparator();
    menu->addAction(actionTextItalic);
    menu->addAction(actionTextBold);
    menu->addAction(actionTextUline);
    menu->addAction(actionTextClear);
    menu->exec(lead2Text->mapToGlobal(pt));
    delete menu;
}

void coupletDialog::showEnd1ContextMenu(const QPoint &pt)
{
    QMenu * menu = endpoint1->createStandardContextMenu();
    menu->removeAction(menu->actions().at(6)); // remove delete
    menu->addSeparator();
    menu->addAction(actionTextItalic);
    menu->addAction(actionTextBold);
    menu->addAction(actionTextUline);
    menu->addAction(actionTextClear);
    menu->exec(endpoint1->mapToGlobal(pt));
    delete menu;
}

void coupletDialog::showEnd2ContextMenu(const QPoint &pt)
{
    QMenu * menu = endpoint2->createStandardContextMenu();
    menu->removeAction(menu->actions().at(6)); // remove delete
    menu->addSeparator();
    menu->addAction(actionTextItalic);
    menu->addAction(actionTextBold);
    menu->addAction(actionTextUline);
    menu->addAction(actionTextClear);
    menu->exec(endpoint2->mapToGlobal(pt));
    delete menu;
}

void coupletDialog::readSettings()
{
    QString companyName = QGuiApplication::organizationName();
    QString windowName = QGuiApplication::applicationName() + " - coupletDialog";
    QSettings settings(companyName, windowName);
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(600, 300)).toSize();
    move(pos);
    resize(size);
}

void coupletDialog::writeSettings()
{
    QString companyName = QGuiApplication::organizationName();
    QString windowName = QGuiApplication::applicationName() + " - coupletDialog";
    QSettings settings(companyName, windowName);
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

