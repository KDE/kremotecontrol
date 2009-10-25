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
  * @author Gav Wood
  */


#include "kcmlirc.h"
#include "addaction.h"
#include "newmodedialog.h"
#include "profileserver.h"

#include "editaction.h"
#include "editmode.h"
#include "dbusinterface.h"
#include "model.h"

#include <qevent.h>
#include <QHBoxLayout>
#include <QWidget>
#include <kapplication.h>
#include <klocale.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kicondialog.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <kconfiggroup.h>
#include <kgenericfactory.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <ktoolinvocation.h>
#include <kaboutdata.h>
#include<QStandardItemModel>

#define VERSION "version name goes here"

#include "selectprofile.h"

K_PLUGIN_FACTORY( KCMLircFactory, registerPlugin<KCMLirc>();)
K_EXPORT_PLUGIN( KCMLircFactory( "kcm_lirc" ) )

KCMLirc::KCMLirc(QWidget *parent, const QVariantList &args) :
        KCModule(KCMLircFactory::componentData(), parent, args)
{

    qRegisterMetaType<IRAction*>("IRAction*");
    qRegisterMetaType<Mode>("Mode");

    KGlobal::locale()->insertCatalog("kcm_lirc");
    setAboutData(
        new KAboutData(
            "kcm_lirc",
            0,
            ki18n("KDE Lirc"),
            VERSION, ki18n("The KDE IR Remote Control System"),
            KAboutData::License_GPL_V2,
            ki18n("Copyright (c)2003 Gav Wood"),
            ki18n(
                "Use this to configure KDE's infrared remote control system in order to control any KDE application with your infrared remote control."),
            "http://utils.kde.org/projects/kdelirc"));
    setQuickHelp(
        i18n(
            "<h1>Remote Controls</h1><p>This module allows you to configure bindings between your remote controls and KDE applications. Simply select your remote control and click Add under the Actions/Buttons list. If you want KDE to attempt to automatically assign buttons to a supported application's actions, try clicking the Auto-Populate button.</p><p>To view the recognised applications and remote controls, simply select the <em>Loaded Profiles</em> tab.</p>"));

    if (!DBusInterface::getInstance()->isProgramRunning("org.kde.irkick")) {
        if (KMessageBox::questionYesNo(
                    this,
                    i18n(
                        "The Infrared Remote Control software is not currently running. This configuration module will not work properly without it. Would you like to start it now?"),
                    i18n("Software Not Running"), KGuiItem(i18n("Start")), KGuiItem(
                        i18n("Do Not Start"))) == KMessageBox::Yes) {
            kDebug() << "S" << KToolInvocation::startServiceByDesktopName("irkick");
            KConfig theConfig("irkickrc");
            KConfigGroup generalGroup = theConfig.group("General");
            if (generalGroup.readEntry("AutoStart", true) == false)
                if (KMessageBox::questionYesNo(
                            this,
                            i18n(
                                "Would you like the infrared remote control software to start automatically when you begin KDE?"),
                            i18n("Automatically Start?"), KGuiItem(i18n("Start Automatically")),
                            KGuiItem(i18n("Do Not Start"))) == KMessageBox::Yes)
                    generalGroup.writeEntry("AutoStart", true);
        }
    }

    QHBoxLayout *layout = new QHBoxLayout(this);

    QWidget *widget = new QWidget(this);
    theKCMLircBase = new Ui::KCMLircBase();
    theKCMLircBase->setupUi(widget);

    QStringList headers = (QStringList() << i18nc("Column which shows the available remotes on system", "Remote"));
    theKCMLircBase->theModes->setHeaderLabels(headers);
    layout->addWidget(widget);
    connectSignalsAndSlots();
    load();
}

void KCMLirc::connectSignalsAndSlots() {
    connect(theKCMLircBase->theModes, SIGNAL(itemSelectionChanged()), this, SLOT(updateActions()));
    connect(theKCMLircBase->theModes, SIGNAL(itemSelectionChanged()), this, SLOT(updateModesStatus()));
    connect(theKCMLircBase->theActions, SIGNAL(itemSelectionChanged()), this, SLOT(updateActionsStatus()));
    connect(theKCMLircBase->theActions, SIGNAL(doubleClicked(QModelIndex)),this,SLOT(slotEditAction()));

    connect(theKCMLircBase->theAvailableProfiles, SIGNAL(itemSelectionChanged()), this, SLOT(updateInformation()));

    connect(theKCMLircBase->theAddActions, SIGNAL(clicked()), this, SLOT(slotAddActions()));
    connect(theKCMLircBase->theAddAction, SIGNAL(clicked()), this, SLOT(slotAddAction()));
    connect(theKCMLircBase->theEditAction, SIGNAL(clicked()), this, SLOT(slotEditAction()));

    connect(theKCMLircBase->theRemoveAction, SIGNAL(clicked()), this, SLOT(slotRemoveAction()));
    connect(theKCMLircBase->theAddMode, SIGNAL(clicked()), this, SLOT(slotAddMode()));
    connect(theKCMLircBase->theEditMode, SIGNAL(clicked()), this, SLOT(slotEditMode()));
    connect(theKCMLircBase->theRemoveMode, SIGNAL(clicked()), this, SLOT(slotRemoveMode()));
}



