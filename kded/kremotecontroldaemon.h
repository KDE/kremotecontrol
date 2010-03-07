/*
    Copyright (C) <2010>  <Frank Scheffold (fscheffold@googlemail.com)>

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

#ifndef KREMOTECONTROLDAEMON_H
#define KREMOTECONTROLDAEMON_H


#include "mode.h"

#include <kdelirc/libkremotecontrol/remote.h>
#include <solid/control/remotecontrolmanager.h>
#include <solid/control/remotecontrol.h>

#include <KDEDModule>
#include <kstatusnotifieritem.h>
#include <QVariant>

class KRemoteControlDaemonPrivate;

class KRemoteControlDaemon : public KDEDModule
{
Q_OBJECT
Q_CLASSINFO("D-Bus Interface", "org.kde.krcd")
Q_DECLARE_PRIVATE(KRemoteControlDaemon)

    private:
        KRemoteControlDaemonPrivate * d_ptr;
        void notifyModeChanged(Remote* remote);
        void notifyEvent(const QString& message, const QString& icon = QString ( "infrared-remote" ), const QString& event = QString ( "global_event" ));
    
    public:
        KRemoteControlDaemon(QObject * parent, const QVariantList&);
        virtual ~KRemoteControlDaemon();
      
        bool isConnected() {
            return Solid::Control::RemoteControlManager::connected();
        }
        void reloadConfiguration();	
        
        void ignoreButtonEvents(const QString &remoteName);
        void considerButtonEvents(const QString& remoteName);
        void changeMode(const QString& remoteName, Mode* mode);    
        bool changeMode(const QString& remoteName, const QString &modeName);
        QStringList getConfiguredRemotes();
        QStringList getModesForRemote(const QString &remoteName);
        QString getCurrentMode(const QString& remoteName);
        QString getModeIcon(const QString &remoteName, const QString &modeName);
        bool eventsIgnored(const QString& remoteName);

    
    
    signals:
        void connectionChanged(bool connected);
        void remoteControlAdded(const QString &remote);
        void remoteControlRemoved(const QString &remote);
        void modeChanged(const QString &remoteName, const QString &modeName);
        void buttonPressed();
        void unloadTray();
      
    private slots:
        void lauchKcmShell();       
        void slotStatusChanged(bool connected);    
        void slotRemoteControlAdded(const QString &name);                        
        void slotRemoteControlRemoved(const QString &name);   
        void gotMessage(const Solid::Control::RemoteControlButton &button);

};
#endif // KREMOTECONTROLDAEMON_H
