/*
    Copyright (C) <2010> Michael Zanetti <michael_zanetti@gmx.net>

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

#include "addaction.h"
#include "dbusaction.h"
#include "profileaction.h"
#include "editactioncontainer.h"

#include <kdebug.h>


AddAction::AddAction() {
    QWidget *widget = new QWidget(this);
    ui.setupUi(widget);
    setMainWidget(widget);
    
    
    ui.cbActionType->addItem(i18n("I whish to create an Action from a Template"), Action::ProfileAction);
    ui.cbActionType->addItem(i18n("I whish to create an Action using D-Bus"), Action::DBusAction);
}


Action::ActionType AddAction::getType() {
    kDebug() << "current index" << ui.cbActionType->currentIndex() << "type" << ui.cbActionType->itemData(ui.cbActionType->currentIndex()).toInt();
    return (Action::ActionType)ui.cbActionType->itemData(ui.cbActionType->currentIndex()).toInt();
}

Action *AddAction::createAction(const QString &remote) {
    Action *action = 0;
    if(exec() == KDialog::Accepted){
        switch(getType()){
            case Action::ProfileAction:
                action = new ProfileAction();
                break;
            case Action::DBusAction:
                action = new DBusAction();
                break;
        }
        if(action){
            EditActionContainer actionContainer(action, remote);
            if(actionContainer.exec() != KDialog::Accepted){
                delete action;
                action = 0;
            }
        }
    }
    return action;
}