KCMLirc::~KCMLirc()
{
}

void KCMLirc::updateModesStatus()
{
    if (! theKCMLircBase->theModes->selectedItems().isEmpty()) {
        QTreeWidgetItem *item = theKCMLircBase->theModes->selectedItems().first();
        bool remoteSelected = item->isSelected();
        theKCMLircBase->theAddActions->setEnabled(remoteSelected && ProfileServer::getInstance()->profiles().count());
        theKCMLircBase->theAddAction->setEnabled(remoteSelected);
        theKCMLircBase->theAddMode->setEnabled(remoteSelected);
        theKCMLircBase->theRemoveMode->setEnabled(item->parent() != 0);
        theKCMLircBase->theEditMode->setEnabled(remoteSelected);
    }
}

void KCMLirc::updateActionsStatus()
{
    theKCMLircBase->theRemoveAction->setEnabled(theKCMLircBase->theActions->currentIndex().isValid());
    theKCMLircBase->theEditAction->setEnabled(theKCMLircBase->theActions->currentIndex().isValid());
}

void KCMLirc::slotEditAction()
{
    QTreeWidgetItem *item = theKCMLircBase->theModes->selectedItems().first();
    if (item->parent())
        item = item->parent();
    QStringList modeList;
    for (int i = 0; i < item->childCount(); i++) {
        modeList << item->child(i)->text(0);
    }
    EditAction theDialog(currentAction(), modeList);
    if (theDialog.exec() == QDialog::Accepted) {
        allActions[allActions.indexOf(currentAction())] = theDialog.getAction();
        updateActions();
	emit changed(true);
    }
}

void KCMLirc::slotAddActions()
{
    if (theKCMLircBase->theModes->selectedItems().isEmpty()) {
        return;
    }
    Mode mode = theKCMLircBase->theModes->currentItem()->data(0, Qt::UserRole).value<Mode>();
     SelectProfile *tDialog = new SelectProfile( mode.remote());
    if (tDialog->exec() == QDialog::Accepted) {
      autoPopulate(tDialog->getSelectedProfile(),mode);	
      updateActions();
      emit changed(true);
    }
}

void KCMLirc::slotAddAction()
{
    kDebug();
    if (theKCMLircBase->theModes->selectedItems().isEmpty()){
        return;
    }
    Mode m = theKCMLircBase->theModes->currentItem()->data(0, Qt::UserRole).value<Mode>();
    AddAction theDialog(this, 0, m);

    // populate the modes list box
    QTreeWidgetItem *item = theKCMLircBase->theModes->selectedItems().first();
    if (item->parent())
        item = item->parent();
    theDialog.theModes->setEnabled(item->child(0));
    theDialog.theSwitchMode->setEnabled(item->child(0));
    for (int i = 0; i < item->childCount(); i++) {
        QListWidgetItem *a = new QListWidgetItem(item->child(i)->text(0),
                theDialog.theModes);
        if (item->isSelected()) {
            a->setSelected(true);
            theDialog.theModes->setCurrentItem(a);
        }
    }

    if (theDialog.exec() == QDialog::Accepted) {
        allActions.addAction(theDialog.getAction());
        updateActions();
        emit changed(true);
    }    
}

void KCMLirc::slotRemoveAction()
{
    allActions.erase(currentAction());
    updateActions();
    emit changed(true);
}

