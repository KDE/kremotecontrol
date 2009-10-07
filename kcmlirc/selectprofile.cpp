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

#include "selectprofile.h"
#include "dbusinterface.h"
#include <QTreeWidgetItem>

SelectProfileWidget::SelectProfileWidget (QWidget *parent) : QWidget(parent){
     QTreeWidget *theProfiles = new QTreeWidget();
     profilesWidget = new QTreeWidget();
//     setHeaderLabel(i18n("Select a profile"));
//     setRootIsDecorated(false);
//     setMainWidget(theProfiles);
//     setWindowTitle(i18n("Auto-Populate"));

}

SelectProfile::SelectProfile(QString remoteName, QWidget *parent, const bool &modal): KDialog(parent)
{
    selectProfileWidget = new SelectProfileWidget;

    setMainWidget(selectProfileWidget);
    setButtons( Ok | Cancel);
    setDefaultButton(Ok);
    setModal(modal);
    QList<Profile*> profiles = ProfileServer::getInstance()->profiles();
    foreach(Profile* profile, profiles) {
        ProfileServer::ProfileSupportedByRemote tSupported;
        tSupported = ProfileServer::getInstance()->isProfileAvailableForRemote(profile, DBusInterface::getInstance()->getButtons(remoteName));
        if (! tSupported == ProfileServer::NO_ACTIONS_DEFINED) {
            ProfileWrapper wrapper = ProfileWrapper(profile, tSupported);
            QTreeWidgetItem* tTreewidget = new QTreeWidgetItem(selectProfileWidget->profilesWidget);
            tTreewidget->setText(0, profile->name());
            tTreewidget->setData(0,Qt::DisplayRole,qVariantFromValue<ProfileWrapper>(wrapper));
            KIcon tIcon;
            switch (tSupported) {
            case ProfileServer::FULL_SUPPORTED :
                tIcon = KIcon("flag-green");
                break;
            case ProfileServer::PARTIAL_SUPPORTED :
                tIcon = KIcon("flag-yellow");
                break;
            default:
                tIcon = KIcon("flag-red");
            }
            tTreewidget->setIcon(0, tIcon);
        }

    }

}


const Profile* SelectProfile::getSelectedProfile()
{
    return selectProfileWidget->profilesWidget->currentItem()->data(0, Qt::UserRole).value<ProfileWrapper>().getProfile();
}

