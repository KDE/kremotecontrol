/*
    Copyright (C) 2010  Michael Zanetti <michael_zanetti@gmx.net>

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

#ifndef ACTIONTEMPLATE_H
#define ACTIONTEMPLATE_H

#include "profileaction.h"
#include "prototype.h"
#include "kremotecontrol_export.h"

#include <solid/control/remotecontrolbutton.h>

class KREMOTECONTROL_EXPORT ProfileActionTemplate
{
    public:
        ProfileActionTemplate(const QString &profile,
                              const QString &actionTemplateID,
                              const QString &actionName,
                              const QString &serviceName,
                              const QString &node,
                              const Prototype &function,
                              const ProfileAction::ActionDestination,
                              bool autostart,
                              bool repeat,
                              const QString &description = QString(),
                              const QString &buttonName = QString());

        ProfileActionTemplate() {};

        QString profileId() const;
        QString actionTemplateId() const;
        QString actionName() const;
        QString service() const;
        QString node() const;
        Prototype function() const;
        QString description() const;
        ProfileAction::ActionDestination destination() const;
        bool autostart() const;
        bool repeat() const;
        QString buttonName() const;

        ProfileAction *createAction(const Solid::Control::RemoteControlButton &button) const;

    protected:
        QString m_profileId; // e.g. multimedia
        QString m_actionTemplateId; // e.g. amarokPlay
        QString m_actionName; // e.g. Play
        QString m_serviceName; // e.g. org.kde.amarok
        QString m_node; // e.g. Player
        Prototype m_function; // play
        QString m_description; // e.g. Start playing
        ProfileAction::ActionDestination m_destination;
        bool m_autostart;
        bool m_repeat;
        QString m_buttonName;

};

Q_DECLARE_METATYPE(ProfileActionTemplate)

#endif // ACTIONTEMPLATE_H
