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

#include "editprofileaction.h"
#include <profileserver.h>

#include <kdebug.h>

EditProfileAction::EditProfileAction(ProfileAction *action, QWidget* parent, Qt::WFlags flags): QWidget(parent, flags) {
    m_action = action;
    ui.setupUi(this);
    
    // Init Profiles
    m_profileModel = new ProfileModel(ui.tvDBusApps);
    ui.tvDBusApps->setModel(m_profileModel);
    ui.tvDBusApps->setRootIsDecorated(false);
    
    //Init Templates
    m_templateModel = new ActionTemplateModel(ui.tvDBusFunctions);
    ui.tvDBusFunctions->setModel(m_templateModel);
    connect(ui.tvDBusApps->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), SLOT(refreshTemplates(const QModelIndex &)));
    

    // Init Arguments View
    m_argumentsModel = new ArgumentsModel(ui.tvArguments);
    ui.tvArguments->setModel(m_argumentsModel);
    ui.tvArguments->setItemDelegate(new ArgumentDelegate(ui.tvArguments));
    connect(ui.tvDBusFunctions->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), SLOT(refreshArguments(const QModelIndex &)));
    

    // Load our action here
    kDebug() << "searching for action:" << m_action->name() << m_action->description() << m_action->application();
    if(!m_action->application().isEmpty()){
        // Find Profile and Template in Models and set current index
        QModelIndex index = m_profileModel->find(m_action);
        ui.tvDBusApps->selectionModel()->setCurrentIndex(index, QItemSelectionModel::SelectCurrent);
        index = m_templateModel->find(m_action);
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
    }
    if(!m_action->function().args().isEmpty()){
        m_argumentsModel->refresh(m_action->function());
    }
}


EditProfileAction::~EditProfileAction() {
}

bool EditProfileAction::checkForComplete() const {
    if(ui.tvDBusFunctions->selectionModel()->currentIndex().isValid()){
        return true;
    }
    return false;
}

void EditProfileAction::applyChanges(){
    ProfileActionTemplate actionTemplate = m_templateModel->actionTemplate(ui.tvDBusApps->selectionModel()->currentIndex());
    kDebug() << "applyChanges to action:" << actionTemplate.profileId();
    m_action->setApplication(actionTemplate.service());
    m_action->setNode(actionTemplate.node());
    Prototype prototype = actionTemplate.function();
    prototype.setArgs(m_argumentsModel->arguments());
    m_action->setFunction(prototype);
    m_action->setActionTemplateId(actionTemplate.actionTemplateId());
    m_action->setProfileId(actionTemplate.profileId());

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

void EditProfileAction::refreshTemplates(const QModelIndex& index) {
    m_templateModel->refresh(m_profileModel->profile(ui.tvDBusApps->selectionModel()->currentIndex()));
    m_templateModel->setColumnCount(2);
    ui.tvDBusFunctions->resizeColumnToContents(0);
    emit formComplete(index.isValid());
}

void EditProfileAction::refreshArguments(const QModelIndex &index) {
    ProfileActionTemplate actionTemplate = m_templateModel->actionTemplate(index);
    kDebug() << "got template:" << actionTemplate.actionName() << "function:" << actionTemplate.function().name();
    m_argumentsModel->refresh(actionTemplate.function());
    ui.tvArguments->resizeColumnsToContents();
    ui.tvArguments->horizontalHeader()->setStretchLastSection(true);

    if(actionTemplate.destination() == DBusAction::Unique){
        ui.gbUnique->setEnabled(false);
    } else {
        ui.gbUnique->setEnabled(true);
    }
    emit formComplete(index.isValid());
}

