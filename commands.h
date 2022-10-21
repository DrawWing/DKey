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

#ifndef COMMANDS_H
#define COMMANDS_H

#include <QUndoCommand>

#include "mainwindow.h"

class PasteCommand : public QUndoCommand
{
public:
    explicit PasteCommand(MainWindow *inWindow, QList<int> &inRows,
                           QList<dkCouplet> &inCouplets, QUndoCommand *parent = 0);

    void undo();
    void redo();

private:
    MainWindow *window;
    QList<int> rowList;
    QList<dkCouplet> coupletList;
};

class CutCommand : public QUndoCommand
{
public:
    explicit CutCommand(MainWindow *inWindow, QList<int> &inRows,
                           QList<dkCouplet> &inCouplets, QUndoCommand *parent = 0);

    void undo();
    void redo();

private:
    MainWindow *window;
    QList<int> rowList;
    QList<dkCouplet> coupletList;
};

class InsertCommand : public QUndoCommand
{
public:
    explicit InsertCommand(MainWindow *inWindow, int inRow,
                           QUndoCommand *parent = 0);

    void undo();
    void redo();

private:
    MainWindow *window;
    int row;
};

class RemoveCommand : public QUndoCommand
{
public:
    explicit RemoveCommand(MainWindow *inWindow, QList<int> &inRows,
                           QList<dkCouplet> &inCouplets, QUndoCommand *parent = 0);

    void undo();
    void redo();

private:
    MainWindow *window;
    QList<int> rowList;
    QList<dkCouplet> coupletList;
};

class EditCommand : public QUndoCommand
{
public:
    explicit EditCommand(MainWindow *inWindow, dkCouplet &inNew, dkCouplet &inOld,
                         int inRow, QUndoCommand *parent = 0);

    void undo();
    void redo();

private:
    MainWindow *window;
    dkCouplet newCouplet;
    dkCouplet oldCouplet;
    int row;
};

#endif // COMMANDS_H
