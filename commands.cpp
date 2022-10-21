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

#include "commands.h"

PasteCommand::PasteCommand(MainWindow *inWindow, QList<int> &inRows,
                             QList<dkCouplet> &inCouplets, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    window = inWindow;
    rowList = inRows;
    coupletList = inCouplets;
    setText(QObject::tr("Paste"));
}

void PasteCommand::undo()
{
    window->removeCouplets(rowList);
}

void PasteCommand::redo()
{
    window->insertCouplets(rowList, coupletList);
}

CutCommand::CutCommand(MainWindow *inWindow, QList<int> &inRows,
                             QList<dkCouplet> &inCouplets, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    window = inWindow;
    rowList = inRows;
    coupletList = inCouplets;
    setText(QObject::tr("Cut"));
}

void CutCommand::undo()
{
    window->insertCouplets(rowList, coupletList);
}

void CutCommand::redo()
{
    window->removeCouplets(rowList);
}

InsertCommand::InsertCommand(MainWindow *inWindow, int inRow,
                             QUndoCommand *parent)
    : QUndoCommand(parent)
{
    window = inWindow;
    row = inRow;
    setText(QObject::tr("Insert"));
}

void InsertCommand::undo()
{
    QList<int> rowList;
    rowList.push_back(row);
    window->removeCouplets(rowList);
}

void InsertCommand::redo()
{
    window->insertEmptyCouplet(row);
}

RemoveCommand::RemoveCommand(MainWindow *inWindow, QList<int> &inRows,
                             QList<dkCouplet> &inCouplets, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    window = inWindow;
    rowList = inRows;
    coupletList = inCouplets;
    setText(QObject::tr("Remove"));
}

void RemoveCommand::undo()
{
    window->insertCouplets(rowList, coupletList);
}

void RemoveCommand::redo()
{
    window->removeCouplets(rowList);
}

EditCommand::EditCommand(MainWindow *inWindow, dkCouplet &inNew, dkCouplet &inOld,
                         int inRow, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    window = inWindow;
    newCouplet = inNew;
    oldCouplet = inOld;
    row = inRow;
    setText(QObject::tr("Edit"));
}

void EditCommand::undo()
{
    window->updateCouplet(oldCouplet, row);
}

void EditCommand::redo()
{
    window->updateCouplet(newCouplet, row);
}