void KCMLirc::autoPopulate(const Profile *profile,const Mode &mode)
{
    QStringList buttonList = DBusInterface::getInstance()->getButtons(mode.remote());

    foreach (QString button, buttonList ) {
        const ProfileAction *pa = profile->getProfileActionByButton(button);
        if (pa) {
            IRAction *action = new IRAction(mode.remote(),button);
            action->setMode(mode.name());
            action->setRepeat(pa->repeat());
            action->setAutoStart(pa->autoStart());
            action->setProgram(pa->profile()->id());
            action->setObject(pa->objId());
            action->setMethod(pa->prototype());
            action->setUnique(pa->profile()->unique());
            action->setIfMulti(pa->profile()->ifMulti());
            Arguments arguments;
            kDebug() << "Argumentcount" << Prototype(pa->prototype()).argumentCount();

            for (int j = 0; j < pa->arguments().size(); ++j) {
                ProfileActionArgument tArgument= pa->arguments().at(j);
                QVariant tVariant;
                if (!tArgument.getDefault().toString().isEmpty()) {
                    tVariant = tArgument.getDefault();
                    tVariant.convert(QVariant::nameToType(tArgument.type().toLocal8Bit()));
                } else {
                    tVariant = QString().setNum(pa->multiplier());
                    tVariant.convert(QVariant::nameToType(Prototype(pa->prototype()).type(0).toLocal8Bit()));
                }
                arguments.append(tVariant);
                kDebug() << "added argument: " << tVariant;
            }
            action->setArguments(arguments);
            allActions.addAction(action);
        }
    }
}

void KCMLirc::slotAddMode()
{
    if (theKCMLircBase->theModes->selectedItems().isEmpty()) {
        return;
    }
    NewModeDialog theDialog(allModes, this,0);
    if (theDialog.exec() == QDialog::Accepted) {
        Mode mode = theDialog.getMode();
        allModes.add(mode);
        if (theDialog.isDefaultMode()) {
            allModes.setDefault(mode);
        }
        updateModes();
        emit changed(true);
    }
}

void KCMLirc::slotEditMode()
{
    if (theKCMLircBase->theModes->selectedItems().isEmpty())
        return;

    Mode mode = theKCMLircBase->theModes->currentItem()->data(0, Qt::UserRole).value<Mode>();
    EditMode theDialog(mode,allModes, this, 0);

    if (theDialog.exec() == QDialog::Accepted) {
        Mode newMode = theDialog.getMode();
        mode.setIconFile(newMode.iconFile());
        if (!mode.name().isEmpty()) {
            allActions.renameMode(mode, newMode.name());
            allModes.rename(mode, newMode.name());
        }
        if (theDialog.isDefaultMode()) {
            allModes.setDefault(mode);
        }
        allModes.updateMode(mode);
        emit changed(true);
        updateModes();
    }
}

void KCMLirc::slotRemoveMode()
{

    Mode tMode = theKCMLircBase->theModes->currentItem()->data(0, Qt::UserRole).value<Mode>();
    if (KMessageBox::warningContinueCancel(this, i18n(
                                               "Are you sure you want to remove %1 and all its actions?",
                                               tMode.name()), i18n(
                                               "Erase Actions?")) == KMessageBox::Continue) {
        theKCMLircBase->theModes->clear();
        IRActions allActionsList = allActions.findByMode(tMode);
        foreach(IRAction *tAction, allActionsList) {
            allActions.erase(tAction);
        }

        allModes.erase(tMode);
        tMode.setName("");
        foreach (IRAction *tAction ,  allActions.findByMode(tMode)) {
            if (tAction->isModeChange() &&  tAction->mode() == tMode.name()) {
                tAction->setProgram("");
                tAction->setObject("");
                tAction->setAutoStart(false);
                tAction->setRepeat(false);
            }
        }
        updateModes();
        emit changed(true);
    }

}

void KCMLirc::slotSetDefaultMode()
{
    if (!theKCMLircBase->theModes->currentItem()) {
        return;
    }
    allModes.setDefault(theKCMLircBase->theModes->currentItem()->data(0, Qt::UserRole).value<Mode>());
    updateModes();
    emit changed(true);
}

void KCMLirc::updateActions()
{
    if (!theKCMLircBase->theModes->currentItem()) {
        return;
    }

    IRAction *oldCurrent = 0;
    if (theKCMLircBase->theActions->currentIndex().isValid()) {
        oldCurrent = currentAction();
    }
    theKCMLircBase->theActions->clear();
    Mode m = theKCMLircBase->theModes->currentItem()->data(0, Qt::UserRole).value<Mode>();
    theKCMLircBase->theModeLabel->setText(m.remote() + ": "
                                          + (m.name().isEmpty() ? i18n("Actions <i>always</i> available") : i18n(
                                                 "Actions available only in mode <b>%1</b>", m.name())));
    IRActions allActionsList = allActions.findByMode(m);

    foreach(IRAction *tmp, allActionsList) {
        QStringList row;
	if(tmp->getButton().id() == RemoteControlButton::Unknown){
	  row << tmp->button(); 
	}else{
	  row << tmp->button() + " (" +tmp->getButton().description() + ")";
	}
	
	row << tmp->application() << tmp->function() << tmp->arguments().toString() << notes(tmp);
        QTreeWidgetItem *actionItem = new  QTreeWidgetItem(row);
        actionItem->setData(0, Qt::UserRole, qVariantFromValue(tmp));
        theKCMLircBase->theActions->addTopLevelItem(actionItem);
    }
    updateActionsStatus();
}


