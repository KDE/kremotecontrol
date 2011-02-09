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

#include "remotecontrolmanager.h"
#include "remotecontrolmanager_p.h"
#include "ifaces/remotecontrolmanagerinterface.h"
#include "ifaces/remotecontrolinterface.h"

#include <kglobal.h>
#include <kservicetypetrader.h>
#include <kservice.h>
#include <kdebug.h>

K_GLOBAL_STATIC(RemoteControlManagerPrivate, globalRemoteControlManager)

bool RemoteControlManager::connected()
{
    return globalRemoteControlManager->connected();
}

RemoteControlManager::Notifier* RemoteControlManager::notifier()
{
    return globalRemoteControlManager;
}


/***********************************************************************
  RemoteControlManagerPrivate
 ***********************************************************************/

RemoteControlManagerPrivate::RemoteControlManagerPrivate()
{
    loadBackends("KRemoteControlManager");
}

RemoteControlManagerPrivate::~RemoteControlManagerPrivate()
{
    while(!m_backendList.isEmpty()) {
        delete m_backendList.takeFirst();
    }
}

void RemoteControlManagerPrivate::loadBackends(const char *serviceName)
{
    QStringList error_msg;

    KService::List offers = KServiceTypeTrader::self()->query(serviceName, "(Type == 'Service')");

    foreach (const KService::Ptr &ptr, offers) {
        QString error_string;
        QObject *backend = ptr->createInstance<QObject>(0, QVariantList(), &error_string);

        if(backend!=0) {
            if(backend->inherits("Iface::RemoteControlManager")) {
                kDebug() << "Backend loaded: " << ptr->name();
                m_backendList.append(qobject_cast<Iface::RemoteControlManager*>(backend));
                break;
            } else {
                kDebug() << "Failed loading:" << error_string;
                QString error_string = i18n("Backend loaded but wrong type obtained, expected %1",
                                             QLatin1String("Iface::RemoteControlManager"));

                kDebug() << "Error loading '" << ptr->name() << "': " << error_string;
                error_msg.append(error_string);

                delete backend;
                backend = 0;
            }
        } else {
            kDebug() << "Error loading '" << ptr->name() << "', KService said: " << error_string;
            error_msg.append(error_string);
        }
    }

    if (m_backendList.isEmpty()) {
        if (offers.size() == 0) {
            kDebug() << "No Backend found";
        } else {
            kDebug() << "could not load any of the backends";
        }
    }
}

bool RemoteControlManagerPrivate::connected()
{
    return m_connected;
}

RemoteControlList RemoteControlManagerPrivate::buildDeviceList(const QStringList &remoteList)
{
    RemoteControlList list;

    foreach (const QString &remote, remoteList) {
        QPair<RemoteControl *, Iface::RemoteControl *> pair = findRegisteredRemoteControl(remote);

        if (pair.first!= 0) {
            list.append(pair.first);
        }
    }

    return list;

}

void RemoteControlManagerPrivate::_k_remoteControlAdded(const QString &name)
{
    Iface::RemoteControlManager *backendManager = qobject_cast<Iface::RemoteControlManager*>(sender());
    if(backendManager == 0) {
        return;
    }
    RemoteControl *rc = new RemoteControl(name);
    Iface::RemoteControl *rcBackend = backendManager->createRemoteControl(name);
    rc->d_ptr->setBackendObject(rcBackend);
    m_remoteControlMap.insert(name, QPair<RemoteControl*, Iface::RemoteControl*>(rc, rcBackend));

    emit remoteControlAdded(name);
}

void RemoteControlManagerPrivate::_k_remoteControlRemoved(const QString &name)
{
    delete m_remoteControlMap[name].first;
    delete m_remoteControlMap[name].second;
    m_remoteControlMap.remove(name);

    emit remoteControlRemoved(name);
}

void RemoteControlManagerPrivate::_k_statusChanged(bool connected)
{
    if(connected == m_connected) {
        return;
    }

    if(!connected) {
        // Is there still another backend connected?
        foreach(Iface::RemoteControlManager* backend, m_backendList) {
            if(backend->connected()) {
                return;
            }
        }
    }

    m_connected = connected;
    emit statusChanged(connected);
    kDebug() << "Remotecontrol backend status has changed to" << connected;
}

RemoteControlList RemoteControlManagerPrivate::allRemotes()
{
    QStringList remoteList;
    foreach(Iface::RemoteControlManager *backend, m_backendList) {
        remoteList.append(backend->remoteNames());
    }

    if (!m_backendList.isEmpty()) {
        return buildDeviceList(remoteList);
    } else {
        return RemoteControlList();
    }
}

RemoteControl* RemoteControlManagerPrivate::findRemoteControl(const QString &name)
{
    return m_remoteControlMap.value(name).first;
}

RemoteControl::RemoteControl(const QString &name): QObject(), d_ptr(new RemoteControlPrivate(this))
{
    Q_D(RemoteControl);

    RemoteControl *other = globalRemoteControlManager->findRemoteControl(name);

    if(other) {
        d->setBackendObject(other->d_ptr->backendObject());
    }
}

QList<RemoteControl*> RemoteControl::allRemotes()
{
    return globalRemoteControlManager->allRemotes();
}

QPair<RemoteControl *, Iface::RemoteControl *>
RemoteControlManagerPrivate::findRegisteredRemoteControl(const QString &remote)
{
    if (m_remoteControlMap.contains(remote)) {
        return m_remoteControlMap[remote];
    } else {
        foreach(Iface::RemoteControlManager *backend, m_backendList) {

            Iface::RemoteControl * iface = backend->createRemoteControl(remote);
            RemoteControl *device = 0;
            if(iface != 0) {
                device = new RemoteControl(iface);
            } else {
                kDebug() << "Unknown Remote: " << remote;
            }
            if (device != 0) {
                QPair<RemoteControl *, Iface::RemoteControl *> pair(device, iface);
                connect(dynamic_cast<QObject*>(iface), SIGNAL(destroyed(QObject *)),
                        this, SLOT(_k_destroyed(QObject *)));
                m_remoteControlMap[remote] = pair;
                return pair;
            } else {
                return QPair<RemoteControl *, Iface::RemoteControl *>(0, 0);
            }
        }
    }
    return QPair<RemoteControl *, Iface::RemoteControl *>(0, 0);
}

void RemoteControlManagerPrivate::_k_destroyed(QObject *object)
{
    Iface::RemoteControl *remote = qobject_cast<Iface::RemoteControl*>(object);
    if(remote) {
        QString name = remote->name();
        QPair<RemoteControl*, Iface::RemoteControl*> pair = m_remoteControlMap.take(name);
        delete pair.first;
    }
}
