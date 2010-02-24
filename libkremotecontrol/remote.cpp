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


	/**
	Goes to the next mode which has assigned the button given as argument.
	
	When pressed button is the same as the assigned button in the current mode, start searching for the next mode at index +1
	Otherwise the user want to switch from a other "button mode" into the current, so start the serach from the beginning
	If a button was found the current mode is set to it. If not you have got 2 cases:
	
	1. The current mode button is the same as the pressed button. We are in the last element of the mode list which hast assigned 
	this button to its mode. Switch to master.
	
	2. The pressed button has no assigned mode button. Stay in the current mode.
	
	@return true if the mode was changed.
	*/
        bool handleModeButton(const QString& button) {
	    
            int size = m_remote->m_modeList.size();
	    int currentModeHasAssignedPressedButton = m_remote->currentMode()->button() == button;	    
	    int index= currentModeHasAssignedPressedButton ? m_remote->m_modeList.indexOf(m_remote->currentMode()) +1 : 0;	    
	    for(index; index < size ; ++index){
                    if(m_remote->m_modeList.at(index)->button() == button){
                        m_remote->setCurrentMode(m_remote->m_modeList.at(index));
			return true;
                    }                    
	    }
	    if(currentModeHasAssignedPressedButton){
	      m_remote->setCurrentMode(m_remote->masterMode());
	      return true;
	    }			    	    	    
	    kDebug()<< "Mode with button " << button << " not available. Mode is not changed.";
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
