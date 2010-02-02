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

#include "editdbusaction.h"
#include "model.h"
#include "dbusinterface.h"

#include <QHBoxLayout>

EditDBusAction::EditDBusAction(DBusAction *action, QWidget* parent, Qt::WFlags flags): QWidget(parent, flags) {
    m_action = action;
    ui.setupUi(this);
    
    // Init DBus services
    m_dbusServiceModel = new DBusServiceModel(ui.tvDBusApps);
    ui.tvDBusApps->setModel(m_dbusServiceModel);

    
    //Init DBus functions
    m_dbusFunctionModel = new DBusFunctionModel(ui.tvDBusFunctions);
    ui.tvDBusFunctions->setModel(m_dbusFunctionModel);
    connect(ui.tvDBusApps->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), SLOT(refreshDBusFunctions(const QModelIndex &)));
    

    // Init Arguments View
    m_argumentsModel = new ArgumentsModel(ui.tvArguments);
    ui.tvArguments->setModel(m_argumentsModel);
    ui.tvArguments->setItemDelegate(new ArgumentDelegate(ui.tvArguments));
    connect(ui.tvDBusFunctions->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), SLOT(refreshArguments(const QModelIndex &)));
    
    //    updateDBusFunctions(QModelIndex());

}


EditDBusAction::~EditDBusAction() {
}

void EditDBusAction::applyChanges(){
    m_action->setApplication(m_dbusServiceModel->application(ui.tvDBusApps->selectionModel()->currentIndex()));
    m_action->setNode(m_dbusServiceModel->node(ui.tvDBusApps->selectionModel()->currentIndex()));
    Prototype prototype = m_dbusFunctionModel->getPrototype(ui.tvDBusFunctions->selectionModel()->currentIndex().row());
    prototype.setArgs(m_argumentsModel->arguments());
    m_action->setFunction(prototype);
}

void EditDBusAction::refreshDBusFunctions(const QModelIndex& index) {
    m_dbusFunctionModel->refresh(m_dbusServiceModel->application(index), m_dbusServiceModel->node(index));
    ui.tvDBusFunctions->resizeColumnToContents(0);
}

void EditDBusAction::refreshArguments(const QModelIndex &index) {
    m_argumentsModel->refresh(m_dbusFunctionModel->getPrototype(index.row()));
    ui.tvArguments->resizeColumnsToContents();
    ui.tvArguments->horizontalHeader()->setStretchLastSection(true);
}

