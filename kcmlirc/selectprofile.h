/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef SELECTPROFILE_H
#define SELECTPROFILE_H

#include "profileserver.h"
#include <QtGui/QWidget>
#include <QMap>
#include <QPair>
#include "kicondialog.h"
#include <QTreeWidget>
#include <qlabel.h>




class SelectProfileWidget : public QWidget
{
private:
  
public:
    QTreeWidget *profilesWidget;
    QLabel *selectionLabel;
    
    SelectProfileWidget (QWidget *parent = 0);
    
};


class SelectProfile : public KDialog
{
    Q_OBJECT

  private:
    SelectProfileWidget *selectProfileWidget;
    
    
  public:	
    SelectProfile(QString remoteName, QWidget *parent = 0, const bool &modal = false);
    const Profile *getSelectedProfile();
    
};

class ProfileWrapper 
{
  private:
    Profile *profile;
    ProfileServer::ProfileSupportedByRemote supported;
  
  public:
  ProfileWrapper(){
  qRegisterMetaType<ProfileWrapper>("ProfileWrapper");
  };
    
    ProfileWrapper( Profile * pProfile, ProfileServer::ProfileSupportedByRemote pSupported){
      ProfileWrapper();
      
      profile = pProfile;
      supported = pSupported;
    };
    
    const Profile *getProfile() {
      return profile;
    };
    
     ProfileServer::ProfileSupportedByRemote getSupported() {
      return supported;
    };
};

Q_DECLARE_METATYPE(ProfileWrapper)

#endif // SELECTPROFILE_H
