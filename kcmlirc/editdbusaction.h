/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

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

#ifndef EDITDBUSACTION_H
#define EDITDBUSACTION_H

#include "ui_editdbusaction.h"

#include "dbusaction.h"
#include "model.h"

#include <QStandardItemModel>

class EditDBusAction: public QWidget{
    Q_OBJECT
    
    public:
    
        EditDBusAction(DBusAction *action, QWidget* parent = 0, Qt::WFlags flags = 0);
        ~EditDBusAction();
        
        void applyChanges();

    private:
        Ui::EditDBusAction ui;
        
        DBusAction *m_action;
        DBusServiceModel *m_dbusServiceModel;
        DBusFunctionModel *m_dbusFunctionModel;
        ArgumentsModel *m_argumentsModel;
        
    private slots:
        void refreshDBusFunctions(const QModelIndex &index);
        void refreshArguments(const QModelIndex &index);
};

#endif // EDITDBUSACTION_H
