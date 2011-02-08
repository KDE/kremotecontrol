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

#include "remotecontrolbutton.h"

class ProfileActionTemplatePrivate;

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

        ProfileActionTemplate();

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

        ProfileAction *createAction(const RemoteControlButton &button) const;
        
    private:
        QSharedDataPointer<ProfileActionTemplatePrivate> d;

};

class ProfileActionTemplatePrivate: public QSharedData
{
    public:
        ProfileActionTemplatePrivate(){};
        
        ProfileActionTemplatePrivate(const ProfileActionTemplatePrivate &other): QSharedData(other),
                                      m_profileId(other.m_profileId),
                                      m_actionTemplateId(other.m_actionTemplateId),
                                      m_actionName(other.m_actionName),
                                      m_serviceName(other.m_serviceName),
                                      m_node(other.m_node),
                                      m_function(other.m_function),
                                      m_description(other.m_description),
                                      m_destination(other.m_destination),
                                      m_autostart(other.m_autostart),
                                      m_repeat(other.m_repeat),
                                      m_buttonName(other.m_buttonName){};
                              
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
