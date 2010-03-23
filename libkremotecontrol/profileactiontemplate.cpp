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

#include "profileactiontemplate.h"


ProfileActionTemplate::ProfileActionTemplate( const QString &profileId,
                                              const QString &actionTemplateId,
                                              const QString &actionName,
                                              const QString &serviceName,
                                              const QString &node,
                                              const Prototype &function,
                                              const ProfileAction::ActionDestination destination,
                                              bool autostart,
                                              bool repeat,
                                              const QString &description,
                                              const QString &buttonName)
{
    d = new ProfileActionTemplatePrivate;
    d->m_profileId = profileId;
    d->m_actionTemplateId = actionTemplateId;
    d->m_actionName = actionName;
    d->m_node = node;
    d->m_serviceName = serviceName;
    d->m_function = function;
    d->m_description = description;
    d->m_destination= destination;
    d->m_autostart = autostart;
    d->m_repeat = repeat;
    d->m_buttonName = buttonName;
}

QString ProfileActionTemplate::profileId() const {
    return d->m_profileId;
}

QString ProfileActionTemplate::actionTemplateId() const {
    return d->m_actionTemplateId;
}

QString ProfileActionTemplate::actionName() const {
    return d->m_actionName;
}

QString ProfileActionTemplate::service() const {
    return d->m_serviceName;
}

QString ProfileActionTemplate::node() const {
    return d->m_node;
}

QString ProfileActionTemplate::description() const {
    return d->m_description;
}

Prototype ProfileActionTemplate::function() const {
    return d->m_function;
}

DBusAction::ActionDestination ProfileActionTemplate::destination() const {
    return d->m_destination;
}

bool ProfileActionTemplate::autostart() const {
    return d->m_autostart;
}

bool ProfileActionTemplate::repeat() const {
    return d->m_repeat;
}

QString ProfileActionTemplate::buttonName() const {
    return d->m_buttonName;
}

ProfileAction *ProfileActionTemplate::createAction(const Solid::Control::RemoteControlButton& button) const {
    ProfileAction *action = new ProfileAction(button.name(), d->m_profileId, d->m_actionTemplateId);
    action->setApplication(d->m_serviceName);
    action->setNode(d->m_node);
    action->setFunction(d->m_function);
    action->setDestination(d->m_destination);
    action->setAutostart(d->m_autostart);
    action->setRepeat(d->m_repeat);
    return action;
}
