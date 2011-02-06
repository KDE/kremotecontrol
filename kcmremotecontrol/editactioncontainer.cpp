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

#include "editactioncontainer.h"
#include "editdbusaction.h"
#include "editprofileaction.h"
#include "editkeypressaction.h"
#include "dbusinterface.h"
#include "executionengine.h"

#include <kdebug.h>

#include "keypressaction.h"


EditActionContainer::EditActionContainer(Action *action, const QString &remote, QWidget* parent, Qt::WFlags flags): KDialog(parent, flags) {
    m_action = action;
    m_remote = remote;
    
    QWidget *widget = new QWidget(this);
    ui.setupUi(widget);
    setMainWidget(widget);
    
    setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Try);
    
    // Init Buttons
    foreach(const Solid::Control::RemoteControlButton &button, Solid::Control::RemoteControl(remote).buttons()){
        ui.cbButton->addItem(button.description(), button.name());
    }
    ui.cbButton->setCurrentIndex(ui.cbButton->findData(action->button()));
    connect(ui.cbButton, SIGNAL(currentIndexChanged(int)), SLOT(checkForComplete()));
    
    m_innerWidget = 0;
    switch(action->type()){
        case Action::DBusAction:{
            DBusAction *dbusAction = dynamic_cast<DBusAction*>(action);
            if(dbusAction){
                m_innerWidget = new EditDBusAction(dbusAction);
            }
            break;
            }
        case Action::ProfileAction:{
            ProfileAction *profileAction = dynamic_cast<ProfileAction*>(action);
            if(profileAction){
                m_innerWidget = new EditProfileAction(profileAction);
            }
            break;
            }
        case Action::KeypressAction: {
            KeypressAction *keypressAction = dynamic_cast<KeypressAction*>(action);
            if(keypressAction){
                m_innerWidget = new EditKeypressAction(keypressAction);
            }
            break;
        }
        default:
          kDebug() << "Invalid action type. Not creating inner Widget";
    }
    
    if(m_innerWidget){
        QHBoxLayout *innerLayout = new QHBoxLayout();
        innerLayout->setMargin(0);

        innerLayout->addWidget(m_innerWidget);
        ui.wActionWidget->setLayout(innerLayout);
        connect(m_innerWidget, SIGNAL(formComplete(bool)), SLOT(checkForComplete()));
    }
    checkForComplete();
    
    // Pause remote to make use of button presses here
    DBusInterface::getInstance()->ignoreButtonEvents(remote);
    connect(new Solid::Control::RemoteControl(remote), SIGNAL(buttonPressed(const Solid::Control::RemoteControlButton &)), SLOT(buttonPressed(const Solid::Control::RemoteControlButton &)));
    
}

void EditActionContainer::checkForComplete() {
    if(ui.cbButton->currentIndex() < 0){
        enableButtonOk(false);
        enableButton(Try, false);
        return;   
    }
    switch(m_action->type()){
        case Action::DBusAction:{
            EditDBusAction *dbusActionEditor = dynamic_cast<EditDBusAction*>(m_innerWidget);
            if(dbusActionEditor){
                bool complete = dbusActionEditor->checkForComplete();
                enableButtonOk(complete);
                enableButton(Try, complete);
                return;
            }
        }
        case Action::ProfileAction:{
            EditProfileAction *profileActionEditor = dynamic_cast<EditProfileAction*>(m_innerWidget);
            if(profileActionEditor){
                bool complete = profileActionEditor->checkForComplete();
                enableButtonOk(complete);
                enableButton(Try, complete);
                return;
            }
        }
        case Action::KeypressAction:{
            EditKeypressAction *keypressActionEditor = dynamic_cast<EditKeypressAction*>(m_innerWidget);
            if(keypressActionEditor){
                bool complete = keypressActionEditor->checkForComplete();
                enableButtonOk(complete);
                enableButton(Try, complete);
                return;
            }
        }
        default:
          kDebug() << "Invalid action type! Nothing to check for completeness!";
    }
}

void EditActionContainer::slotButtonClicked(int button) {
    if (button == KDialog::Ok) {
        switch(m_action->type()){
            case Action::DBusAction:{
                EditDBusAction *dbusActionEditor = dynamic_cast<EditDBusAction*>(m_innerWidget);
                if(dbusActionEditor){
                    dbusActionEditor->applyChanges();
                }
                break;
            }
            case Action::ProfileAction:{
                EditProfileAction *profileActionEditor = dynamic_cast<EditProfileAction*>(m_innerWidget);
                if(profileActionEditor){
                    profileActionEditor->applyChanges();
                }
                break;
            }
            case Action::KeypressAction:{
                EditKeypressAction *keypressActionEditor = dynamic_cast<EditKeypressAction*>(m_innerWidget);
                if(keypressActionEditor){
                    keypressActionEditor->applyChanges();
                }
                break;
            }
            default:
              kDebug() << "Invalid action type! No changes made to action!";
        }
        m_action->setButton(ui.cbButton->itemData(ui.cbButton->currentIndex()).toString());
    } else if(button == KDialog::Try){
        switch(m_action->type()){
            case Action::DBusAction:{
                EditDBusAction *dbusActionEditor = dynamic_cast<EditDBusAction*>(m_innerWidget);
                if(dbusActionEditor){
                    DBusAction action = dbusActionEditor->action();
                    ExecutionEngine::executeAction(&action);
                }
                break;
            }
            case Action::ProfileAction:{
                EditProfileAction *profileActionEditor = dynamic_cast<EditProfileAction*>(m_innerWidget);
                if(profileActionEditor){
                    ProfileAction action = profileActionEditor->action();
                    ExecutionEngine::executeAction(&action);
                }
                break;
            }
            case Action::KeypressAction: {
                EditKeypressAction *keypressActionEditor = dynamic_cast<EditKeypressAction*>(m_innerWidget);
                if(keypressActionEditor){
                    KeypressAction action = keypressActionEditor->action();
                    kDebug() << action.keySequenceList();
                    ExecutionEngine::executeAction(&action);
                }
                break;
            }
            default:
              kDebug() << "Invalid action type! Not executing!";
        }
        // return here because try button should not unpause remote
        return;
    }
    DBusInterface::getInstance()->considerButtonEvents(m_remote);
    KDialog::slotButtonClicked(button);
}

void EditActionContainer::buttonPressed(const Solid::Control::RemoteControlButton& button) {
    kDebug() << "button event received";
    if(button.remoteName() == m_remote) {
        ui.cbButton->setCurrentIndex(ui.cbButton->findData(button.name()));
    }
}
