/*************************************************************************
 * Copyright: (C) 2010 by Michael Zanetti <michael_zanetti@gmx.net>      *
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


#include "kcmlirc.h"
#include "addaction.h"
#include "editactioncontainer.h"
#include "remote.h"
#include "editactioncontainer.h"
#include "modedialog.h"
#include "profileserver.h"
#include "selectprofile.h"
#include "dbusinterface.h"
#include "model.h"
#include "remotelist.h"

#include <kdeutils-version.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <kaboutdata.h>

#include <QDBusInterface>

K_PLUGIN_FACTORY( KCMLircFactory, registerPlugin<KCMLirc>();)
K_EXPORT_PLUGIN( KCMLircFactory( "kcm_lirc" ) )

KCMLirc::KCMLirc(QWidget *parent, const QVariantList &args) :
        KCModule(KCMLircFactory::componentData(), parent, args)
{

    QDBusConnection::sessionBus().registerObject("/KCMLirc", this, QDBusConnection::ExportAllSlots);

    KGlobal::locale()->insertCatalog("kcm_lirc");
    setAboutData(
        new KAboutData(
            "kcm_lirc",
            0,
            ki18n("KDE Lirc"),
            KDEUTILS_VERSION_STRING, ki18n("The KDE Remote Control System"),
            KAboutData::License_GPL_V2,
            ki18n("Copyright (c)2003 Gav Wood, 2007 Michael Zanetti, 2009 Frank Scheffold"),
            ki18n(
                "Use this to configure KDE's remote control system in order to control any KDE application with your remote control."),
            "http://utils.kde.org/projects/kdelirc"));
    setQuickHelp(
        i18n(
            "<h1>Remote Controls</h1><p>This module allows you to configure bindings between your remote controls and KDE applications. Simply select your remote control and click Add under the Actions/Buttons list. If you want KDE to attempt to automatically assign buttons to a supported application's actions, try clicking the Auto-Populate button.</p><p>To view the recognized applications and remote controls, simply select the <em>Loaded Profiles</em> tab.</p>"));

//     if (!DBusInterface::getInstance()->isProgramRunning("org.kde.irkick")) {
//         if (KMessageBox::questionYesNo(
//                     this,
//                     i18n(
//                         "The Infrared Remote Control software is not currently running. This configuration module will not work properly without it. Would you like to start it now?"),
//                     i18n("Software Not Running"), KGuiItem(i18nc("Start irkick daemon","Start")), KGuiItem(
//                         i18nc("Do not start irkick daemon", "Do Not Start"))) == KMessageBox::Yes) {
//             kDebug() << "S" << KToolInvocation::startServiceByDesktopName("irkick");
//             KConfig theConfig("irkickrc");
//             KConfigGroup generalGroup = theConfig.group("General");
//             if (generalGroup.readEntry("AutoStart", true) == false)
//                 if (KMessageBox::questionYesNo(
//                             this,
//                             i18n(
//                                 "Would you like the infrared remote control software to start automatically when you begin KDE?"),
//                             i18n("Automatically Start?"), KGuiItem(i18n("Start Automatically")),
//                             KGuiItem(i18n("Do Not Start"))) == KMessageBox::Yes)
//                     generalGroup.writeEntry("AutoStart", true);
//         }
//     }

    QHBoxLayout *layout = new QHBoxLayout(this);
    QWidget *widget = new QWidget(this);
    ui.setupUi(widget);
    layout->addWidget(widget);

    // Set up GUI buttons
    ui.pbAddMode->setIcon(KIcon("list-add"));
    connect(ui.pbAddMode, SIGNAL(clicked(bool)), SLOT(addMode()));

    ui.pbRemoveMode->setIcon(KIcon("list-remove"));
    connect(ui.pbRemoveMode, SIGNAL(clicked(bool)), SLOT(removeMode()));

    ui.pbAddAction->setIcon(KIcon("list-add"));
    connect(ui.pbAddAction, SIGNAL(clicked(bool)), SLOT(addAction()));

    ui.pbRemoveAction->setIcon(KIcon("list-remove"));
    connect(ui.pbRemoveAction, SIGNAL(clicked(bool)), SLOT(removeAction()));

    ui.pbEditMode->setIcon(KIcon("configure"));
    connect(ui.pbEditMode, SIGNAL(clicked(bool)), SLOT(editMode()));

    ui.pbEditAction->setIcon(KIcon("configure"));
    connect(ui.pbEditAction, SIGNAL(clicked(bool)), SLOT(editAction()));
    
    ui.pbMoveModeUp->setIcon(KIcon("arrow-up"));
    connect(ui.pbMoveModeUp, SIGNAL(clicked(bool)), SLOT(moveModeUp()));

    ui.pbMoveModeDown->setIcon(KIcon("arrow-down"));
    connect(ui.pbMoveModeDown, SIGNAL(clicked(bool)), SLOT(moveModeDown()));

    ui.pbMoveActionUp->setIcon(KIcon("arrow-up"));
    connect(ui.pbMoveActionUp, SIGNAL(clicked(bool)), SLOT(moveActionUp()));

    ui.pbMoveActionDown->setIcon(KIcon("arrow-down"));
    connect(ui.pbMoveActionDown, SIGNAL(clicked(bool)), SLOT(moveActionDown()));

    ui.pbCopyAction->setIcon(KIcon("edit-copy"));
    connect(ui.pbCopyAction, SIGNAL(clicked(bool)), SLOT(copyAction()));
    
    ui.pbAutoPopulate->setIcon(KIcon("tools-wizard"));
    connect(ui.pbAutoPopulate, SIGNAL(clicked(bool)), SLOT(autoPopulate()));
    
    // Create RemoteModel 
    m_remoteModel = new RemoteModel(m_remoteList, ui.tvRemotes);
    ui.tvRemotes->setModel(m_remoteModel);
    connect(ui.tvRemotes->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), SLOT(modeSelectionChanged(const QModelIndex &)));
    // QueuedConnection needed because the model itself may has some slots queded and refreshing the model before that breaks logic
    connect(m_remoteModel, SIGNAL(modeChanged(Mode *)), SLOT(actionDropped(Mode*)), Qt::QueuedConnection);
    
    // Create ActionModel
    m_actionModel = new ActionModel(ui.tvActions);
    ui.tvActions->setModel(m_actionModel);
    connect(ui.tvActions->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), SLOT(actionSelectionChanged(const QModelIndex &)));
}

KCMLirc::~KCMLirc() {
}

void KCMLirc::addAction()
{
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

void KCMLirc::removeAction() {
    Mode *mode = m_remoteModel->mode(ui.tvRemotes->selectionModel()->currentIndex());
    Action *action = m_actionModel->action(ui.tvActions->selectionModel()->currentIndex());
    
    mode->removeAction(action);
    updateActions(mode);
    emit changed(true);
}

void KCMLirc::editAction() {
    Action *action = m_actionModel->action(ui.tvActions->selectionModel()->currentIndex());
    Remote *remote = m_remoteModel->remote(ui.tvRemotes->selectionModel()->currentIndex());
    Mode *mode = m_remoteModel->mode(ui.tvRemotes->selectionModel()->currentIndex());
    
    EditActionContainer editActioncontainer(action, remote->name());
    if(editActioncontainer.exec()) {
        updateActions(mode);
        emit changed(true);
    }    
}

void KCMLirc::copyAction() {
    Action *action = m_actionModel->action(ui.tvActions->selectionModel()->currentIndex());
    Mode *mode = m_remoteModel->mode(ui.tvRemotes->selectionModel()->currentIndex());
    Action *newAction = action->clone();
    mode->addAction(newAction);
    updateActions(mode);
    ui.tvActions->selectionModel()->setCurrentIndex(m_actionModel->find(newAction), QItemSelectionModel::Rows | QItemSelectionModel::SelectCurrent);
    editAction();
    emit changed(true);
}

void KCMLirc::moveActionUp() {
    Mode *mode = m_remoteModel->mode(ui.tvRemotes->selectionModel()->currentIndex());
    Action *action = m_actionModel->action(ui.tvActions->selectionModel()->currentIndex());
    mode->moveActionUp(action);
    updateActions(mode);
    emit changed(true);
}

void KCMLirc::moveActionDown() {
    Mode *mode = m_remoteModel->mode(ui.tvRemotes->selectionModel()->currentIndex());
    Action *action = m_actionModel->action(ui.tvActions->selectionModel()->currentIndex());
    mode->moveActionDown(action);
    updateActions(mode);
    emit changed(true);
}

void KCMLirc::autoPopulate() {
    Mode *mode = m_remoteModel->mode(ui.tvRemotes->selectionModel()->currentIndex());
    Remote *remote = m_remoteModel->remote(ui.tvRemotes->selectionModel()->currentIndex());
    SelectProfile *autoPopulateDialog = new SelectProfile(remote, this);
    if(autoPopulateDialog->exec()){
        Profile *profile = autoPopulateDialog->getSelectedProfile();
        foreach(const Solid::Control::RemoteControlButton &button, Solid::Control::RemoteControl(remote->name()).buttons()){
            ProfileActionTemplate actionTemplate = profile->actionTemplateByButton(button.name());
            if(!actionTemplate.buttonName().isEmpty()){
                mode->addAction(actionTemplate.createAction(button));
            }
        }
    }
    updateActions(mode);
    emit changed(true);

}

void KCMLirc::addMode() {
    Remote *remote = m_remoteModel->remote(ui.tvRemotes->selectionModel()->currentIndex());
    kDebug() << "current selected remote:" << remote;
    
    ModeDialog modeDialog(remote);
    if(modeDialog.exec()){
        m_remoteModel->refresh(m_remoteList);
        foreach(const Mode *mode, remote->allModes()){
            kDebug() << "Created Mode" << mode->name();
        }
        updateModes();
        emit changed(true);
    }
}

void KCMLirc::editMode() {
    Remote *remote = m_remoteModel->remote(ui.tvRemotes->selectionModel()->currentIndex());
    Mode *mode = m_remoteModel->mode(ui.tvRemotes->selectionModel()->currentIndex());
    kDebug() << "current selected remote:" << remote << "and mode:" << mode;
    
    ModeDialog modeDialog(remote, mode);
    if(modeDialog.exec()){
        m_remoteModel->refresh(m_remoteList);
        updateModes();
        emit changed(true);
    }
}

void KCMLirc::removeMode() {

    QModelIndex currentIndex = ui.tvRemotes->selectionModel()->currentIndex();
    Remote *remote = m_remoteModel->remote(currentIndex);
    Mode *mode = m_remoteModel->mode(currentIndex);
    if(remote && remote->allModes().contains(mode)){
        remote->removeMode(mode);
        updateModes();
        ui.tvRemotes->selectionModel()->setCurrentIndex(m_remoteModel->find(remote->masterMode()), QItemSelectionModel::Rows | QItemSelectionModel::SelectCurrent);
        emit changed(true);
    }
}
void KCMLirc::moveModeUp() {
    QModelIndex currentIndex = ui.tvRemotes->selectionModel()->currentIndex();
    Remote *remote = m_remoteModel->remote(currentIndex);
    Mode *mode = m_remoteModel->mode(currentIndex);    
    remote->moveModeUp(mode);
    updateModes();
    emit changed(true);
}

void KCMLirc::moveModeDown() {
    QModelIndex currentIndex = ui.tvRemotes->selectionModel()->currentIndex();
    Remote *remote = m_remoteModel->remote(currentIndex);
    Mode *mode = m_remoteModel->mode(currentIndex);    
    remote->moveModeDown(mode);
    updateModes();
    emit changed(true);
}

void KCMLirc::updateModes() {
    Mode *mode = m_remoteModel->mode(ui.tvRemotes->selectionModel()->currentIndex());
    m_remoteModel->refresh(m_remoteList);
    ui.tvRemotes->expandAll();
    ui.tvRemotes->resizeColumnToContents(0);
    if(mode){
        ui.tvRemotes->selectionModel()->setCurrentIndex(m_remoteModel->find(mode), QItemSelectionModel::Rows | QItemSelectionModel::SelectCurrent);
    }
    modeSelectionChanged(ui.tvRemotes->selectionModel()->currentIndex());
}

void KCMLirc::updateActions(Mode *mode) {
    Action *oldAction = m_actionModel->action(ui.tvActions->selectionModel()->currentIndex());
    m_actionModel->refresh(mode);
    ui.tvActions->resizeColumnToContents(0);
    if(oldAction){
        ui.tvActions->selectionModel()->setCurrentIndex(m_actionModel->find(oldAction), QItemSelectionModel::Rows | QItemSelectionModel::SelectCurrent);
    }
    actionSelectionChanged(ui.tvActions->selectionModel()->currentIndex());
}

void KCMLirc::modeSelectionChanged(const QModelIndex &index) {
    if(index.isValid()){
        ui.pbAddMode->setEnabled(true);
        ui.pbEditMode->setEnabled(true);
        ui.pbAddAction->setEnabled(true);
        ui.pbAutoPopulate->setEnabled(true);
    } else {
        ui.pbAddMode->setEnabled(false);
        ui.pbEditMode->setEnabled(true);
        ui.pbAddAction->setEnabled(false);
        ui.pbAutoPopulate->setEnabled(false);
    }
    
    if(index.isValid() && index.parent().isValid()){
        ui.pbRemoveMode->setEnabled(true);
    } else {
        ui.pbRemoveMode->setEnabled(false);
    }
    
    Mode *mode = m_remoteModel->mode(index);
    if(mode){
        updateActions(mode);
        
        Remote *remote = m_remoteModel->remote(index);
        ui.lActions->setText(remote->name() + " (" + mode->name() + ")");
        
        if(remote->allModes().indexOf(mode) > 2){
            ui.pbMoveModeUp->setEnabled(true);
        } else {
            ui.pbMoveModeUp->setEnabled(false);
        }

        if(remote->allModes().indexOf(mode) < (remote->allModes().count() - 1)){
            ui.pbMoveModeDown->setEnabled(true);
        } else {
            ui.pbMoveModeDown->setEnabled(false);
        }

    }
    
    actionSelectionChanged(QModelIndex());
    
}

void KCMLirc::actionSelectionChanged(const QModelIndex& index) {
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


void KCMLirc::load() {
    m_remoteList.loadFromConfig("kremotecontrolrc");

    // Check if there are Remotes available in Solid but not yet in m_remoteList
    foreach(const QString &remoteName, Solid::Control::RemoteControl::allRemoteNames()){
        if(!m_remoteList.contains(remoteName)){
            Remote *remote = new Remote(remoteName);
            m_remoteList.append(remote);
        }
    }

    updateModes();
}

void KCMLirc::save() {
    m_remoteList.saveToConfig("kremotecontrolrc");
    DBusInterface::getInstance()->reloadRemoteControlDaemon();
}

void KCMLirc::gotButton(QString remote, QString button) {
    emit haveButton(remote, button);
}

void KCMLirc::actionDropped(Mode* mode) {
    ui.tvRemotes->selectionModel()->setCurrentIndex(m_remoteModel->find(mode), QItemSelectionModel::Rows | QItemSelectionModel::SelectCurrent);
    updateActions(mode);
}

#include "kcmlirc.moc"
