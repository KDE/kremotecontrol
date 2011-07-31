/*************************************************************************
 * Copyright (C) 2011 by Michael Zanetti <mzanetti@gmx.net>              *
 *                                                                       *
 * This program is free software; you can redistribute it and/or         *
 * modify it under the terms of the GNU General Public License as        *
 * published by the Free Software Foundation; either version 2 of        *
 * the License or (at your option) version 3 or any later version        *
 * accepted by the membership of KDE e.V. (or its successor approved     *
 * by the membership of KDE e.V.), which shall act as a proxy            *
 * defined in Section 14 of version 3 of the license.                    *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *************************************************************************/

#include "remotecontrol.h"

#include "ifaces/remotecontrolinterface.h"

RemoteControl::RemoteControl(Iface::RemoteControl *backendObject)
    : QObject(), d_ptr(new RemoteControlPrivate(this))
{
    Q_D(RemoteControl);
    d->setBackendObject(backendObject);
}

RemoteControl::~RemoteControl()
{

}

QStringList RemoteControl::allRemoteNames()
{
    QStringList retList;
    foreach(RemoteControl* rc, allRemotes()) {
        retList.append(rc->name());
    }
    return retList;
}

QString RemoteControl::name() const
{
    return d_ptr->backendObject()->name();
}

QList<RemoteControlButton> RemoteControl::buttons() const
{
    return d_ptr->backendObject()->buttons();
}


RemoteControlPrivate::RemoteControlPrivate(QObject *parent):
    m_parent(parent)
{

}

void RemoteControlPrivate::setBackendObject(Iface::RemoteControl *object)
{
    m_backendObject = object;
    if (object) {
        QObject *tmpObj = dynamic_cast<QObject*>(object);
        QObject::connect(tmpObj, SIGNAL(buttonPressed(RemoteControlButton)),
                         parent(), SIGNAL(buttonPressed(RemoteControlButton)));
    }

}

Iface::RemoteControl* RemoteControlPrivate::backendObject() const
{
    return m_backendObject;
}

QObject *RemoteControlPrivate::parent() const
{
    return m_parent;
}
