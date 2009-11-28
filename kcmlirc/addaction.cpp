/*************************************************************************
 * Copyright            : (C) 2003 by Gav Wood <gav@kde.org>             *
 *                                                                       *
 * This program is free software; you can redistribute it and/or         *
 * modify it under the terms of the GNU General Public License as        *
 * published by the Free Software Foundation; either version 2 of        *
 * the License or (at your option) version 3 or any later version        *
 * accepted by the membership of KDE e.V. (or its successor approved     *
 * by the membership of KDE e.V.), which shall act as a proxy            *
 * defined in Section 14 of version 3 of the license.                    *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *************************************************************************/


/**
  * @author Gav Wood, Michael Zanetti & Frank Scheffold
  */

#include "addaction.h"
#include "prototype.h"
#include "profileserver.h"
#include "remoteserver.h"
#include "mode.h"
#include "arguments.h"
#include "iraction.h"
#include "model.h"
#include "dbusinterface.h"
#include <QRegExp>
#include <QHeaderView>

#include <kdebug.h>
#include <klineedit.h>
#include <kapplication.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <keditlistbox.h>
#include <klocale.h>

AddAction::AddAction(QWidget *parent, const char *name, const Mode &mode): theMode(mode)
{
    Q_UNUSED(name)
    Q_UNUSED(parent)
    setupUi(this);

    dbusFunctionModel = new DBusFunctionModel(theDBusFunctions);
    theDBusFunctions->setModel(dbusFunctionModel);
    theDBusFunctions->setSelectionBehavior(QAbstractItemView::SelectRows);
    theDBusFunctions->setSelectionMode(QAbstractItemView::SingleSelection);
    //theFunctions->setShowGrid(false);
    dbusAppsModel = new QStandardItemModel(theDBusApplications);    
    theDBusApplications->setEditTriggers(QAbstractItemView::NoEditTriggers);
    theDBusApplications->setModel(dbusAppsModel);

    argumentsModel = new QStandardItemModel(argumentsView);
    argumentsView->setModel(argumentsModel);
    argumentsView->setItemDelegate(new ArgumentDelegate());

    connect(this, SIGNAL(currentIdChanged(int)), this, SLOT(updateButtonStates()));
    connect(theChangeMode, SIGNAL(clicked()), this, SLOT(updateButtonStates()));
    connect(theSwitchMode, SIGNAL(clicked()), this, SLOT(updateButtonStates()));
    connect(theExitMode, SIGNAL(clicked()), this, SLOT(updateButtonStates()));
    connect(theUseDBus, SIGNAL(clicked()), this, SLOT(updateButtonStates()));

    connect(theUseProfile, SIGNAL(toggled(bool)), theProfiles, SLOT(setEnabled(bool)));
    connect(theUseProfile, SIGNAL(clicked()), this, SLOT(updateButtonStates()));

    connect(theNotJustStart, SIGNAL(clicked()), this, SLOT(updateButtonStates()));
    connect(theNotJustStart, SIGNAL(toggled(bool)), theProfileFunctions, SLOT(setEnabled(bool)));
    connect(theNotJustStart, SIGNAL(toggled(bool)), theAutoStart, SLOT(setEnabled(bool)));
    connect(theNotJustStart, SIGNAL(toggled(bool)), theRepeat, SLOT(setEnabled(bool)));
    connect(theNotJustStart, SIGNAL(toggled(bool)), theRepeat, SLOT(setChecked(bool)));

    connect(theJustStart, SIGNAL(toggled(bool)), theAutoStart, SLOT(setChecked(bool)));
    connect(theJustStart, SIGNAL(clicked()), this, SLOT(updateButtonStates()));

    connect(theDBusApplications, SIGNAL(clicked(QModelIndex)), this, SLOT(updateDBusFunctions(QModelIndex)));
    connect(theDBusApplications, SIGNAL(clicked(QModelIndex)), this, SLOT(updateButtonStates()));

    connect(theProfiles, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtonStates()));
    connect(theProfiles, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(next()));

    connect(theModes, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtonStates()));
    connect(theModes, SIGNAL(itemSelectionChanged()), this, SLOT(slotModeSelected()));
    connect(theModes, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(next()));

    connect(theButtons, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtonStates()));
    connect(theButtons, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(next()));

    connect(theProfileFunctions, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtonStates()));
    connect(theProfileFunctions, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(next()));


    connect(theDBusFunctions, SIGNAL(clicked(QModelIndex)), this, SLOT(updateButtonStates()));
    connect(theDBusFunctions,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(next()));

    connect(DBusInterface::getInstance(), SIGNAL(haveButton(const QString &, const QString &)), this, SLOT(updateButton(const QString &, const QString &)));


    updateProfiles();
    updateButtons();
}

