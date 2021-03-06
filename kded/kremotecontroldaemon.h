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
#include "remote.h"
#include "remotelist.h"

#include "remotecontrolmanager.h"
#include "remotecontrol.h"

#include <KDEDModule>
#include <KComponentData>
#include <KNotification>

#include <QtCore/QVariant>
#include <QtCore/QPointer>
#include <QtCore/QTimer>

class KRemoteControlDaemonPrivate;

class KRemoteControlDaemon : public KDEDModule
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.krcd")

    private:
        void notifyModeChanged(Remote* remote);
        void notifyEvent(const QString& message, const QString& icon = QLatin1String( "infrared-remote" ), const QString& event = QLatin1String ( "global_event" ));

        RemoteList m_remoteList;
        QStringList m_ignoreNextButtonList;
        KComponentData m_applicationData;
        QPointer<KNotification> m_notification;
        QTimer m_modeSwitchTimer;

    public:
        KRemoteControlDaemon(QObject * parent, const QVariantList&);
        virtual ~KRemoteControlDaemon();

        bool isConnected() {
            return RemoteControlManager::connected();
        }
        void reloadConfiguration();

        void ignoreButtonEvents(const QString &remoteName);
        void considerButtonEvents(const QString& remoteName);
        void changeMode(const QString& remoteName, Mode* mode);
        bool changeMode(const QString& remoteName, const QString &modeName);
        QStringList configuredRemotes();
        QStringList modesForRemote(const QString &remoteName);
        QString currentMode(const QString& remoteName);
        QString modeIcon(const QString &remoteName, const QString &modeName);
        bool eventsIgnored(const QString& remoteName);

    Q_SIGNALS:
        void connectionChanged(bool connected);
        void remoteControlAdded(const QString &remote);
        void remoteControlRemoved(const QString &remote);
        void modeChanged(const QString &remoteName, const QString &modeName);
        void buttonPressed();
        void unloadTray();

    private Q_SLOTS:
        void lauchKcmShell();
        void slotStatusChanged(bool connected);
        void slotRemoteControlAdded(const QString &name);
        void slotRemoteControlRemoved(const QString &name);
        void gotMessage(const RemoteControlButton &button);

};
#endif // KREMOTECONTROLDAEMON_H