const QString KCMLirc::notes(IRAction *action) const
{

    if (action->isModeChange())
        return QString(action->doBefore() ? i18n("Do actions before. ") : "") +
               QString(action->doAfter() ? i18n("Do actions after. ") : "");
    else if (action->isJustStart())
        return "";
    else
        return QString(action->autoStart() ? i18n("Auto-start. ") : "")
               + QString(action->repeat() ? i18n("Repeatable. ") : "")
               + QString(!action->unique() ? (action->ifMulti() == IM_DONTSEND ? i18n("Do nothing if many instances. ")
                                       : action->ifMulti()== IM_SENDTOTOP ? i18n("Send to top instance. ")
                                       : action->ifMulti() == IM_SENDTOBOTTOM ? i18n("Send to bottom instance. ") : i18n("Send to all instances. "))
                                 : "");
}

void KCMLirc::updateModes()
{
    Mode currentSelectedMode;
    if (theKCMLircBase->theModes->currentItem()) {
        currentSelectedMode = theKCMLircBase->theModes->currentItem()->data(0, Qt::UserRole).value<Mode>();
    }

    theKCMLircBase->theModes->clear();


    QStringList remotes = DBusInterface::getInstance()->getRemotes();

    if (remotes.begin() == remotes.end()) {
        theKCMLircBase->theMainLabel->setMaximumSize(32767, 32767);
        return;
    }
    else {
        theKCMLircBase->theMainLabel->setMaximumSize(0, 0);
    }

    for (QStringList::iterator i = remotes.begin(); i != remotes.end(); ++i) {
        Mode mode = allModes.getMode(*i, "");
        if (mode.remote().isEmpty()) {
            mode.setRemote(*i);
            allModes.add(mode);
        }
        QStringList remoteList;

        remoteList << *i;

        QFont font = KApplication::font();
        font.setBold(true);
        QFontMetrics fm(font);
        QFont tFont = QFont();
        tFont.setBold(allModes.isDefault(mode));
        QTreeWidgetItem *remoteTreeWidgetIcon = new QTreeWidgetItem(theKCMLircBase->theModes, remoteList);
        remoteTreeWidgetIcon->setFont(0, tFont);
        remoteTreeWidgetIcon->setExpanded(true);
        remoteTreeWidgetIcon->setToolTip(0, *remoteList.begin());
        remoteTreeWidgetIcon->setToolTip(1, *(--remoteList.end()));

        if (!mode.iconFile().isNull())
            remoteTreeWidgetIcon->setIcon(0, KIconLoader().loadIcon(mode.iconFile(), KIconLoader::Panel));
        remoteTreeWidgetIcon->setData(0, Qt::UserRole, qVariantFromValue(mode)); // the null mode

        if (remoteTreeWidgetIcon->data(0, Qt::UserRole).value<Mode>() == currentSelectedMode) {
            theKCMLircBase->theModes->setCurrentItem(remoteTreeWidgetIcon);
        }

        ModeList modeList = allModes.getModes(*i);
        for (ModeList::iterator modeListIter = modeList.begin(); modeListIter != modeList.end(); ++modeListIter)
            if (! modeListIter->name().isEmpty()) {
                QStringList modeList;
                modeList << (*modeListIter).name();
                modeList << (modeListIter->iconFile().isNull() ? "" : "");
                if (fm.width((*modeListIter).name()) + 70 > theKCMLircBase->theModes->columnWidth(0)) {
                    theKCMLircBase->theModes->setColumnWidth(0, fm.width((*modeListIter).name()) + 70);
                }
                QTreeWidgetItem *modeWidgetItem = new QTreeWidgetItem(remoteTreeWidgetIcon, modeList);
                tFont.setBold(allModes.isDefault(*modeListIter));
                modeWidgetItem->setFont(0, tFont);
                if (! modeListIter->iconFile().isNull()) {
                    modeWidgetItem->setIcon(0, KIconLoader().loadIcon((*modeListIter).iconFile(), KIconLoader::Panel));
                }
                modeWidgetItem->setToolTip(0,modeListIter->name());
                modeWidgetItem->setToolTip(1,modeListIter->name());
                modeWidgetItem->setData(0, Qt::UserRole, qVariantFromValue(*modeListIter));
                if (*modeListIter == currentSelectedMode) {
                    theKCMLircBase->theModes->setCurrentItem(modeWidgetItem);
                }
            }
        remoteTreeWidgetIcon->sortChildren(0, Qt::AscendingOrder);
    }
    if (theKCMLircBase->theModes->currentItem() == 0) {
        theKCMLircBase->theModes->setCurrentItem(theKCMLircBase->theModes->topLevelItem(0));
    }
    theKCMLircBase->theModes->currentItem()->setSelected(true);
    updateModesStatus();
    updateActions();
}

