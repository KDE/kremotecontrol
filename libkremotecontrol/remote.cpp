/*
    Copyright (C) <2010>  Michael Zanetti <michael_zanetti@gmx.net>

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

#include "remote.h"

#include <KLocale>
#include <kdebug.h>

class ModeChangeHandler{
    public:
        ModeChangeHandler(Remote *remote) {
            m_remote = remote;
        }
        
        virtual bool handleModeButton(const QString &button) = 0;
        virtual Remote::ModeChangeMode type() const = 0;
        virtual QStringList availableModeSwitchButtons() const = 0;
      
    protected:
        Remote *m_remote;
};    


class GroupModeChangeHandler : public ModeChangeHandler
{

    public:
        GroupModeChangeHandler(Remote* remote): ModeChangeHandler(remote){}

        bool handleModeButton(const QString& button) {
            int index= m_remote->currentMode()->button() == button ?  m_remote->m_modeList.indexOf(m_remote->currentMode()) : m_remote->m_modeList.size();
            int size = m_remote->m_modeList.size();
            if(index < size){
                for(int i = index +1; i < size ; ++i){
                    kDebug()<< "Size == "<< i;      
                    if(m_remote->m_modeList.at(i)->button() == button){
                        m_remote->setCurrentMode(m_remote->m_modeList.at(i));
                        return true;
                    }
                }
            }      
            for(int i = 0; i < index; ++i){
                kDebug()<< " else 	Size == "<< i;          
                if(m_remote->m_modeList.at(i)->button() == button){
                    m_remote->setCurrentMode(m_remote->m_modeList.at(i));
                    return true;
                }
            }
            return false;
        }
        
        Remote::ModeChangeMode type() const {
            return Remote::Group;
        }
        
        QStringList availableModeSwitchButtons() const {
            // GroupModeHandler allows to re-use all buttons
            QStringList retList;
            foreach(const Solid::Control::RemoteControlButton &button, Solid::Control::RemoteControl(m_remote->name()).buttons()){
                retList.append(button.name());
            }
            return retList;
        }
};


Remote::Remote() {
    // Always create the Master Mode and set it default
    Mode *masterMode = new Mode("Master");
    addMode(masterMode);
    setDefaultMode(masterMode);
    setCurrentMode(masterMode);
    m_modechangeHandler = new GroupModeChangeHandler(this);
}

Remote::Remote(const QString &remote, const QList<Mode*> &modes) {
    m_modeList = modes;
    m_remoteName = remote;
    m_modechangeHandler = new GroupModeChangeHandler(this);

    // Always create the Master Mode and set it default
    bool hasMaster = false;
    foreach(Mode *mode, m_modeList){
        if(mode->name() == "Master"){
            hasMaster = true;
	    setCurrentMode(mode);            
        }
    }
    if(!hasMaster){
        Mode *masterMode = new Mode("Master");	
        addMode(masterMode);
        setDefaultMode(masterMode);
        setCurrentMode(masterMode);
    }
    
}

Remote::~Remote() {
    while(!m_modeList.isEmpty()){
        delete m_modeList.takeFirst();
    }
}

QString Remote::name() const {
    return m_remoteName;
}

void Remote::moveModeUp(Mode* mode) {
    int oldPos = m_modeList.indexOf(mode);
    if(oldPos > 1){
        m_modeList.move(oldPos, oldPos - 1);
    }
}

void Remote::moveModeDown(Mode* mode) {
    int oldPos = m_modeList.indexOf(mode);
    if(oldPos < (m_modeList.count() - 1)){
        m_modeList.move(oldPos, oldPos + 1);
    }
}

QList<Mode*> Remote::allModes() const {
    return m_modeList;
}

void Remote::addMode(Mode *mode) {
    m_modeList.append(mode);
}

void Remote::removeMode(Mode *mode) {
    if(mode->name() == "Master"){
        kDebug() << "cannot delete the Master mode";
        return;
    }

    if(m_defaultMode == mode){
        // Deleting the Default Mode... Setting Master Mode to default
        foreach(Mode *tmp, m_modeList){
            if(tmp->name() == "Master"){
                m_defaultMode = tmp;
                break;
            }
        }
    }
    m_modeList.removeAll(mode);
    delete mode;
}

Mode* Remote::masterMode() const {
    foreach(Mode *mode, m_modeList){
//         kDebug() << "checking Mode" << mode->name();
        if(mode->name() == "Master"){	  
            return mode;
        }
    }
    kDebug() << "Master mode not found";
    // Huh??? No Master Mode? Should never happen as removeMode() doesn't delete the Master mode
    // and all ctors create a Master Mode...
    return 0;
}

Mode *Remote::defaultMode() const {
    return m_defaultMode;
}

void Remote::setDefaultMode(Mode *mode) {
    if(!m_modeList.contains(mode)){
        m_modeList.append(mode);
    }
    m_defaultMode = mode;
}

void Remote::setDefaultMode(const QString &modeName){
    foreach(Mode *mode, m_modeList){
        if(mode->name() == modeName){
            setDefaultMode(mode);
            return;
        }
    }
}

Mode* Remote::currentMode() const {
    if(m_currentMode != 0){
        return m_currentMode;
    }
    return m_defaultMode;
}

void Remote::setCurrentMode(Mode* mode) {
    m_currentMode = mode;
}

bool Remote::isAvailable() const {
    return Solid::Control::RemoteControl::allRemoteNames().contains(m_remoteName);
}

bool Remote::nextMode(const QString& button) {
    return m_modechangeHandler->handleModeButton(button);
}

Remote::ModeChangeMode Remote::modeChangeMode() const {
    return m_modechangeHandler->type();
}

QStringList Remote::availableModeSwitchButtons() const {
    return m_modechangeHandler->availableModeSwitchButtons();
}

QString Remote::nextModeButton() const {
    return m_nextModeButton;
}

QString Remote::previousModeButton() const {
    return m_previousModeButton;
}
