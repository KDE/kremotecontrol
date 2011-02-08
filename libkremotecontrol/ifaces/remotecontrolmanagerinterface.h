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

#ifndef REMOTECONTROLMANAGERIFACE
#define REMOTECONTROLMANAGERIFACE

#include <QtCore/QObject>
#include <QtCore/QStringList>

#include "remotecontrolinterface.h"

#include "../remotecontrolmanager.h"

namespace Iface
{
/**
 * This class specifies the interface a backend will have to implement in
 * order to be used in the system.
 *
 * A remote control manager allows to query the underlying platform to discover the
 * available remote controls and their buttons.
 */
class KREMOTECONTROL_EXPORT RemoteControlManager: public QObject
{
    Q_OBJECT

public:
 /**
     * Constructs a RemoteControl.
     *
     * @param parent the parent object
     */
    RemoteControlManager(QObject * parent = 0);
    /**
     * Destructs a RemoteControl object.
     */
    virtual ~RemoteControlManager();

    /**
     * Get the manager connection state
     */
    virtual bool connected() const = 0;

    /**
     * Retrieves the list of all the remotes installed
     * in the system.
     *
     * @return the list of remote controls available in this system
     */
    virtual QStringList remoteNames() const = 0;

    /**
     * Instantiates a new RemoteControlInterface object from this backend given its remote.
     *
     * @param name the identifier of the remote control interface instantiated
     * @returns a new RemoteControl object if there's a device having the given remote, 0 otherwise
     */
    virtual Iface::RemoteControl *createRemoteControl(const QString &name) = 0;

Q_SIGNALS:
    /**
    * This signal is emitted when a new remote control is available.
    *
    * @param name the name of the RemoteControl
    */
    void remoteControlAdded(const QString &name);

    /**
    * This signal is emitted when a remote control is not available anymore.
    *
    * @param name the name of the RemoteControl
    */
    void remoteControlRemoved(const QString &name);

    /**
     * This signal is emitted when the system's connection state changes
     */
    void statusChanged(bool connected);

};

}
Q_DECLARE_INTERFACE(Iface::RemoteControlManager, "org.kde.KRemoteControl.Ifaces.RemoteControlManager/0.1")

#endif // REMOTECONTROLMANAGERIFACE
