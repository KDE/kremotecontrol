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
#include<KNotification>
#include <KAboutData>
#include <KIconLoader>

#include<QPixmap>

using namespace Solid::Control;

K_PLUGIN_FACTORY(KRemoteControlDaemonFactory,
                 registerPlugin<KRemoteControlDaemon>();
    )
K_EXPORT_PLUGIN(KRemoteControlDaemonFactory("kremotecontrol"))

class KRemoteControlDaemonPrivate
{
  private:
   RemoteList m_remoteList;    
     QStringList m_ignoreNextButtonList;

  public:
    
    QWeakPointer<KNotification> notification;
//     QTimer *lookupTimer;
    KComponentData applicationData;


    KRemoteControlDaemonPrivate(){};
 
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
  KGlobal::locale()->insertCatalog("krcd");
  KAboutData aboutData("krcd", "krcd", ki18n("K Remote Control Daemon"),
                         "0.1", ki18n("Remote Control Daemon for KDE4"),
                         KAboutData::License_GPL, ki18n("(c) 2010 Frank Scheffold"),
                         KLocalizedString(), "http://www.kde.org");

  aboutData.addAuthor(ki18n("Frank Scheffold"), ki18n("Maintainer"), "fscheffold@googlemail.com");
  aboutData.addAuthor(ki18n("Michael Zanetti"), ki18n("Maintainer"), "michal_zanetti@gmx.net");

  d->applicationData = KComponentData(aboutData);
//   d->lookupTimer = new QTimer(this);
 connect(RemoteControlManager::notifier(), SIGNAL(statusChanged(bool)), this, SLOT(slotStatusChanged(bool)));
  reloadConfiguration();
  foreach(const QString &remote, RemoteControl::allRemoteNames()){
        RemoteControl *rc = new RemoteControl(remote);
        kDebug() << "connecting to remote" << remote;
        connect(rc,
                SIGNAL(buttonPressed(const Solid::Control::RemoteControlButton &)),
                this,  SLOT(gotMessage(const Solid::Control::RemoteControlButton &)));
      }
}

KRemoteControlDaemon::~KRemoteControlDaemon()
{

}


void KRemoteControlDaemon::slotStatusChanged(bool connected) {
  if(connected){
     KNotification::event("global_event", i18n("A connection to the remote control  subsystem has been made. Remote controls may now be available."),  SmallIcon("irkick"));
    foreach(const QString &remote, RemoteControl::allRemoteNames()){
        RemoteControl *rc = new RemoteControl(remote);
        kDebug() << "connecting to remote" << remote;
        connect(rc,
		
		
                SIGNAL(buttonPressed(const Solid::Control::RemoteControlButton &)),
                this,
                SLOT(gotMessage(const Solid::Control::RemoteControlButton &)));
    }
    } else {
      KNotification::event("global_event", i18n("The remote control subsystem  has severed its connection. Remote controls are no longer available."),
			   SmallIcon("irkick"));
    }
}


void KRemoteControlDaemon::gotMessage(const Solid::Control::RemoteControlButton& button) {
  kDebug()<< "Got message";
  Remote *remote=   d_ptr->getRemote(button.name());
  if(! remote){
    return;
  }
    if(d_ptr->isButtonEventIgnored(remote->name())){      
    kDebug() << "Events for  "<< remote->name() <<    " are currently is ignored";
    return;
  }
  //This is for debugging purposes, till we got our tray icon back
  KNotification::event("mode_event",
	"<b>" + remote->name() + ":</b><br>" + i18n("Button %1 pressed" , button.name()),
	DesktopIcon("infrared-remote"));      
  if(remote->currentMode()){
    QList<Action*> actionList = remote->currentMode()->actionsForButton(button.name());      
    if(remote->nextMode(button.name())){
      Mode *mode = remote->currentMode();
      
      KNotification::event("mode_event", 
	"<b>" + remote->name() + ":</b><br>" + i18n("Mode switched to %1" , mode->name()),
	DesktopIcon(mode->iconName().isEmpty() ? "infrared-remote" : mode->iconName())
      );
      
      if(remote->currentMode()-> doAfter()){
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