AddAction::~AddAction()
{
}

void AddAction::slotModeSelected()
{
    theSwitchMode->setChecked(true);
}

int AddAction::nextId() const
{
    if (currentId() == START) {
        return SELECT_BUTTON;
    }

    if (currentId() == SELECT_BUTTON) {
        if (theUseProfile->isChecked()) {
            return SELECT_FUNCTION_PROFILE;
        }

        if (theUseDBus->isChecked()) {
            return SELECT_FUNCTION_DBUS;
        }

        return SELECT_MODE;
    }

    if (currentId() == SELECT_FUNCTION_PROFILE) {
	if(!theProfileFunctions->currentItem()){
	    return ACTION_OPTIONS;
	}
	QString application = theProfiles->currentItem()->data(Qt::UserRole).toString();
	QString function = theProfileFunctions->currentItem()->data(0, Qt::UserRole).toString();
	const ProfileAction *profileAction =  ProfileServer::profileServer()->getAction(application, function);

	const QList<ProfileActionArgument> &profileActionArguments = profileAction->arguments();
	kDebug() << "argcount" << profileActionArguments.count();
	if(profileActionArguments.count() == 0){
	    return ACTION_OPTIONS;
	}
        return ACTION_ARGUMENTS;
    }

    if (currentId() == SELECT_FUNCTION_DBUS) {
	if(!theDBusFunctions->currentIndex().isValid()){
	    return ACTION_OPTIONS;
	}
	Prototype p = dbusFunctionModel->getPrototype(theDBusFunctions->currentIndex().row());
	kDebug() << "argcount" << p.getArguments().size();
	if(p.getArguments().size() == 0){
	    return ACTION_OPTIONS;
	}
	return ACTION_ARGUMENTS;
    }

    if (currentId() == ACTION_ARGUMENTS) {
        return ACTION_OPTIONS;
    }

    return -1;
}

void AddAction::updateButton(const QString &remote, const QString &button)
{
    if (theMode.remote() == remote) {
        theButtons->setCurrentItem(theButtons->findItems(RemoteServer::remoteServer()->getButtonName(remote, button), 0).first());
        theButtons->scrollToItem(theButtons->findItems(RemoteServer::remoteServer()->getButtonName(remote, button), 0).first());
    } else {
        KMessageBox::error(0, i18n("You did not select a mode of that remote control. Please use %1, "
                                   "or revert back to select a different mode.", theMode.remoteName()),
                           i18n("Incorrect Remote Control Detected"));
    }
}

void AddAction::updateButtons()
{
    theButtons->clear();
    foreach(const QString &buttonName, DBusInterface::getInstance()->getButtons(theMode.remote())) {
        kDebug() << "foud buttonName " << buttonName;
        QListWidgetItem *tItem =  new  QListWidgetItem(RemoteServer::remoteServer()->getButtonName(theMode.remote(), buttonName),theButtons);
        tItem->setData(Qt::UserRole,buttonName);

    }
}

void AddAction::initializePage(int id)
{
    if (id == SELECT_BUTTON) {
        DBusInterface::getInstance()->requestNextKeyPress();
    } else if (id == SELECT_FUNCTION_PROFILE || id == SELECT_FUNCTION_DBUS || id == SELECT_MODE) {
        DBusInterface::getInstance()->cancelKeyPressRequest();
        if (id == SELECT_FUNCTION_PROFILE) {
            updateProfileFunctions();
        } else if (id == SELECT_FUNCTION_DBUS) {
            updateDBusApplications();
        }
    } else if (id == ACTION_ARGUMENTS) {
        updateArguments();
    } else if (id == ACTION_OPTIONS ){
	updateOptions();
    }
    updateButtonStates();
}

void AddAction::updateButtonStates()
{
    kDebug() << "Updating button states";
    switch (currentId()) {
    case START:
        button(QWizard::NextButton)->setEnabled(theProfiles->currentItem() || !theUseProfile->isChecked());
        break;
    case SELECT_BUTTON:
        button(QWizard::NextButton)->setEnabled(!theButtons->selectedItems().isEmpty());
        break;
    case SELECT_FUNCTION_DBUS:
        button(QWizard::NextButton)->setEnabled(theDBusApplications->currentIndex().isValid() && theDBusFunctions->currentIndex().isValid());
        break;
    case SELECT_FUNCTION_PROFILE:
        button(QWizard::NextButton)->setEnabled(theProfileFunctions->currentItem() != 0 || theJustStart->isChecked());
        break;
    case ACTION_ARGUMENTS:
        button(QWizard::NextButton)->setEnabled(true);
        break;
    case ACTION_OPTIONS:
        button(QWizard::NextButton)->setEnabled(true);
        button(QWizard::FinishButton)->setEnabled(true);
        break;
    case SELECT_MODE:
        button(QWizard::NextButton)->setEnabled(false);
        button(QWizard::FinishButton)->setEnabled(theModes->currentItem() || !theSwitchMode->isChecked());
        break;
    }
}

