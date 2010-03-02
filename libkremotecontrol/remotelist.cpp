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

// Save everything to config File in the format [Remote][modeIndex][actionIndex]
void RemoteList::saveToConfig(const QString& configName) {
    KConfig config(configName);
    KConfigGroup remotesGroup(&config, "Remotes");
    for(QList<Remote*>::const_iterator remoteIterator = constBegin(); remoteIterator != constEnd(); ++remoteIterator){
        // Clear out old entries for this remote
        remotesGroup.deleteGroup((*remoteIterator)->name());
        KConfigGroup remoteGroup(&remotesGroup, (*remoteIterator)->name());
        // Save Remote properties here
        remoteGroup.writeEntry("DefaultMode", (*remoteIterator)->defaultMode()->name());
        remoteGroup.writeEntry("ModeChangeMode", (*remoteIterator)->modeChangeMode() == Remote::Group ? "Group" : "Cycle");
        remoteGroup.writeEntry("NextModeButton", (*remoteIterator)->nextModeButton());
        remoteGroup.writeEntry("PreviousModeButton", (*remoteIterator)->previousModeButton());
        
        int modeIndex = 0;
        foreach(const Mode *mode, (*remoteIterator)->allModes()){
            KConfigGroup modeGroup(&remoteGroup, QString::number(modeIndex++));
            // Save Mode properties here
            modeGroup.writeEntry("Name", mode->name());
            modeGroup.writeEntry("IconName", mode->iconName());
            modeGroup.writeEntry("Button", mode->button());
            
            int actionIndex = 0;
            foreach(Action *action, mode->actions()){
                KConfigGroup actionGroup(&modeGroup, QString::number(actionIndex++));
                // Actions need to save themselves...
                action->saveToConfig(actionGroup);
            }
        }
    }
}

void RemoteList::loadFromConfig(const QString& configName) {
    clear(); //Drop old entries
    KConfig config(configName, KConfig::NoGlobals);
    KConfigGroup remotesGroup(&config, "Remotes");

    foreach(const QString &remoteGroupName, remotesGroup.groupList()){
        Remote *remote = new Remote(remoteGroupName);
        KConfigGroup remoteGroup(&remotesGroup, remoteGroupName);
        QStringList modeGroupList = remoteGroup.groupList();
        modeGroupList.sort();
        foreach(const QString &modeIndex, modeGroupList){
            KConfigGroup modeGroup(&remoteGroup, modeIndex);
            Mode *mode;
            QString modeName = modeGroup.readEntry("Name");
            if(modeName == "Master") { // A Remote always has a Master Mode... Adding a second one will not work
                mode = remote->masterMode();
                mode->setIconName(modeGroup.readEntry("IconName"));
            } else {
                mode = new Mode(modeName, modeGroup.readEntry("IconName"));
            }
            QStringList actionGroupList = modeGroup.groupList();
            actionGroupList.sort();
            foreach(const QString &actionId, actionGroupList){
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
        remote->setModeChangeMode(remoteGroup.readEntry("ModeChangeMode", "Group") == "Group" ? Remote::Group : Remote::Cycle);
        remote->setNextModeButton(remoteGroup.readEntry("NextModeButton"));
        remote->setPreviousModeButton(remoteGroup.readEntry("PreviousModeButton"));
        
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

