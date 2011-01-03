/*
        Copyright (C) <2009>  <Frank Scheffold (fscheffold@googlemail.com)>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License as
	published by the Free Software Foundation; either version 2 of
	the License or (at your option) version 3 or any later version
	accepted by the membership of KDE e.V. (or its successor approved
	by the membership of KDE e.V.), which shall act as a proxy 
	defined in Section 14 of version 3 of the license.

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
#include <QtGui/QTreeWidget>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>

#include <KTextEdit>
#include <KDialog>

class SelectProfileWidget : public QWidget
{
    public:
        QTreeWidget *profilesWidget;
        QLabel *selectionLabel;
        QVBoxLayout *layout;
        
        SelectProfileWidget (QWidget *parent = 0);
    
};


class SelectProfile : public KDialog
{
    Q_OBJECT

    private:
        SelectProfileWidget *selectProfileWidget;
      
      
    public:
        explicit SelectProfile(Remote *remote, QWidget *parent = 0, const bool &modal = false);
        Profile *getSelectedProfile();
      
    public Q_SLOTS:
        void checkForUpdate(QTreeWidgetItem*, int);
};

class ProfileWrapper 
{
    private:
        Profile *profile;
        ProfileServer::ProfileSupportedByRemote supported;
      
    public:
        ProfileWrapper()
		: profile(0)
	{
            qRegisterMetaType<ProfileWrapper>("ProfileWrapper");
        }
        
        ProfileWrapper( Profile * pProfile, ProfileServer::ProfileSupportedByRemote pSupported){
            ProfileWrapper();
        
            profile = pProfile;
            supported = pSupported;
        }
        
        Profile *getProfile() {
            return profile;
        }
        
        ProfileServer::ProfileSupportedByRemote getSupported() const {
            return supported;
        }
};

Q_DECLARE_METATYPE(ProfileWrapper)

#endif // SELECTPROFILE_H
