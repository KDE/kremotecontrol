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

class KDELIRC_EXPORT ProfileActionTemplate
{
  public:
    ProfileActionTemplate(const QString &profile,
			  const QString &templateID,
			  const QString &application,
			  const QString &appName,
			  const QString &node,
			  const Prototype &function,
			  const QString &description,
			  const QList<NewArgument> &arguments,
			  const NewProfileAction::ActionDestination,
			  bool autostart,
			  bool repeat,
			  const QString &buttonName = QString());
			  
    ProfileActionTemplate() {};

    QString profile() const;
    QString templateID() const;
    QString appName() const;
    QString description() const;
    QList<NewArgument> defaultArguments() const;
    NewProfileAction::ActionDestination destination() const;
    bool autostart() const;
    bool repeat() const;
    QString buttonName() const;
    
    NewProfileAction *createAction(const Solid::Control::RemoteControlButton &button) const;
    
  protected:
    QString m_profile; // e.g. Multimedia
    QString m_templateID; // e.g. ??? define in profile.xml or autogenerate in readFromConfig?
    QString m_application; // e.g. org.kde.amarok
    QString m_node; // e.g. Player
    QString m_appName; // e.g. Amarok
    Prototype m_function;
    QString m_description; // e.g. Amarok Music Player
    QList<NewArgument> m_defaultArguments;
    NewProfileAction::ActionDestination m_destination;
    bool m_autostart;
    bool m_repeat;
    QString m_buttonName;

};

#endif // ACTIONTEMPLATE_H
