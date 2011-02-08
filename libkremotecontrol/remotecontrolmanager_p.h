/*
    Copyright (C) <2011>  Michael Zanetti <mzanetti@kde.org>

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

#ifndef REMOTECONTROLMANAGER_P_H
#define REMOTECONTROLMANAGER_P_H

#include <QObject>
#include <QMap>
#include <QPair>

#include "ifaces/remotecontrolmanagerinterface.h"
#include "remotecontrolmanager.h"
#include "remotecontrol.h"

class RemoteControlManagerPrivate : public RemoteControlManager::Notifier
{
    Q_OBJECT
public:
    RemoteControlManagerPrivate();
    ~RemoteControlManagerPrivate();

    RemoteControlList allRemotes();
    RemoteControl *findRemoteControl(const QString &name);

    bool connected();

private Q_SLOTS:
    void _k_remoteControlAdded(const QString &name);
    void _k_remoteControlRemoved(const QString &name);
    void _k_statusChanged(bool connected);
    void _k_destroyed(QObject *object);

private:
    void loadBackends(const char *serviceName);
    bool m_connected;
    QList<Iface::RemoteControlManager*> m_backendList;

    RemoteControlList buildDeviceList(const QStringList &remoteList);
    QPair<RemoteControl *, Iface::RemoteControl *> findRegisteredRemoteControl(const QString &name);

    QMap<QString, QPair<RemoteControl *, Iface::RemoteControl *> > m_remoteControlMap;
//    RemoteControl m_invalidDevice;
};

#endif
