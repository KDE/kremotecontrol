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

#include "selectprofile.h"

#include <klocale.h>
#include <kdebug.h>

#include <QTreeWidgetItem>

SelectProfileWidget::SelectProfileWidget (QWidget *parent) : QWidget(parent) {
    selectionLabel = new QLabel();
    selectionLabel->setWordWrap(true);

    profilesWidget = new QTreeWidget();
    layout = new QVBoxLayout(this);
    QLabel *headerLabel = new QLabel(i18n("Select a profile to automatically generate actions for your remote control:"));
    headerLabel->setWordWrap(true);
    layout->addWidget(headerLabel);
    profilesWidget->setHeaderLabel(i18n("Available profiles"));
    layout->addWidget(profilesWidget);
    layout->addWidget(selectionLabel);
    
}

SelectProfile::SelectProfile(Remote *remote, QWidget *parent, const bool &modal): KDialog(parent)
{
    selectProfileWidget = new SelectProfileWidget;

    setMainWidget(selectProfileWidget);
    setButtons( Ok | Cancel);
    setDefaultButton(Ok);
    setModal(modal);
    setMainWidget(selectProfileWidget);
    setWindowTitle(i18n("Auto-Populate"));

    connect(selectProfileWidget->profilesWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(checkForUpdate(QTreeWidgetItem*,int)));
    kDebug()<< "remote  " << remote->name();
    QList<Profile*> profiles = ProfileServer::allProfiles();
    foreach(Profile* profile, profiles) {
        ProfileServer::ProfileSupportedByRemote tSupported = ProfileServer::isProfileAvailableForRemote(profile, *remote);
        kDebug()<< "support "<< tSupported;
        if ( tSupported != ProfileServer::NO_ACTIONS_DEFINED) {	
            ProfileWrapper wrapper = ProfileWrapper(profile, tSupported);
            QTreeWidgetItem* tTreewidget = new QTreeWidgetItem(selectProfileWidget->profilesWidget,QStringList()<< profile->name());
            tTreewidget->setData(0,Qt::UserRole,qVariantFromValue<ProfileWrapper>(wrapper));
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
     enableButtonOk(false);
}



void SelectProfile::checkForUpdate(QTreeWidgetItem* widgetItem, int col)
{
    if (col == -1) {
        selectProfileWidget->selectionLabel->setText(QString());
        enableButtonOk(false);
        return;
    }
    ProfileServer::ProfileSupportedByRemote tSupported =  widgetItem->data(0, Qt::UserRole).value<ProfileWrapper>().getSupported();

    switch (tSupported) {
    case ProfileServer::FULL_SUPPORTED :
        selectProfileWidget->selectionLabel->setText(i18n("Remote supports all defined buttons in selected profile"));
        enableButtonOk(true);
        break;
    case ProfileServer::PARTIAL_SUPPORTED :
        selectProfileWidget->selectionLabel->setText(i18n("Remote does not support all defined buttons in selected profile"));
        enableButtonOk(true);
        break;
    case ProfileServer::NOT_SUPPORTED :
        selectProfileWidget->selectionLabel->setText(i18n("Remote supports none of the defined buttons in selected profile"));
        enableButtonOk(false);
        break;
    default:
        selectProfileWidget->selectionLabel->setText(QString());
        enableButtonOk(false);
    }
}


Profile* SelectProfile::getSelectedProfile()
{
    return selectProfileWidget->profilesWidget->currentItem()->data(0, Qt::UserRole).value<ProfileWrapper>().getProfile();
}

