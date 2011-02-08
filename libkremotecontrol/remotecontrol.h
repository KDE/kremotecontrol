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
#ifndef REMOTECONTROL_H
#define REMOTECONTROL_H

#include "remotecontrol_p.h"
#include "remotecontrolbutton.h"
#include "kremotecontrol_export.h"

#include <QtCore/QObject>
#include <QtCore/QStringList>


#include "kremotecontrol_export.h"

class RemoteControlPrivate;

class KREMOTECONTROL_EXPORT RemoteControl: public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(RemoteControl)

public:
    RemoteControl(const QString &name);

    /**
    * Get the Names of the available remotes in the system
    */
    static QStringList allRemoteNames();

    /**
    * Get all RemoteControl's available in the system
    */
    static QList<RemoteControl *> allRemotes();

    /**
     * Creates a new RemoteControl object.
     *
     * @param backendObject the RemoteControl object provided by the backend
     */
    explicit RemoteControl(Iface::RemoteControl *backendObject);

    /**
     * Constructs a copy of a remote control.
     *
     * @param remoteControl the remote control to copy
     */
    RemoteControl(const RemoteControl &remoteControl);

    /**
     * Destroys a RemoteControl object.
     */
    virtual ~RemoteControl();


    /**
     * Retrieves the name of the remote.
     *
     * @returns Returns the name of the remote control
     */
    QString name() const;

    /**
     * Retrieves the buttons of the remote.
     *
     * @returns Returns the buttons of the remote control
     */
    QList<RemoteControlButton> buttons() const;

Q_SIGNALS:
    /**
     * This signal is emitted when button on the remote is pressed
     *
     * @param button The RemoteControlButton pressed
     */
    void buttonPressed(const RemoteControlButton &button);

protected:
    /**
     * @internal
     */
    RemoteControl(RemoteControlPrivate &dd, QObject *backendObject);

    /**
     * @internal
     */
    RemoteControl(RemoteControlPrivate &dd, const RemoteControl &name);

    RemoteControlPrivate *d_ptr;
    friend class RemoteControlManagerPrivate;
};

typedef QList<RemoteControl *> RemoteControlList;

#endif // REMOTECONTROL_H
