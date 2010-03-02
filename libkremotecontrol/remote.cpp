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

class ModeChangeHandler
{
    public:
        ModeChangeHandler(Remote *remote) {
            m_remote = remote;
        }
        virtual ~ModeChangeHandler(){}

        virtual bool handleModeButton(const QString &button) = 0;
        virtual Remote::ModeChangeMode type() const = 0;
        virtual QStringList availableModeSwitchButtons() const = 0;
        virtual void addMode(Mode *mode) const = 0;
        virtual void handleModeButtonAssignment(const QString &button){
            Q_UNUSED(button);
        };
        virtual void handleModeButtonAssignments(){};
                
    protected:
        Remote *m_remote;
};


class GroupModeChangeHandler : public ModeChangeHandler
{

    public:
        GroupModeChangeHandler(Remote* remote) : ModeChangeHandler(remote) {}


        /**
        Goes to the next mode which has assigned the button given as argument.

        When the pressed button is the same as the assigned mode button in the current mode, start searching for the next mode at the index of the current mode +1.
        Otherwise the user want to switch from a mode which has another (or none) mode button as the given. So start the search from the beginning.

        If a button was found the current mode is set to it. If not you have got 2 cases:

        1. The current mode button is the same as the pressed button. We are in the last element of the mode list, which has assigned
        this button to its mode. Switch to master.

        2. The pressed button has no assigned mode button. Stay in the current mode.

        @return true if the mode was changed.
        */
        bool handleModeButton(const QString& button) {

            int size = m_remote->m_modeList.size();
            int currentModeHasButton = m_remote->currentMode()->button() == button;

            int index= currentModeHasButton ? m_remote->m_modeList.indexOf(m_remote->currentMode()) +1 : 0;

            for(; index < size ; ++index) {
                if (m_remote->m_modeList.at(index)->button() == button) {

                    m_remote->setCurrentMode(m_remote->m_modeList.at(index));
                    return true;
                }
            }

            if (currentModeHasButton) {
                //Current mode has the same button as the pressed, but no mode after the current mode which has the same button was found.
                // Switch to master mode.
                m_remote->setCurrentMode(m_remote->masterMode());
                return true;
            }

            //mode with this button is not available, this should usually not happen.In this case the mode is not changed
            kDebug() << "Mode with button " << button << " not available. Mode is not changed.";
            return false;
        }

        Remote::ModeChangeMode type() const {
            return Remote::Group;
        }

        QStringList availableModeSwitchButtons() const {
            // GroupModeHandler allows to re-use all buttons
            QStringList retList;
            foreach(const Solid::Control::RemoteControlButton &button, Solid::Control::RemoteControl(m_remote->name()).buttons()) {
                retList.append(button.name());
            }
            return retList;
        }

        void addMode(Mode* mode) const {
            m_remote->m_modeList.append(mode);
        };
};

class CycleModeChangeHandler : public ModeChangeHandler
{

    public:
        CycleModeChangeHandler(Remote* remote) : ModeChangeHandler(remote) {}

        bool handleModeButton(const QString& button) {
            int index = -1;
            if(!m_remote->previousModeButton().isEmpty() && m_remote->previousModeButton() == button){
                index = m_remote->m_modeList.indexOf(m_remote->currentMode());
                index = index > 0 ?  index-1  :  m_remote->m_modeList.size()-1;
            }else if (! m_remote->nextModeButton().isEmpty() && m_remote->nextModeButton() == button){
                index = m_remote->m_modeList.indexOf(m_remote->currentMode());
                index = index < m_remote->m_modeList.size() -1 ?  index+1  : 0;
            }else if((m_remote->currentMode()->button().isEmpty() || m_remote->currentMode()->button() != button )){
                index = indexOfModeButton(button);                            
            }
            if(index > -1){
                m_remote->setCurrentMode(m_remote->allModes().at(index));
                return true;
            }
            return false;
        }