void AddAction::updateProfiles()
{
    theProfiles->clear();

    foreach (Profile *tmp, ProfileServer::profileServer()->profiles()) {
        QListWidgetItem *item = new QListWidgetItem(tmp->name());
        item->setData(Qt::UserRole, tmp->id());
        theProfiles->addItem(item);
    }
}

void AddAction::updateOptions()
{
    IfMulti im = IM_SENDTOTOP;
    if (theUseProfile->isChecked()) {
        ProfileServer *theServer = ProfileServer::profileServer();
        if (!theProfiles->currentItem()) {
            return;
        }
        const Profile *p = theServer->getProfileById(theProfiles->currentItem()->data(Qt::UserRole).toString() );
        im = p->ifMulti();
        isUnique = p->unique();
    } else if (theUseDBus->isChecked()) {
	QString currentDBusApp = dbusAppsModel->data(theDBusApplications->currentIndex().parent(), Qt::UserRole).toString();
        isUnique = DBusInterface::getInstance()->isUnique(currentDBusApp);
    } else return;

    theIMLabel->setEnabled(!isUnique);
    theIMLine->setEnabled(!isUnique);
    theIMTop->setEnabled(!isUnique);
    theDontSend->setEnabled(!isUnique);
    theSendToBottom->setEnabled(!isUnique);
    theSendToTop->setEnabled(!isUnique);
    theSendToAll->setEnabled(!isUnique);
    switch (im) {
    case IM_DONTSEND:
        theDontSend->setChecked(true);
        break;
    case IM_SENDTOTOP:
        theSendToTop->setChecked(true);
        break;
    case IM_SENDTOBOTTOM:
        theSendToBottom->setChecked(true);
        break;
    case IM_SENDTOALL:
        theSendToAll->setChecked(true);
        break;
    }
}

void AddAction::updateProfileFunctions()
{
    kDebug() << "updateProfileFunctions called";
    ProfileServer *theServer = ProfileServer::profileServer();
    theProfileFunctions->clear();
    if (!theProfiles->currentItem()) {
        return;
    }

    const Profile *p = theServer->getProfileById(theProfiles->currentItem()->data(Qt::UserRole).toString());
    QHash<QString, ProfileAction*> dict = p->actions();
    kDebug() << "actions: " << p->actions();
    QHash<QString, ProfileAction*>::const_iterator i;
    for (i = dict.constBegin(); i != dict.constEnd(); ++i) {
        kDebug() << "got function: " << i.value()->name();
        QStringList parameters;
        parameters << i.value()->name() << QString().setNum(i.value()->arguments().count()) << i.value()->comment();
        QTreeWidgetItem *item = new QTreeWidgetItem(theProfileFunctions, parameters);
        kDebug() << "inserting profile function" << i.key();
        item->setData(0, Qt::UserRole, i.key());
    }
}

void AddAction::updateArguments()
{
    argumentsModel->clear();
    QStringList headerLabels;
    headerLabels << i18n("Argument name") << i18nc("The value of an argument", "Value");
    argumentsModel->setHorizontalHeaderLabels(headerLabels);

    if (theUseProfile->isChecked()) {
        QString application = theProfiles->currentItem()->data(Qt::UserRole).toString();
        QString function = theProfileFunctions->currentItem()->data(0, Qt::UserRole).toString();
        const ProfileAction *profileAction =  ProfileServer::profileServer()->getAction(application, function);

        const QList<ProfileActionArgument> &profileActionArguments = profileAction->arguments();
        for (int i = 0; i < profileActionArguments.count(); ++i) {
            QList<QStandardItem*> tmp;
            tmp.append(new ArgumentsModelItem(profileActionArguments.at(i).comment() + " (" + profileActionArguments.at(i).type() + ')'));
            tmp.append(new ArgumentsModelItem(profileActionArguments.at(i).getDefault()));
            argumentsModel->appendRow(tmp);
        }
    } else if ( theUseDBus->isChecked()) {
        Prototype p = dbusFunctionModel->getPrototype(theDBusFunctions->currentIndex().row());
        for (int i = 0; i < p.getArguments().size(); ++i) {
            QList<QStandardItem*> tmp;
            tmp.append(new ArgumentsModelItem(p.getArguments().at(i).second + " (" + QVariant::typeToName(p.getArguments().at(i).first) + ')'));
            tmp.append(new ArgumentsModelItem(QVariant(p.getArguments().at(i).first)));
            argumentsModel->appendRow(tmp);
        }
    }    
    argumentsView->resizeColumnsToContents();
    argumentsView->resizeRowsToContents();    
    argumentsView->horizontalHeader()->setStretchLastSection(true);
}

