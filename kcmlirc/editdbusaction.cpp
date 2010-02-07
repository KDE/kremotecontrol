/*
    Copyright (C) 2010 Michael Zanetti <michael_zanetti@gmx.net>

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
#include <dbusinterface.h>

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
    

    // Load our action here
    if(!m_action->application().isEmpty()){
        // Find in or insert into Models and set current selection
        QModelIndex index = m_dbusServiceModel->findOrInsert(m_action);
        ui.tvDBusApps->selectionModel()->setCurrentIndex(index, QItemSelectionModel::SelectCurrent);
        index = m_dbusFunctionModel->findOrInsert(m_action);
        ui.tvDBusFunctions->selectionModel()->setCurrentIndex(index, QItemSelectionModel::SelectCurrent);
        
        // Load Options tab
        ui.cbAutostart->setChecked(m_action->autostart());
        ui.cbRepeat->setChecked(m_action->repeat());
        switch(m_action->destination()){
            case DBusAction::Unique:
                ui.gbUnique->setEnabled(false);
                break;
            case DBusAction::Top:
                ui.rbTop->setChecked(true);
                break;
            case DBusAction::Bottom:
                ui.rbBottom->setChecked(true);
                break;
            case DBusAction::All:
                ui.rbAll->setChecked(true);
                break;
            case DBusAction::None:
                ui.rbNone->setChecked(true);
                break;
        }
    } else {
        // Set default options
        ui.cbAutostart->setChecked(true);
        ui.cbRepeat->setChecked(true);
        ui.rbTop->setChecked(true);
    }
    if(!m_action->function().args().isEmpty()){
        m_argumentsModel->refresh(m_action->function());
    }
}


EditDBusAction::~EditDBusAction() {
}

bool EditDBusAction::checkForComplete() const {
    if(ui.tvDBusFunctions->selectionModel()->currentIndex().isValid()){
       return true; 
    }
    return false;
}

void EditDBusAction::applyChanges(){
    m_action->setApplication(m_dbusServiceModel->application(ui.tvDBusApps->selectionModel()->currentIndex()));
    m_action->setNode(m_dbusServiceModel->node(ui.tvDBusApps->selectionModel()->currentIndex()));
    Prototype prototype = m_dbusFunctionModel->getPrototype(ui.tvDBusFunctions->selectionModel()->currentIndex().row());
    prototype.setArgs(m_argumentsModel->arguments());
    m_action->setFunction(prototype);
    
    m_action->setAutostart(ui.cbAutostart->isChecked());
    m_action->setRepeat(ui.cbRepeat->isChecked());
    if(ui.gbUnique->isEnabled()){
        if(ui.rbAll->isChecked()){
            m_action->setDestination(DBusAction::All);
        } else if(ui.rbNone->isChecked()){
            m_action->setDestination(DBusAction::None);
        } else if(ui.rbTop->isChecked()){
            m_action->setDestination(DBusAction::Top);
        } else if(ui.rbBottom->isChecked()){
            m_action->setDestination(DBusAction::Bottom);
        }
    } else {
        m_action->setDestination(DBusAction::Unique);
    }
}

void EditDBusAction::refreshDBusFunctions(const QModelIndex& index) {
    m_dbusFunctionModel->refresh(m_dbusServiceModel->application(index), m_dbusServiceModel->node(index));
    ui.tvDBusFunctions->resizeColumnToContents(0);
    
    if(DBusInterface::getInstance()->isUnique(m_dbusServiceModel->application(index))){
        ui.gbUnique->setEnabled(false);
    } else {
        ui.gbUnique->setEnabled(true);
    }
    emit formComplete(index.parent().isValid());
}

void EditDBusAction::refreshArguments(const QModelIndex &index) {
    m_argumentsModel->refresh(m_dbusFunctionModel->getPrototype(index.row()));
    ui.tvArguments->resizeColumnsToContents();
    ui.tvArguments->horizontalHeader()->setStretchLastSection(true);
    emit formComplete(index.isValid());
}

