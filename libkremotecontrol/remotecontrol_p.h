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

#ifndef REMOTECONTROL_P_H
#define REMOTECONTROL_P_H

#include <QObject>

namespace Iface
{
    class RemoteControl;
}

class RemoteControlPrivate
{
public:
    explicit RemoteControlPrivate(QObject *parent);
    ~RemoteControlPrivate();

    void setBackendObject(Iface::RemoteControl *object);

    Iface::RemoteControl *backendObject() const;

    QObject *parent() const;

private:
    QObject *m_parent;
    Iface::RemoteControl *m_backendObject;
};

#endif // REMOTECONTROL_P_H