        Remote::ModeChangeMode type() const {
            return Remote::Cycle;
        }

        QStringList availableModeSwitchButtons() const {
            QStringList retList;
            foreach(const Solid::Control::RemoteControlButton &button, Solid::Control::RemoteControl(m_remote->name()).buttons()) {
                retList << button.name();
            }
            retList.removeAll(m_remote->nextModeButton());
            retList.removeAll(m_remote->previousModeButton());
            foreach(const Mode *mode, m_remote->m_modeList){
                retList.removeAll(mode->button());
            }
            return retList;
        }
                
        void addMode(Mode* mode) const {
            foreach(Mode *m, m_remote->m_modeList){
                if(! mode->button().isEmpty() &&  m->button() == mode->button()){
                    kDebug() << "mode "<< m->name() << " has already assigned the button " << mode->button();
                    return;
                }
            }
            m_remote->m_modeList.append(mode);
        };

        virtual void handleModeButtonAssignment(const QString &button) {
            if(button.isEmpty()){
                return;
            }
            foreach(Mode* mode, m_remote->allModes()){
                if(mode->button() == button){
                    mode->setButton(QString());
                }
            }
        };

        int indexOfModeButton(const QString &button){
            if(button.isEmpty()){
                return -1;
            }
            for (int index=0; index < m_remote->m_modeList.size() ; ++index) {
                kDebug()<< "index " << index << " size " << m_remote->m_modeList.size();
                if (m_remote->m_modeList.at(index)->button() == button) {
                    return index;
                }
            }
            return -1;
        }
                
        virtual void handleModeButtonAssignments(){
            handleModeButtonAssignment(m_remote->nextModeButton());
            handleModeButtonAssignment(m_remote->previousModeButton());
            int size = m_remote->m_modeList.size();
            for(int index=0; index < size; ++index){
                QString buttonToValidate = m_remote->m_modeList.at(index)->button();
                if(! buttonToValidate.isEmpty()){
                    for(int counter=index+1; counter< size; ++counter){
                        Mode *mode = m_remote->m_modeList.at(counter);
                        if(!mode->button().isEmpty() && mode->button() == buttonToValidate){
                            mode->setButton(QString());
                        }
                    }
                }
            }
        };

};

Remote::Remote() {
    // Always create the Master Mode and set it default
    Mode *masterMode = new Mode("Master");
    addMode(masterMode);
    setDefaultMode(masterMode);
    setCurrentMode(masterMode);
    m_modechangeHandler = new GroupModeChangeHandler(this);
}

Remote::Remote(const QString &remote, ModeChangeMode changeMode) {
    m_remoteName = remote;
    m_modechangeHandler = 0;

    // Always create the Master Mode and set it default
    bool hasMaster = false;
    setModeChangeMode(changeMode);
    foreach(Mode *mode, m_modeList) {
        if (mode->name() == "Master") {
            hasMaster = true;
            setCurrentMode(mode);
        }
    }

    if (!hasMaster) {
        Mode *masterMode = new Mode("Master");
        addMode(masterMode);
        setDefaultMode(masterMode);
        setCurrentMode(masterMode);
    }
}

Remote::~Remote() {
    while (!m_modeList.isEmpty()) {
        delete m_modeList.takeFirst();
    }
}

QString Remote::name() const {
    return m_remoteName;
}

void Remote::moveModeUp(Mode* mode) {
    int oldPos = m_modeList.indexOf(mode);

    if (oldPos > 1) {
        m_modeList.move(oldPos, oldPos - 1);
    }
}

void Remote::moveModeDown(Mode* mode) {
    int oldPos = m_modeList.indexOf(mode);

    if (oldPos < (m_modeList.count() - 1)) {
        m_modeList.move(oldPos, oldPos + 1);
    }
}

QList<Mode*> Remote::allModes() const {
    return m_modeList;
}

