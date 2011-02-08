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

#ifndef REMOTECONTROLIFACE_H
#define REMOTECONTROLIFACE_H

#include <QObject>

#include "../remotecontrol.h"

namespace Iface
{

class KREMOTECONTROL_EXPORT RemoteControl
{
public:
    virtual ~RemoteControl() {}

    virtual QString name() const = 0;

    virtual QList<RemoteControlButton> buttons() const = 0;


protected:
Q_SIGNALS:
    void buttonPressed(const RemoteControlButton &button);
};
}

Q_DECLARE_INTERFACE(Iface::RemoteControl, "org.kde.KRemoteControl.Ifaces.RemoteControl/0.1")

#endif // REMOTECONTROLIFACE_H
