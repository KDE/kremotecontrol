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
#include "kremotecontrol_export.h"

#include <solid/control/remotecontrolbutton.h>

class KREMOTECONTROL_EXPORT ProfileActionTemplate
{
  public:
    ProfileActionTemplate(const QString &profile,
			  const QString &actionTemplateID,
			  const QString &application,
			  const QString &appName,
			  const QString &node,
			  const QString &function,
			  const QList<Argument> &arguments,
			  const ProfileAction::ActionDestination,
			  bool autostart,
			  bool repeat,
			  const QString &description = QString(),
			  const QString &buttonName = QString());
			  
    ProfileActionTemplate() {};

    QString profile() const;
    QString actionTemplateID() const;
    QString appName() const;
    QString description() const;
    QList<Argument> defaultArguments() const;
    ProfileAction::ActionDestination destination() const;
    bool autostart() const;
    bool repeat() const;
    QString buttonName() const;
    
    ProfileAction *createAction(const Solid::Control::RemoteControlButton &button, const Mode &mode) const;
    
  protected:
    QString m_profile; // e.g. Multimedia
    QString m_actionTemplateID; // e.g. multimedia-amarok-1
    QString m_application; // e.g. org.kde.amarok
    QString m_node; // e.g. Player
    QString m_appName; // e.g. Amarok
    QString m_function; // play
    QString m_description; // e.g. Amarok Music Player
    QList<Argument> m_defaultArguments;
    ProfileAction::ActionDestination m_destination;
    bool m_autostart;
    bool m_repeat;
    QString m_buttonName;

};

#endif // ACTIONTEMPLATE_H