void Remote::addMode(Mode* mode) {
    // Don't add a second master mode
    if(mode != masterMode() && mode->name() != "Master"){
        m_modechangeHandler->addMode(mode);
    }
}

void Remote::removeMode(Mode *mode) {
    if (mode->name() == "Master") {
        kDebug() << "cannot delete the Master mode";
        return;
    }

    if (m_defaultMode == mode) {
        // Deleting the Default Mode... Setting Master Mode to default
        foreach(Mode *tmp, m_modeList) {
            if (tmp->name() == "Master") {
                m_defaultMode = tmp;
                break;
            }
        }
    }

    m_modeList.removeAll(mode);
    delete mode;
}

Mode* Remote::masterMode() const {
    foreach(Mode *mode, m_modeList) {
//         kDebug() << "checking Mode" << mode->name();
        if (mode->name() == "Master") {
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
    if (!m_modeList.contains(mode)) {
        m_modeList.append(mode);
    }

    m_defaultMode = mode;
}

void Remote::setDefaultMode(const QString &modeName) {
    foreach(Mode *mode, m_modeList) {
        if (mode->name() == modeName) {
            setDefaultMode(mode);
            return;
        }
    }
}

Mode* Remote::modeByName(const QString& modeName) const {
    foreach(Mode *mode, m_modeList) {
        if (mode->name() == modeName) {
            return mode;
        }
    }
    return 0;
}

Mode* Remote::currentMode() const {
    if (m_currentMode != 0) {
        return m_currentMode;
    }

    return m_defaultMode;
}

void Remote::setCurrentMode(Mode* mode) {
    m_currentMode = mode;
}

bool Remote::isAvailable() const {
    kDebug() << "available remotes in solid:" << Solid::Control::RemoteControl::allRemoteNames();
    return Solid::Control::RemoteControl::allRemoteNames().contains(m_remoteName);
}

bool Remote::nextMode(const QString& button) {
    return m_modechangeHandler->handleModeButton(button);
}

Remote::ModeChangeMode Remote::modeChangeMode() const {
    return m_modechangeHandler->type();
}

void Remote::setModeChangeMode(Remote::ModeChangeMode modeChangeMode) {
    if(m_modechangeHandler){
        delete m_modechangeHandler;
    }
    if(modeChangeMode == Remote::Group){
        m_modechangeHandler = new GroupModeChangeHandler(this);
    }else{
        m_modechangeHandler = new CycleModeChangeHandler(this);
    }
    m_modechangeHandler->handleModeButtonAssignments();
}

QStringList Remote::availableModeSwitchButtons(const Mode *mode) const {
    QStringList buttonList = m_modechangeHandler->availableModeSwitchButtons();
    if(mode && !mode->button().isEmpty() && !buttonList.contains(mode->button())){
        buttonList.append(mode->button());
    }
    return buttonList;
}

QStringList Remote::availableNextModeButtons() const {
    QStringList buttonList = m_modechangeHandler->availableModeSwitchButtons();
    if(!nextModeButton().isEmpty() && !buttonList.contains(nextModeButton())){
        buttonList.append(nextModeButton());
    }
    return buttonList;
}

QStringList Remote::availablePreviousModeButtons() const {
    QStringList buttonList = m_modechangeHandler->availableModeSwitchButtons();
    if(!previousModeButton().isEmpty() && !buttonList.contains(previousModeButton())){
        buttonList.append(previousModeButton());
    }
    return buttonList;
}

QString Remote::nextModeButton() const {
    return m_nextModeButton;
}

void Remote::setNextModeButton(const QString& button) {
    m_nextModeButton = button;
    m_modechangeHandler->handleModeButtonAssignment(button);
}

QString Remote::previousModeButton() const {
    return m_previousModeButton;
}

void Remote::setPreviousModeButton(const QString& button) {
    m_previousModeButton = button;
    m_modechangeHandler->handleModeButtonAssignment(button);
}

