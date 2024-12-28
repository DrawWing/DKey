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

#include "termDialog.h"
#include "dkString.h"

termDialog::termDialog(QWidget *parent)
    : QDialog(parent)
{
    createWidget();
}

termDialog::termDialog(dkTerm * inTerm, QWidget *parent)
    : QDialog(parent)
{
    createWidget();
    thisTerm = inTerm;
    fillData();
}

void termDialog::createWidget()
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

    // synonyms
    QLabel *termLabel = new QLabel(tr("Synonyms separated by commas:"));
    synonymsTxt = new QLineEdit;

    // definition
    QLabel *definitionLabel = new QLabel(tr("Definition:"));
    definitionTxt = new QTextEdit;
    definitionTxt->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(definitionTxt,SIGNAL(customContextMenuRequested(const QPoint&)),
            this,SLOT(showContextMenu(const QPoint &)));
    connect(definitionTxt, SIGNAL(currentCharFormatChanged(QTextCharFormat)),
            this, SLOT(currentCharFormatChanged(QTextCharFormat)));

    // main layout
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(termLabel);
    mainLayout->addWidget(synonymsTxt);
    mainLayout->addWidget(definitionLabel);
    mainLayout->addWidget(definitionTxt);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    setWindowTitle(tr("Edit term"));
    resize(600, 300);
}

void termDialog::fillData()
{
//    setWindowTitle(QString ("Edit term number: %1").arg(thisTerm->getNumber()));

    synonymsTxt->setText(thisTerm->getSynonymsTxt());
    definitionTxt->setHtml(thisTerm->getDefinition());
}

void termDialog::accept()
{
    // Update the term
    thisTerm->setSynonyms(synonymsTxt->text().toLower().simplified());

    dkString htmlTxt = definitionTxt->toHtml();
    htmlTxt = htmlTxt.cleanHtml();
    thisTerm->setDefinition(htmlTxt);

    QDialog::accept();
}

void termDialog::textBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
}

void termDialog::textUline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(actionTextUline->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void termDialog::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(actionTextItalic->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void termDialog::clearFormat()
{
//    QTextCharFormat fmt;
//    mergeFormatOnWordOrSelection(fmt);
    definitionTxt->setHtml(definitionTxt->toPlainText());
}

void termDialog::currentCharFormatChanged(const QTextCharFormat &format)
{
    QFont theFont = format.font();
    actionTextItalic->setChecked(theFont.italic());
    actionTextBold->setChecked(theFont.bold());
    actionTextUline->setChecked(theFont.underline());
}

void termDialog::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = definitionTxt->textCursor();
    if(!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    definitionTxt->mergeCurrentCharFormat(format);
}

void termDialog::showContextMenu(const QPoint &pt)
{
    QMenu * menu = definitionTxt->createStandardContextMenu();
    menu->removeAction(menu->actions().at(6)); // remove delete
    menu->addSeparator();
    menu->addAction(actionTextItalic);
    menu->addAction(actionTextBold);
    menu->addAction(actionTextUline);
    menu->addAction(actionTextClear);
    menu->exec(definitionTxt->mapToGlobal(pt));
    delete menu;
}

