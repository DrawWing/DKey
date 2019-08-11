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

#ifndef TERMDIALOG_H
#define TERMDIALOG_H

#include <QtWidgets>

#include <QDialog>
//QT_BEGIN_NAMESPACE
//class QPushButton;
//class QTextEdit;
//QT_END_NAMESPACE

#include "dkTerm.h"

class termDialog : public QDialog
{
    Q_OBJECT

public:
    termDialog(QWidget *parent = 0);
    termDialog(dkTerm *inTerm, QWidget *parent = 0);

protected:
    void accept();

private slots:
    void textBold();
    void textUline();
    void textItalic();
    void clearFormat();
    void currentCharFormatChanged(const QTextCharFormat &format);
    void showContextMenu(const QPoint &pt);

private:
    void createWidget();
    void fillData();
    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);

    QAction *actionTextItalic;
    QAction *actionTextBold;
    QAction *actionTextUline;
    QAction *actionTextClear;

    QLineEdit *synonymsTxt;
    QTextEdit *definitionTxt;
    QPushButton *okButton;
    QPushButton *cancelButton;

    dkTerm *thisTerm;
};

#endif // TERMDIALOG_H
