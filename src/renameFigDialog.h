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

#ifndef RENAMEFIGDIALOG_H
#define RENAMEFIGDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QPushButton;
class QLineEdit;
class QLabel;
QT_END_NAMESPACE

class renameFigDialog : public QDialog
{
    Q_OBJECT

public:
    renameFigDialog(QWidget *parent = nullptr);

    QString getOldName() const;
    QString getNewName() const;

protected:
    void accept() override;

private:
    void createWidget();
    void readSettings();
    void writeSettings();
    bool isValidFigureName(const QString &name) const;

    QLineEdit *oldNameEdit;
    QLineEdit *newNameEdit;

    QPushButton *okButton;
    QPushButton *cancelButton;
};

#endif // RENAMEFIGDIALOG_H
