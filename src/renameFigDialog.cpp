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
#include <QSettings>
#include "renameFigDialog.h"

renameFigDialog::renameFigDialog(QWidget *parent)
    : QDialog(parent)
{
    createWidget();
    readSettings();

    setWindowTitle(tr("Rename Figure"));
    setWindowModality(Qt::WindowModal);
    setMinimumWidth(400);

    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    oldNameEdit->setFocus();
}

void renameFigDialog::createWidget()
{
    QLabel *oldLabel = new QLabel(tr("Old figure name:"));
    oldNameEdit = new QLineEdit();
    oldNameEdit->setPlaceholderText(tr("e.g., 1a"));

    QLabel *newLabel = new QLabel(tr("New figure name:"));
    newNameEdit = new QLineEdit();
    newNameEdit->setPlaceholderText(tr("e.g., 1"));

    okButton = new QPushButton(tr("OK"));
    cancelButton = new QPushButton(tr("Cancel"));

    QVBoxLayout *mainLayout = new QVBoxLayout();

    mainLayout->addWidget(oldLabel);
    mainLayout->addWidget(oldNameEdit);

    mainLayout->addSpacing(12);

    mainLayout->addWidget(newLabel);
    mainLayout->addWidget(newNameEdit);

    mainLayout->addSpacing(18);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

QString renameFigDialog::getOldName() const
{
    return oldNameEdit->text().trimmed();
}

QString renameFigDialog::getNewName() const
{
    return newNameEdit->text().trimmed();
}

bool renameFigDialog::isValidFigureName(const QString &name) const
{
    if (name.isEmpty())
        return false;

    for (const QChar &c : name)
    {
        if (c == '/' || c == '\\' || c == ':' || c == '*' || c == '?' || c == '"' || c == '<' || c == '>' || c == '|')
            return false;
    }

    return true;
}

void renameFigDialog::accept()
{
    QString oldName = getOldName();
    QString newName = getNewName();

    if (oldName.isEmpty())
    {
        QMessageBox::warning(this, tr("Error"),
                           tr("Please enter the old figure name."));
        oldNameEdit->setFocus();
        return;
    }

    if (newName.isEmpty())
    {
        QMessageBox::warning(this, tr("Error"),
                           tr("Please enter the new figure name."));
        newNameEdit->setFocus();
        return;
    }

    if (oldName.toLower() == newName.toLower())
    {
        QMessageBox::warning(this, tr("Error"),
                           tr("Old and new names must be different."));
        newNameEdit->setFocus();
        return;
    }

    if (!isValidFigureName(oldName))
    {
        QMessageBox::warning(this, tr("Error"),
                           tr("Invalid characters in old figure name."));
        oldNameEdit->setFocus();
        return;
    }

    if (!isValidFigureName(newName))
    {
        QMessageBox::warning(this, tr("Error"),
                           tr("Invalid characters in new figure name."));
        newNameEdit->setFocus();
        return;
    }

    writeSettings();
    QDialog::accept();
}

void renameFigDialog::readSettings()
{
    QSettings settings("DrawWing", "DKey");
    oldNameEdit->setText(settings.value("renameFig/oldName", "").toString());
    newNameEdit->setText(settings.value("renameFig/newName", "").toString());
}

void renameFigDialog::writeSettings()
{
    QSettings settings("DrawWing", "DKey");
    settings.setValue("renameFig/oldName", oldNameEdit->text());
    settings.setValue("renameFig/newName", newNameEdit->text());
}
