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

/**
  @author: Frank Scheffold
*/

#include "kremotecontroldaemon.h"


#include <kdelirc/libkremotecontrol/dbusinterface.h>
#include <kdelirc/libkremotecontrol/mode.h>
#include <kdelirc/libkremotecontrol/action.h>
#include <kdelirc/libkremotecontrol/executionengine.h>
#include <kdelirc/libkremotecontrol/remotelist.h>

#include <KCModuleInfo>
#include <KDebug>

#include<QHash>

using namespace Solid::Control;

K_PLUGIN_FACTORY(KRemoteControlDaemonFactory,
                 registerPlugin<KRemoteControlDaemon>();
    )
K_EXPORT_PLUGIN(KRemoteControlDaemonFactory("kremotecontrol"))

class KRemoteControlDaemonPrivate
{
  private:
   RemoteList m_remoteList;
   
  public:
  KRemoteControlDaemonPrivate(){
    
  };

  private:

  QHash<QString, Mode*> m_remoteModes;
  QStringList m_ignoreNextButtonList;
  
  public:
  
    RemoteList remoteList(){
      return m_remoteList;
    };
    
    void reload(){
      m_remoteList.loadFromConfig("kremotecontrolrc");
    };
    
    Remote* getRemote(const QString& remoteName) {          
      return m_remoteList.getRemote(remoteName);
    };

    bool isButtonEventIgnored(const QString &remote){
      return m_ignoreNextButtonList.contains(remote);      
    };
    
    void ignoreButtonEvents(const QString& remote){   
	m_ignoreNextButtonList << remote;
	m_ignoreNextButtonList.removeDuplicates();
    }
    
    void considerButtonEvents(const QString& remote){
      m_ignoreNextButtonList.removeAll(remote);
    }
    
    void clearIgnore(){
      m_remoteList.clear();
    }
};



KRemoteControlDaemon::KRemoteControlDaemon(QObject* parent, const QVariantList& ): KDEDModule(parent), d_ptr(new KRemoteControlDaemonPrivate)
{
Q_D(KRemoteControlDaemon);
  reloadConfiguration();
  foreach(const QString &remote, RemoteControl::allRemoteNames()){
        RemoteControl *rc = new RemoteControl(remote);
        kDebug() << "connecting to remote" << remote;
        connect(rc,
                SIGNAL(buttonPressed(const Solid::Control::RemoteControlButton &)),
                this,
                SLOT(gotMessage(const Solid::Control::RemoteControlButton &))
                );
    }
}


KRemoteControlDaemon::~KRemoteControlDaemon()
{

}


void KRemoteControlDaemon::gotMessage(const Solid::Control::RemoteControlButton& button)
{
    if(d_ptr->isButtonEventIgnored(button.name())){      
      return;
    }
    Remote *remote=   d_ptr->getRemote(button.name());
    if(! remote){
      return;
    }    
    if(remote->currentMode()){
      QList<Action*> actionList = remote->currentMode()->actionsForButton(button.name());      
      if(remote->isButtonModechange(button.name())){
	remote->nextMode(button.name());
	if(remote->currentMode() && remote->currentMode()-> doAfter()){
	  actionList.append(remote->currentMode()->actionsForButton(button.name()));
	}
      }
      foreach(Action *action, actionList){      
	 ExecutionEngine::executeAction(action);
      } 
  }
}


void KRemoteControlDaemon::reloadConfiguration()
{
  d_ptr->reload();
}

void KRemoteControlDaemon::changeMode(const QString& remoteName, Mode* mode) {
   Remote *remote=   d_ptr->getRemote(remoteName);        
    if(remote && remote->allModes().contains(mode)){
      remote->setCurrentMode(mode);
    }      
}

void KRemoteControlDaemon::ignoreButtonEvents(const QString& remote = QString()){
  if(remote.isEmpty()){
    foreach(Remote *remote, d_ptr->remoteList()){
      d_ptr->ignoreButtonEvents(remote->name());
    }
  }else{
    d_ptr->ignoreButtonEvents(remote);
  }  
}

void KRemoteControlDaemon::considerButtonEvents(const QString& remote){
  if(remote.isEmpty()){
    d_ptr->clearIgnore();
  }else{
    foreach(Remote *remote, d_ptr->remoteList()){
      d_ptr->considerButtonEvents(remote->name());
    }
  }
}
