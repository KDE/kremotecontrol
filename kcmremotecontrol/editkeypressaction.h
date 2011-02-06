/*
    Copyright (C) 2010 Michael Zanetti <mzanetti@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#ifndef EDITKEYPRESSACTION_H
#define EDITKEYPRESSACTION_H

#include "ui_editkeypressaction.h"

#include "keypressaction.h"

#include <QStandardItemModel>

class KeySequenceListModel;

class EditKeypressAction: public QWidget{
    Q_OBJECT

    public:

        explicit EditKeypressAction(KeypressAction *action, QWidget* parent = 0, Qt::WFlags flags = 0);
        ~EditKeypressAction();

        bool checkForComplete() const;
        void applyChanges();
        KeypressAction action() const;

    private:
        Ui::EditKeypressAction ui;

        KeypressAction *m_action;
        KeySequenceListModel *m_model;

    private Q_SLOTS:
        void keySequenceChanged();
        void setKeySequence(const QKeySequence &);
        void on_pbAdd_clicked();
        void on_pbRemove_clicked();
        void on_pbUp_clicked();
        void on_pbDown_clicked();
        void activateButtons();

    Q_SIGNALS:
        void formComplete(bool complete);
};

class KeySequenceItem: public QStandardItem
{
public:
    KeySequenceItem(const QKeySequence &seq);
    virtual QVariant data(int role = Qt::UserRole + 1) const;

private:
    QKeySequence m_sequence;
};

class KeySequenceListModel: public QStandardItemModel
{
public:
    KeySequenceListModel(QObject *parent = 0);
    void setList(QList<QKeySequence> list);
    QList<QKeySequence> keySeqenceList();
};

#endif // EDITDBUSACTION_H
