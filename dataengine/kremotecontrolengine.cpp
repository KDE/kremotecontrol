/*
 *   Copyright 2010 Frank Scheffold <fscheffold@googlemail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "kremotecontrolengine.h"

#include <QtCore/QDate>
#include <QtCore/QTime>

#include <KSystemTimeZones>
#include <KDateTime>

#include <Plasma/DataContainer>
#include <dbusinterface.h>

KRemoteControlEngine::KRemoteControlEngine(QObject* parent, const QVariantList& args) : Plasma::DataEngine(parent, args) {
    // We ignore any arguments - data engines do not have much use for them
    Q_UNUSED(args)

    // This prevents applets from setting an unnecessarily high
    // update interval and using too much CPU.
    // In the case of a clock that only has second precision,
    // a third of a second should be more than enough.
    setMinimumPollingInterval(333);
}

bool KRemoteControlEngine::sourceRequestEvent(const QString &name) {
    // We do not have any special code to execute the
    // first time a source is requested, so we just call
    // updateSourceEvent().
    return updateSourceEvent(name);
}

bool KRemoteControlEngine::updateSourceEvent(const QString &name) {
    Q_UNUSED(name)
    QStringList remotes = DBusInterface::getInstance()->configuredRemotes();
    setData("remotes", remotes);
    kDebug() << "remotes" << remotes;
    foreach(const QString &remote, remotes) {
        setData(remote, "modes", DBusInterface::getInstance()->modesForRemote(remote));
        setData(remote, "currentMode", DBusInterface::getInstance()->currentMode(remote));
        setData(remote, "eventsIgnored", DBusInterface::getInstance()->eventsIgnored(remote));
    }

    return true;
}

// This does the magic that allows Plasma to load
// this plugin.  The first argument must match
// the X-Plasma-EngineName in the .desktop file.
K_EXPORT_PLASMA_DATAENGINE(kremotecontrol, KRemoteControlEngine)

// this is needed since TestTimeEngine is a QObject
#include "kremotecontrolengine.moc"