void KCMLirc::updateExtensions()
{
    theKCMLircBase->theAvailableProfiles->clear();
    QTreeWidgetItem *a = new QTreeWidgetItem(theKCMLircBase->theAvailableProfiles, (QStringList() << i18n("Applications")));
    a->setExpanded(true);
    foreach(Profile *tmp, ProfileServer::getInstance()->profiles()) {
	QTreeWidgetItem *item = new  QTreeWidgetItem(a, (QStringList()<< tmp->name())); 
	//QListWidgetItem *item = new QListWidgetItem(tmp->name());
        item->setData(0,Qt::UserRole, tmp->id());
	
    }
    a->sortChildren(1, Qt::AscendingOrder);
    theKCMLircBase->theAvailableProfiles->setCurrentItem(theKCMLircBase->theAvailableProfiles->topLevelItem(0));
    updateInformation();
}

void KCMLirc::updateInformation()
{
    theKCMLircBase->theInformation->clear();
    theKCMLircBase->theInformationLabel->setText("");

    if (theKCMLircBase->theAvailableProfiles->selectedItems().isEmpty()) {
        return;
    }

    if (!theKCMLircBase->theAvailableProfiles->selectedItems().first()->parent()) {
        theKCMLircBase->theInformationLabel->setText(i18n(
                    "Information on <b>%1</b>:",
                    theKCMLircBase->theAvailableProfiles->selectedItems().first()->text(0)));
        if (theKCMLircBase->theAvailableProfiles->selectedItems().first()->text(0) == i18n(
                    "Applications")) {
            QStringList infoList;
            infoList << i18n("Number of Applications") << QString().setNum(
                theKCMLircBase->theAvailableProfiles->selectedItems().first()->childCount());
            new QTreeWidgetItem(theKCMLircBase->theInformation, infoList);
        } else if (theKCMLircBase->theAvailableProfiles->selectedItems().first()->text(0)
                   == i18n("Remote Controls")) {
            QStringList infoList;
            infoList << i18n("Number of Remote Controls") << QString().setNum(
                theKCMLircBase->theAvailableProfiles->selectedItems().first()->childCount());
            new QTreeWidgetItem(theKCMLircBase->theInformation, infoList);
        }
    } else if (theKCMLircBase->theAvailableProfiles->selectedItems().first()->parent()->text(
                   0) == i18n("Applications")) {
        ProfileServer *theServer = ProfileServer::getInstance();
        const Profile *p = theServer->getProfileById(theKCMLircBase->theAvailableProfiles->selectedItems().first()->data(0, Qt::UserRole).toString());
        QStringList infoList;
        infoList << i18n("Profile Name") << p->name();
        new QTreeWidgetItem(theKCMLircBase->theInformation, infoList);
        infoList.clear();
        infoList << i18n("Profile Author") << p->author();
        new QTreeWidgetItem(theKCMLircBase->theInformation, infoList);
        infoList.clear();
        infoList << i18n("Application Identifier") << p->id();
        new QTreeWidgetItem(theKCMLircBase->theInformation, infoList);
        infoList.clear();
        infoList << i18n("Number of Actions") << QString().setNum(
            p->actions().count());
        new QTreeWidgetItem(theKCMLircBase->theInformation, infoList);
        theKCMLircBase->theInformationLabel->setText(i18n(
                    "Information on <b>%1</b>:", p->name()));
    }
}

void KCMLirc::load()
{
    KConfig theConfig("irkickrc");
    allActions.loadFromConfig(theConfig);
    allModes.loadFromConfig(theConfig);
    QStringList remotes = DBusInterface::getInstance()->getRemotes();

    allModes.generateNulls(remotes);

    updateExtensions();
    updateModes();
    updateActions();
}

void KCMLirc::defaults()
{
    // insert your default settings code here...
    emit changed(true);
}

void KCMLirc::save()
{
    KConfig theConfig("irkickrc");
    allActions.saveToConfig(theConfig);
    allModes.saveToConfig(theConfig);

    theConfig.sync();

    DBusInterface::getInstance()->reloadIRKick();

    emit changed(true);
}

void KCMLirc::configChanged()
{
    // insert your saving code here...
    emit changed(true);
}
#include "kcmlirc.moc"
