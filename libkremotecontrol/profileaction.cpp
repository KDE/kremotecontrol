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

#include "profileaction.h"
#include "profileserver.h"

ProfileAction::ProfileAction(): DBusAction() {
    //change type from DBusAction (c'tor) to ProfileAction
    m_type = Action::ProfileAction;
}

ProfileAction::ProfileAction(const QString& button, const QString &profileName, const QString &actionTemplate): DBusAction(button) {
    //change type from DBusAction (c'tor) to ProfileAction
    m_type = Action::ProfileAction;
    m_profileId = profileName;
    m_template = actionTemplate;
}

QString ProfileAction::profileId() const {
    return m_profileId;
}

void ProfileAction::setProfileId(const QString& profileId) {
    m_profileId = profileId;
}

QString ProfileAction::actionTemplateId() const {
    return m_template;
}

void ProfileAction::setActionTemplateId(const QString& actionTemplateId) {
    m_template = actionTemplateId;
}

QString ProfileAction::name() const {
    foreach(const Profile *profile, ProfileServer::allProfiles()){
        if(profile->profileId() == m_profileId){
            return profile->name();
        }
    }
    return m_profileId;
}

QString ProfileAction::description() const {
    foreach(const Profile *profile, ProfileServer::allProfiles()){
        if(profile->profileId() == m_profileId){
            foreach(const ProfileActionTemplate &actionTemplate, profile->actionTemplates()){
                if(actionTemplate.actionTemplateId() == m_template && actionTemplate.profileId() == m_profileId){
                    return actionTemplate.actionName();
                }
            }
        }
    }
    return m_function.name();
}

void ProfileAction::saveToConfig(KConfigGroup &config) {
    DBusAction::saveToConfig(config);
    config.writeEntry("ProfileId", m_profileId);
    config.writeEntry("Template", m_template);
}

void ProfileAction::loadFromConfig(const KConfigGroup &config) {
    DBusAction::loadFromConfig(config);
    m_profileId = config.readEntry("ProfileId");
    m_template = config.readEntry("Template");
}

Action* ProfileAction::clone() const {
    ProfileAction *action = new ProfileAction();
    action->setActionTemplateId(m_template);
    action->setProfileId(m_profileId);
    action->setApplication(m_application);
    action->setAutostart(m_autostart);
    action->setDestination(m_destination);
    action->setFunction(m_function);
    action->setNode(m_node);
    action->setRepeat(m_repeat);
    action->setButton(m_button);
    return action;
}
