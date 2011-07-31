/*************************************************************************
 * Copyright (C) 2010 by Michael Zanetti <michael_zanetti@gmx.net>       *
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


#include "kcmremotecontrol.h"
#include "addaction.h"
#include "remote.h"
#include "editactioncontainer.h"
#include "modedialog.h"
#include "profileserver.h"
#include "selectprofile.h"
#include "dbusinterface.h"
#include "model.h"
#include "remotelist.h"
#include "remotecontrolmanager.h"

#include <kdeutils-version.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <kaboutdata.h>
#include <kmessagebox.h>

#include <QtDBus/QDBusInterface>

K_PLUGIN_FACTORY( KCMLircFactory, registerPlugin<KCMRemoteControl>();)
K_EXPORT_PLUGIN( KCMLircFactory( "kcm_remotecontrol" ) )

KCMRemoteControl::KCMRemoteControl(QWidget *parent, const QVariantList &args) :
        KCModule(KCMLircFactory::componentData(), parent, args)
{

    setAboutData(new KAboutData("kcm_remotecontrol", 0, ki18n("KRemoteControl"), KDEUTILS_VERSION_STRING,
                                ki18n("The KDE Remote Control System"), KAboutData::License_GPL_V2,
                                ki18n("2010 Michael Zanetti, 2010 Frank Scheffold"),
                                ki18n("Use this to configure KDE's remote control system in order to control any KDE application with your remote control."),
                                "http://utils.kde.org/projects/kremotecontrol"));
    setQuickHelp(i18n("<h1>Remote Controls</h1><p>This module allows you to configure bindings between your remote controls and KDE applications. Simply select your remote control and click Add next to the Actions/Buttons list to create new action for button presses.</p>"));

    KGlobal::locale()->insertCatalog( QLatin1String( "libkremotecontrol" ));

    QHBoxLayout *layout = new QHBoxLayout(this);
    QWidget *widget = new QWidget(this);
    ui.setupUi(widget);
    layout->addWidget(widget);

    // Set up GUI buttons
    ui.pbAddMode->setIcon(KIcon( QLatin1String( "list-add" )));
    connect(ui.pbAddMode, SIGNAL(clicked(bool)), SLOT(addMode()));

    ui.pbRemoveMode->setIcon(KIcon( QLatin1String( "list-remove" )));
    connect(ui.pbRemoveMode, SIGNAL(clicked(bool)), SLOT(removeMode()));

    ui.pbAddAction->setIcon(KIcon( QLatin1String( "list-add" )));
    connect(ui.pbAddAction, SIGNAL(clicked(bool)), SLOT(addAction()));

    ui.pbRemoveAction->setIcon(KIcon( QLatin1String( "list-remove" )));
    connect(ui.pbRemoveAction, SIGNAL(clicked(bool)), SLOT(removeAction()));

    ui.pbEditMode->setIcon(KIcon( QLatin1String( "configure" )));
    connect(ui.pbEditMode, SIGNAL(clicked(bool)), SLOT(editMode()));

    ui.pbEditAction->setIcon(KIcon( QLatin1String( "configure" )));
    connect(ui.pbEditAction, SIGNAL(clicked(bool)), SLOT(editAction()));

    ui.pbMoveModeUp->setIcon(KIcon( QLatin1String( "arrow-up" )));
    connect(ui.pbMoveModeUp, SIGNAL(clicked(bool)), SLOT(moveModeUp()));

    ui.pbMoveModeDown->setIcon(KIcon( QLatin1String( "arrow-down" )));
    connect(ui.pbMoveModeDown, SIGNAL(clicked(bool)), SLOT(moveModeDown()));

    ui.pbMoveActionUp->setIcon(KIcon( QLatin1String( "arrow-up" )));
    connect(ui.pbMoveActionUp, SIGNAL(clicked(bool)), SLOT(moveActionUp()));

    ui.pbMoveActionDown->setIcon(KIcon( QLatin1String( "arrow-down" )));
    connect(ui.pbMoveActionDown, SIGNAL(clicked(bool)), SLOT(moveActionDown()));

    ui.pbCopyAction->setIcon(KIcon( QLatin1String( "edit-copy" )));
    connect(ui.pbCopyAction, SIGNAL(clicked(bool)), SLOT(copyAction()));

    ui.pbAutoPopulate->setIcon(KIcon( QLatin1String( "tools-wizard" )));
    connect(ui.pbAutoPopulate, SIGNAL(clicked(bool)), SLOT(autoPopulate()));

    // Create RemoteModel
    m_remoteModel = new RemoteModel(m_remoteList, ui.tvRemotes);
    ui.tvRemotes->setModel(m_remoteModel);
    connect(ui.tvRemotes->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), SLOT(modeSelectionChanged(QModelIndex)));
    connect(ui.tvRemotes, SIGNAL(doubleClicked(QModelIndex)), SLOT(editMode()));
    // QueuedConnection needed because the model itself may has some slots queded and refreshing the model before that breaks logic
    connect(m_remoteModel, SIGNAL(modeChanged(Mode*)), SLOT(actionDropped(Mode*)), Qt::QueuedConnection);

    // Create ActionModel
    m_actionModel = new ActionModel(ui.tvActions);
    ui.tvActions->setModel(m_actionModel);
    connect(ui.tvActions->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), SLOT(actionSelectionChanged(QModelIndex)));
    connect(ui.tvActions, SIGNAL(doubleClicked(QModelIndex)), SLOT(editAction()));

    // connect ShowTrayIcon checkbox
    connect(ui.cbTrayIcon, SIGNAL(clicked(bool)), SLOT(changed()));

    connect(RemoteControlManager::notifier(), SIGNAL(statusChanged(bool)), SLOT(addUnconfiguredRemotes()));
    connect(RemoteControlManager::notifier(), SIGNAL(remoteControlAdded(QString)), SLOT(addUnconfiguredRemotes()));
}

KCMRemoteControl::~KCMRemoteControl() {
    foreach(Remote *remote, m_remoteList){
        delete remote;
    }
}

void KCMRemoteControl::addAction() {
    Remote *remote = m_remoteModel->remote(ui.tvRemotes->currentIndex());

    AddAction addActionDialog;
    Action *newAction = addActionDialog.createAction(remote->name());
    if(newAction != 0){
        Mode *mode = m_remoteModel->mode(ui.tvRemotes->currentIndex());
        mode->addAction(newAction);
        updateActions(mode);
        emit changed(true);
    }
}

void KCMRemoteControl::removeAction() {
    Mode *mode = m_remoteModel->mode(ui.tvRemotes->selectionModel()->currentIndex());
    Action *action = m_actionModel->action(ui.tvActions->selectionModel()->currentIndex());

    mode->removeAction(action);
    updateActions(mode);
    emit changed(true);
}

void KCMRemoteControl::editAction() {
    Action *action = m_actionModel->action(ui.tvActions->selectionModel()->currentIndex());
    Remote *remote = m_remoteModel->remote(ui.tvRemotes->selectionModel()->currentIndex());
    Mode *mode = m_remoteModel->mode(ui.tvRemotes->selectionModel()->currentIndex());

    QPointer<EditActionContainer> editActioncontainer = new EditActionContainer(action, remote->name());
    if(editActioncontainer->exec()) {
        updateActions(mode);
        emit changed(true);
    }
    delete editActioncontainer;
}

void KCMRemoteControl::copyAction() {
    Action *action = m_actionModel->action(ui.tvActions->selectionModel()->currentIndex());
    Mode *mode = m_remoteModel->mode(ui.tvRemotes->selectionModel()->currentIndex());
    Action *newAction = action->clone();
    mode->addAction(newAction);
    updateActions(mode);
    ui.tvActions->selectionModel()->setCurrentIndex(m_actionModel->find(newAction), QItemSelectionModel::Rows | QItemSelectionModel::SelectCurrent);
    editAction();
    emit changed(true);
}

void KCMRemoteControl::moveActionUp() {
    Mode *mode = m_remoteModel->mode(ui.tvRemotes->selectionModel()->currentIndex());
    Action *action = m_actionModel->action(ui.tvActions->selectionModel()->currentIndex());
    mode->moveActionUp(action);
    updateActions(mode);
    emit changed(true);
}

void KCMRemoteControl::moveActionDown() {
    Mode *mode = m_remoteModel->mode(ui.tvRemotes->selectionModel()->currentIndex());
    Action *action = m_actionModel->action(ui.tvActions->selectionModel()->currentIndex());
    mode->moveActionDown(action);
    updateActions(mode);
    emit changed(true);
}

void KCMRemoteControl::autoPopulate() {
    Mode *mode = m_remoteModel->mode(ui.tvRemotes->selectionModel()->currentIndex());
    Remote *remote = m_remoteModel->remote(ui.tvRemotes->selectionModel()->currentIndex());
    QPointer<SelectProfile> autoPopulateDialog = new SelectProfile(remote, this);
    if(autoPopulateDialog->exec()){
        Profile *profile = autoPopulateDialog->getSelectedProfile();
        foreach(const RemoteControlButton &button, RemoteControl(remote->name()).buttons()){
            ProfileActionTemplate actionTemplate = profile->actionTemplateByButton(button.name());
            if(!actionTemplate.buttonName().isEmpty()){
                mode->addAction(actionTemplate.createAction(button));
            }
        }
    }
    delete autoPopulateDialog;
    updateActions(mode);
    emit changed(true);

}

void KCMRemoteControl::addMode() {
    Remote *remote = m_remoteModel->remote(ui.tvRemotes->selectionModel()->currentIndex());
    kDebug() << "current selected remote:" << remote;

    QPointer<ModeDialog> modeDialog = new ModeDialog(remote);
    if(modeDialog->exec()){
        m_remoteModel->refresh(m_remoteList);
        foreach(const Mode *mode, remote->allModes()){
            kDebug() << "Created Mode" << mode->name();
        }
        updateModes();
        emit changed(true);
    }
    delete modeDialog;
}

void KCMRemoteControl::editMode() {
    Remote *remote = m_remoteModel->remote(ui.tvRemotes->selectionModel()->currentIndex());
    Mode *mode = m_remoteModel->mode(ui.tvRemotes->selectionModel()->currentIndex());
    kDebug() << "current selected remote:" << remote << "and mode:" << mode;

    QPointer<ModeDialog> modeDialog = new ModeDialog(remote, mode);
    if(modeDialog->exec()){
        m_remoteModel->refresh(m_remoteList);
        updateModes();
        emit changed(true);
    }
    delete modeDialog;
}

void KCMRemoteControl::removeMode() {
    QModelIndex currentIndex = ui.tvRemotes->selectionModel()->currentIndex();
    Remote *remote = m_remoteModel->remote(currentIndex);
    Mode *mode = m_remoteModel->mode(currentIndex);
    if(remote && remote->allModes().contains(mode)){
        // If the master mode is selected the we should remove the whole remote
        if(mode == remote->masterMode()){
            if(KMessageBox::questionYesNo(this, i18n("Are you sure you want to remove this remote and all of its modes and actions?"), i18n("Remove remote")) == KMessageBox::Yes) {
                m_remoteList.remove(m_remoteList.indexOf(remote));
                m_remoteModel->clear(); // Clear the models before deleting the remote!!!
                m_actionModel->clear();
                delete remote;
                addUnconfiguredRemotes(); // Just in case we removed a physically available remote (shouldn't happen). This also refreshes the modeModel
            } else {
                return; // User cancelled
            }
        } else {
            if((mode->actions().count() > 0) &&
                (KMessageBox::questionYesNo(this, i18n("Are you sure you want to remove this mode and all contained actions?"), i18n("Remove mode")) != KMessageBox::Yes)) {
                return; // User cancelled
            }
            remote->removeMode(mode);
            updateModes();
            ui.tvRemotes->selectionModel()->setCurrentIndex(m_remoteModel->find(remote->masterMode()), QItemSelectionModel::Rows | QItemSelectionModel::SelectCurrent);
        }
        emit changed(true);
    }
}
void KCMRemoteControl::moveModeUp() {
    QModelIndex currentIndex = ui.tvRemotes->selectionModel()->currentIndex();
    Remote *remote = m_remoteModel->remote(currentIndex);
    Mode *mode = m_remoteModel->mode(currentIndex);
    remote->moveModeUp(mode);
    updateModes();
    emit changed(true);
}

void KCMRemoteControl::moveModeDown() {
    QModelIndex currentIndex = ui.tvRemotes->selectionModel()->currentIndex();
    Remote *remote = m_remoteModel->remote(currentIndex);
    Mode *mode = m_remoteModel->mode(currentIndex);
    remote->moveModeDown(mode);
    updateModes();
    emit changed(true);
}

void KCMRemoteControl::updateModes() {
    Mode *mode = m_remoteModel->mode(ui.tvRemotes->selectionModel()->currentIndex());
    m_remoteModel->refresh(m_remoteList);
    ui.tvRemotes->expandAll();
    ui.tvRemotes->resizeColumnToContents(0);
    if(mode){
        ui.tvRemotes->selectionModel()->setCurrentIndex(m_remoteModel->find(mode), QItemSelectionModel::Rows | QItemSelectionModel::SelectCurrent);
    }
    modeSelectionChanged(ui.tvRemotes->selectionModel()->currentIndex());

    if(!m_remoteList.isEmpty()){
        ui.lNoRemotesWarning->setMaximumSize(0,0);
        ui.tvRemotes->setEnabled(true);
        ui.tvActions->setEnabled(true);
        ui.lRemotes->setEnabled(true);
    } else {
        ui.lNoRemotesWarning->setMaximumSize(32767, 32767);
        ui.tvRemotes->setEnabled(false);
        ui.tvActions->setEnabled(false);
        ui.lRemotes->setEnabled(false);
    }
}

void KCMRemoteControl::updateActions(Mode *mode) {
    Action *oldAction = m_actionModel->action(ui.tvActions->selectionModel()->currentIndex());
    m_actionModel->refresh(mode);
    ui.tvActions->resizeColumnToContents(0);
    if(oldAction){
        ui.tvActions->selectionModel()->setCurrentIndex(m_actionModel->find(oldAction), QItemSelectionModel::Rows | QItemSelectionModel::SelectCurrent);
    }
    actionSelectionChanged(ui.tvActions->selectionModel()->currentIndex());
}

void KCMRemoteControl::modeSelectionChanged(const QModelIndex &index) {
    if(index.isValid()){
        ui.pbAddMode->setEnabled(true);
        ui.pbEditMode->setEnabled(true);
        ui.pbAddAction->setEnabled(true);
        ui.pbAutoPopulate->setEnabled(true);

        // Only enable the remove mode button if a non-Master mode is selected,
        // or if the Remote is not available in in the system
        if((m_remoteModel->mode(index) != m_remoteModel->remote(index)->masterMode())
            || (!m_remoteModel->remote(index)->isAvailable())){
            ui.pbRemoveMode->setEnabled(true);
        } else {
            ui.pbRemoveMode->setEnabled(false);
        }

    } else {
        ui.pbAddMode->setEnabled(false);
        ui.pbEditMode->setEnabled(false);
        ui.pbAddAction->setEnabled(false);
        ui.pbAutoPopulate->setEnabled(false);
        ui.pbRemoveMode->setEnabled(false);
    }

    Mode *mode = m_remoteModel->mode(index);
    if(mode){

        Remote *remote = m_remoteModel->remote(index);
        if(mode == remote->masterMode()){
            ui.lActions->setText(i18n("Configured actions for %1:", remote->name()));
        } else {
            ui.lActions->setText(i18n("Configured actions for %1 in mode %2:", remote->name(), mode->name()));
        }

        if(remote->allModes().indexOf(mode) > 1){
            ui.pbMoveModeUp->setEnabled(true);
        } else {
            ui.pbMoveModeUp->setEnabled(false);
        }

        if((remote->allModes().indexOf(mode) < (remote->allModes().count() - 1)) && (mode != remote->masterMode())){
            ui.pbMoveModeDown->setEnabled(true);
        } else {
            ui.pbMoveModeDown->setEnabled(false);
        }

        updateActions(mode);
    }
}

void KCMRemoteControl::actionSelectionChanged(const QModelIndex& index) {
    if(index.isValid()){
        ui.pbRemoveAction->setEnabled(true);
        ui.pbEditAction->setEnabled(true);
        ui.pbCopyAction->setEnabled(true);
        if(index.row() > 0){
            ui.pbMoveActionUp->setEnabled(true);
        } else {
            ui.pbMoveActionUp->setEnabled(false);
        }
        if(index.row() < (m_actionModel->rowCount() - 1)){
            ui.pbMoveActionDown->setEnabled(true);
        } else {
            ui.pbMoveActionDown->setEnabled(false);
        }
    } else {
        ui.pbRemoveAction->setEnabled(false);
        ui.pbEditAction->setEnabled(false);
        ui.pbCopyAction->setEnabled(false);
        ui.pbMoveActionUp->setEnabled(false);
        ui.pbMoveActionDown->setEnabled(false);
    }
}

void KCMRemoteControl::addUnconfiguredRemotes() {
    // Check if there are Remotes available in the system but not yet in m_remoteList
    foreach(const QString &remoteName, RemoteControl::allRemoteNames()){
        if(!m_remoteList.contains(remoteName)){
            Remote *remote = new Remote(remoteName);
            m_remoteList.append(remote);
        }
    }
    updateModes();
}

void KCMRemoteControl::load() {
    m_remoteList.loadFromConfig(QLatin1String( "kremotecontrolrc" ));

    addUnconfiguredRemotes();

/*    if(!m_remoteList.isEmpty()){
        QModelIndex firstItem = m_remoteModel->find(m_remoteList.first()->masterMode());
        ui.tvRemotes->selectionModel()->setCurrentIndex(firstItem, QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
    }*/

    // Check if the daemon module is running
    if(!m_remoteList.isEmpty()){ // No need to run the daemon if we have no remote controls
        kDebug() << "remotes found... checking for kded module";
        if(!DBusInterface::getInstance()->isKdedModuleRunning()) {
            kDebug() << "kded module not running";
            if(!DBusInterface::getInstance()->loadKdedModule()){
                KMessageBox::error(this, i18n("The remote control daemon failed to load. Your remote controls will not work."), i18n("Failed to load daemon"));
            }
        }
    }

    KConfig config(QLatin1String( "kremotecontrolrc" ));
    KConfigGroup globalGroup = KConfigGroup(&config, "Global");
    ui.cbTrayIcon->setChecked(globalGroup.readEntry("ShowTrayIcon", true));
}

void KCMRemoteControl::save() {
    m_remoteList.saveToConfig(QLatin1String( "kremotecontrolrc" ));

    KConfig config(QLatin1String( "kremotecontrolrc" ));
    KConfigGroup globalGroup = KConfigGroup(&config, "Global");
    globalGroup.writeEntry("ShowTrayIcon", ui.cbTrayIcon->isChecked());
    globalGroup.sync(); // Sync the config before requesting the daemon to update.

    DBusInterface::getInstance()->reloadRemoteControlDaemon();

    // If there are no remotes configured it makes no sense to have the daemon running. stop it
    if(m_remoteList.isEmpty()){
        if(DBusInterface::getInstance()->isKdedModuleRunning()){
            DBusInterface::getInstance()->unloadKdedModule();
        }
    }
}

void KCMRemoteControl::actionDropped(Mode* mode) {
    ui.tvRemotes->selectionModel()->setCurrentIndex(m_remoteModel->find(mode), QItemSelectionModel::Rows | QItemSelectionModel::SelectCurrent);
    updateActions(mode);
    emit changed(true);
}

#include "kcmremotecontrol.moc"