void AddAction::updateDBusApplications()
{
    dbusAppsModel->clear();
    dbusAppsModel->setHorizontalHeaderLabels(QStringList() << i18n("D-Bus applications"));
    foreach(const QString &item, DBusInterface::getInstance()->getRegisteredPrograms()) {
        DBusServiceItem *tServiceItem = new DBusServiceItem(item);
        tServiceItem->setEditable(false);
        dbusAppsModel->appendRow(tServiceItem);
        foreach(const QString &object, DBusInterface::getInstance()->getObjects(item)) {
            tServiceItem->appendRow(new QStandardItem(object));
        }
    }
    dbusAppsModel->sort(0, Qt::AscendingOrder);
    updateDBusFunctions(QModelIndex());
}


void AddAction::updateDBusFunctions(QModelIndex pIndex) {
    dbusFunctionModel->clear();
    QModelIndex tParent = pIndex.parent();
    if (tParent.isValid()) {
        QList<Prototype> tList = DBusInterface::getInstance()->getFunctions(dbusAppsModel->data(tParent, Qt::UserRole).toString(), dbusAppsModel->data(pIndex).toString() );
        for (int i = 0; i < tList.size(); i++) {
	    dbusFunctionModel->appendRow(tList.at(i));
        }
        theDBusFunctions->model()->sort(0, Qt::AscendingOrder);

    }
    theDBusFunctions->resizeColumnsToContents();
    theDBusFunctions->resizeRowsToContents();
    updateButtonStates();
}


IRAction* AddAction::getAction()
{
    IRAction *action = new IRAction();
    action->setRemote(theMode.remote());
    action->setMode(theMode.name());
    action->setButton(theButtons->currentItem()->data(Qt::UserRole).toString());
    action->setRepeat(theRepeat->isChecked());
    action->setAutoStart(theAutoStart->isChecked());
    action->setDoBefore(theDoBefore->isChecked());
    action->setDoAfter(theDoAfter->isChecked());
    action->setUnique(isUnique);
    action->setIfMulti(theDontSend->isChecked() ? IM_DONTSEND
                       : theSendToTop->isChecked() ? IM_SENDTOTOP
                       : theSendToBottom->isChecked() ? IM_SENDTOBOTTOM
                       : IM_SENDTOALL);
    // change mode?
    if (theChangeMode->isChecked()) {
        if (theSwitchMode->isChecked()
                && !theModes->selectedItems().isEmpty()) {
            action->setProgram("");
            action->setObject(theModes->selectedItems().first()->text());
        } else if (theExitMode->isChecked()) {
            action->setProgram("");
            action->setObject("");
        }
        action->setAutoStart(false);
        action->setRepeat(false);
    }
    // DBus?
    else if (theUseDBus->isChecked()) {
        action->setProgram(dbusAppsModel->data(theDBusApplications->currentIndex().parent(), Qt::UserRole).toString());
        action->setObject(dbusAppsModel->data(theDBusApplications->currentIndex(), Qt::DisplayRole).toString());
        Prototype p =  dbusFunctionModel->getPrototype(theDBusFunctions->currentIndex().row());
        action->setMethod(p.prototype());
        action->setArguments(getCurrentArgs());
    } else if (theUseProfile->isChecked() && !theProfiles->selectedItems().isEmpty() &&
               (!theProfileFunctions->selectedItems().isEmpty() || theJustStart->isChecked())) {
        ProfileServer *theServer = ProfileServer::profileServer();

        if (theNotJustStart->isChecked()) {
            const ProfileAction
            *theAction = theServer->getAction(theProfiles->currentItem()->data(Qt::UserRole).toString(), theProfileFunctions->currentItem()->data(0, Qt::UserRole).toString());
            action->setProgram(theAction->profile()->id());
            action->setObject(theAction->objId());
            action->setMethod(theAction->prototype());
            action->setArguments(getCurrentArgs());
        } else {
            action->setProgram(theProfiles->currentItem()->data(Qt::UserRole).toString());
            action->setObject("");
        }
    }
    return action;
}

#include "addaction.moc"
