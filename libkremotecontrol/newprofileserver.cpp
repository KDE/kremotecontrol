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

#include "newprofileserver.h"
#include "profileactiontemplate.h"


#include <kdebug.h>
#include <kglobal.h>
#include "modeswitchaction.h"
#include "actionlist.h"
#include "executionengine.h"

class NewProfileServerPrivate
{
  public:
    NewProfileServerPrivate();
    QList<NewProfile> m_allProfiles;
};

K_GLOBAL_STATIC(NewProfileServerPrivate, instance)

NewProfileServerPrivate::NewProfileServerPrivate()
{
//load all profiles from config here...



}

void NewProfileServer::addProfile(const NewProfile& profile) {
  instance->m_allProfiles.append(profile);
}

QList< NewProfile > NewProfileServer::allProfiles() {
  return instance->m_allProfiles;
}

NewProfile NewProfileServer::profile(const QString& profileName) {
  foreach(const NewProfile &profile, instance->m_allProfiles){
    if(profile.name() == profileName){
      return profile;
    }
  }
  kDebug() << "Warning: profile" << profileName << "not found. Creating empty one.";
  return NewProfile(profileName);
}

QList< ProfileActionTemplate > KDELIRC_EXPORT NewProfileServer::actionTemplateList(const QString& remote, const NewProfile& profile) {
  QList<ProfileActionTemplate> retList;
  foreach(const ProfileActionTemplate &actionTemplate, profile.actionTemplates()){
    kDebug() << "got template" << actionTemplate.templateID() << "with button" << actionTemplate.buttonName();
    foreach(const Solid::Control::RemoteControlButton &button, Solid::Control::RemoteControl(remote).buttons()){
      kDebug() << "got button" << button.name();
      if(button.name() == actionTemplate.buttonName()){
	retList.append(actionTemplate);
      }
    }
  }
  return retList;
}

ProfileActionTemplate NewProfileServer::actionTemplate(const NewProfileAction* action) {
  return profile(action->profileName()).actionTemplate(action->actionTemplateID());
}
