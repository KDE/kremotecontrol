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

#include "remotelist.h"
#include "profileaction.h"

#include <kglobal.h>
#include <kconfig.h>

#include <QList>


RemoteList::RemoteList() {
}

bool RemoteList::contains(const QString& remoteName) const {
    for(QList<Remote*>::const_iterator i = constBegin(); i != constEnd(); ++i){
        if((*i)->name() == remoteName){
            return true;
        }
    }
    return false;
}

void RemoteList::saveToConfig(const QString& configName) {
    KConfig config(configName);
    for(QList<Remote*>::const_iterator i = constBegin(); i != constEnd(); ++i){
        // Clear out old entries for this remote
        config.deleteGroup((*i)->name());
        KConfigGroup remoteGroup(&config, (*i)->name());
        // Save Remote properties here
        remoteGroup.writeEntry("DefaultMode", (*i)->defaultMode()->name());
        
        foreach(const Mode *mode, (*i)->allModes()){
            KConfigGroup modeGroup(&remoteGroup, mode->name());
            // Save Mode properties here
            modeGroup.writeEntry("IconName", mode->iconName());
            modeGroup.writeEntry("Button", mode->button());
            
            int i = 0; // The ID for the ActionGroup in config file as actions may have no unique attribute
            foreach(Action *action, mode->actions()){
                KConfigGroup actionGroup(&modeGroup, QString::number(i));
                // Save Action properties here
                action->saveToConfig(actionGroup);
                i++;
            }
        }        
    }
}

void RemoteList::loadFromConfig(const QString& configName) {
    KConfig config(configName, KConfig::NoGlobals);

    foreach(const QString &remoteGroupName, config.groupList()){
        Remote *remote = new Remote(remoteGroupName);
        KConfigGroup remoteGroup(&config, remoteGroupName);
        foreach(const QString &modeName, remoteGroup.groupList()){
            KConfigGroup modeGroup(&remoteGroup, modeName);
            Mode *mode;
            if(modeName == "Master") { // A Remote always has a Master Mode... Adding a second one will not work
                mode = remote->masterMode();
                mode->setIconName(modeGroup.readEntry("IconName"));
            } else {
                mode = new Mode(modeName, modeGroup.readEntry("IconName"));
            }
            foreach(const QString &actionId, modeGroup.groupList()){
                KConfigGroup actionGroup(&modeGroup, actionId);
                // Read Action properties here
                Action *action = 0;
                Action::ActionType actionType = (Action::ActionType) actionGroup.readEntry("Type", 0);
                switch(actionType){
                    case Action::DBusAction:
                        action = new DBusAction();
                        break;
                    case Action::ProfileAction:
                        action = new ProfileAction();
                        break;
                }
                if(!action){
                    continue;
                }
                action->loadFromConfig(actionGroup);
                
                mode->addAction(action);
            }
            // Read Mode properties here
            mode->setIconName(modeGroup.readEntry("IconName", "infrared-remote"));
            mode->setButton(modeGroup.readEntry("Button"));
            
            remote->addMode(mode);
        }
        // Read Remote properties here
        remote->setDefaultMode(remoteGroup.readEntry("DefaultMode"));
        
        append(remote);
    }

}
Remote* RemoteList::getRemote ( const QString& remoteName ){
   for(QList<Remote*>::const_iterator i = constBegin(); i != constEnd(); ++i){
        if((*i)->name() == remoteName){
            return *i;
        }
    }
    return 0;
}

