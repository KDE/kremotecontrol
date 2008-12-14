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

#include "addaction.h"
#include "prototype.h"
#include "profileserver.h"
#include "remoteserver.h"

#include <QRegExp>
#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusConnectionInterface>
#include <QDomDocument>

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


    connect(this, SIGNAL(currentIdChanged(int)), SLOT(updateForPageChange()));
    connect(this, SIGNAL(currentIdChanged(int)), SLOT(slotCorrectPage()));

    connect(theObjects, SIGNAL(itemSelectionChanged()), this, SLOT(updateFunctions()));
    connect(theObjects, SIGNAL(itemSelectionChanged()), this, SLOT(updateFunctions()));
    connect(theObjects, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtonStates()));

    connect(theProfiles, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtonStates()));
    connect(theProfiles, SIGNAL(itemSelectionChanged()), this, SLOT(updateProfileFunctions()));
    connect(theProfiles, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(next()));

    connect(theModes, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtonStates()));
    connect(theModes, SIGNAL(itemSelectionChanged()), this, SLOT(slotModeSelected()));
    connect(theModes, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(next()));

    connect(theButtons, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtonStates()));
    connect(theButtons, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(next()));

    connect(theProfileFunctions, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtonStates()));
    connect(theProfileFunctions, SIGNAL(itemSelectionChanged()), this, SLOT(updateParameter()));
    connect(theProfileFunctions, SIGNAL(itemSelectionChanged()), this, SLOT(updateOptions()));
    connect(theProfileFunctions, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(next()));

    connect(theUseProfile, SIGNAL(clicked()), this, SLOT(updateButtonStates()));

    connect(theUseDCOP, SIGNAL(clicked()), this, SLOT(updateButtonStates()));

    connect(theChangeMode, SIGNAL(clicked()), this, SLOT(updateButtonStates()));

    connect(theJustStart, SIGNAL(clicked()), this, SLOT(updateButtonStates()));

    connect(theNotJustStart, SIGNAL(clicked()), this, SLOT(updateButtonStates()));

    connect(theFunctions, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtonStates()));
    connect(theFunctions, SIGNAL(itemSelectionChanged()), this, SLOT(updateParameter()));
    connect(theFunctions, SIGNAL(itemSelectionChanged()), this, SLOT(updateOptions()));
    connect(theFunctions, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(next()));

    connect(theParameters, SIGNAL(itemSelectionChanged()), this, SLOT(updateParameter()));

    connect(theSwitchMode, SIGNAL(clicked()), this, SLOT(updateButtonStates()));

    connect(theExitMode, SIGNAL(clicked()), this, SLOT(updateButtonStates()));

    curPage = 0;
    updateProfiles();
    updateButtons();
    updateObjects();
    updateProfileFunctions();

}

AddAction::~AddAction()
{
}


void AddAction::slotNextParam()
{
    // TODO: go on to next parameter
}

void AddAction::slotModeSelected()
{
    theSwitchMode->setChecked(true);
}

void AddAction::slotCorrectPage()
{
    int lastPage = curPage;
    curPage = this->currentId();

    kDebug() << "lastPage:" << lastPage << "; curPage:" << curPage;

    if (theUseProfile->isChecked() || theUseDCOP->isChecked()) {
        QWizard::page(5)->setFinalPage(true);
    } else {
        QWizard::page(5)->setFinalPage(false);
    }


    if (curPage == 2 && theUseProfile->isChecked()) {
        if (lastPage > 1) {
            back();
        } else {
            next();
        }
    }
    if (curPage == 2  && theChangeMode->isChecked() && lastPage == 1) {
        next();
        next();
        next();
        next();
    }

    if (curPage == 5 && theChangeMode->isChecked() && lastPage == 6) {
        back();
        back();
        back();
        back();
    }

    if (curPage == 3 && theUseDCOP->isChecked()) {
        if (lastPage == 4) {
            back();
        } else {
            next();
        }
    }

    if (curPage == 3) {
        updateProfileFunctions();
    }

    if (curPage == 4) {
        updateParameters();
    }

    if (curPage == 4 && (
                (theUseDCOP->isChecked() && theFunctions->currentItem() && !Prototype(theFunctions->currentItem()->text(2)).count()) ||
                (theUseProfile->isChecked() && (theProfileFunctions->currentItem() && !theProfileFunctions->currentItem()->text(1).toInt())) || theJustStart->isChecked()
            )) {
//  showPage(((QWizard *)this)->page(lastPage == 5 ? (theUseDCOP->isChecked() ? 2 : 3) : 5));

        if (lastPage == 5) {
            if (theUseDCOP->isChecked()) {
                back();
//    back();
            } else {
                back();
            }
            // Restore the Wizard layout in case its modified
            QList<QWizard::WizardButton> layout;
            layout << QWizard::Stretch << QWizard::BackButton << QWizard::NextButton << QWizard::FinishButton << QWizard::CancelButton;
            setButtonLayout(layout);
        } else {
            next();
            QList<QWizard::WizardButton> layout;
            layout << QWizard::Stretch << QWizard::BackButton << QWizard::FinishButton << QWizard::CancelButton;
            setButtonLayout(layout);
        }
    }
}

void AddAction::requestNextPress()
{
    kDebug() << "Requesting next press from irkick";
    QDBusMessage m = QDBusMessage::createMethodCall("org.kde.irkick", "/IRKick", "", "stealNextPress");
    m << "org.kde.kcmshell_kcmlirc";
    m << "/KCMLirc";
    m << "gotButton";
    QDBusMessage response = QDBusConnection::sessionBus().call(m);
    if (response.type() == QDBusMessage::ErrorMessage) {
        kDebug() << response.errorMessage();
    }
}

void AddAction::cancelRequest()
{
    kDebug() << "Cancelling keypress request";
    QDBusMessage m = QDBusMessage::createMethodCall("org.kde.irkick", "/IRKick", "", "dontStealNextPress");
    QDBusMessage response = QDBusConnection::sessionBus().call(m);
    if (response.type() == QDBusMessage::ErrorMessage) {
        kDebug() << response.errorMessage();
    }
    kDebug() << "done...";
}

void AddAction::updateButton(const QString &remote, const QString &button)
{
    if (theMode.remote() == remote) { // note this isn't the "correct" way of doing it; really i should iterate throughg the items and try to find the item which when put through buttonMap[item] returns the current button name. but i cant be arsed.
        theButtons->setCurrentItem(theButtons->findItems(RemoteServer::remoteServer()->getButtonName(remote, button), 0).first());
        theButtons->scrollToItem(theButtons->findItems(RemoteServer::remoteServer()->getButtonName(remote, button), 0).first());
    } else
        KMessageBox::error(0, i18n("You did not select a mode of that remote control. Please use %1, "
                                   "or revert back to select a different mode.", theMode.remoteName()),
                           i18n("Incorrect Remote Control Detected"));

    updateButtonStates();

}

void AddAction::updateButtons()
{
    theButtons->clear();
    buttonMap.clear();
    QDBusMessage m = QDBusMessage::createMethodCall("org.kde.irkick", "/IRKick", "", "buttons");
    m << theMode.remote();
    QDBusMessage response = QDBusConnection::sessionBus().call(m);

    if (response.type() == QDBusMessage::ErrorMessage) {
        kDebug() << response.errorMessage();
    }

    kDebug() << "Got response: " << response.arguments();

    QStringList buttons = response.arguments().at(0).toStringList();

    for (QStringList::iterator j = buttons.begin(); j != buttons.end(); ++j)
        buttonMap[new QListWidgetItem(RemoteServer::remoteServer()->getButtonName(theMode.remote(), *j), theButtons)] = *j;
}

void AddAction::updateForPageChange()
{
    if (currentId() == 1) requestNextPress(); //else cancelRequest();
    updateButtonStates();
}

void AddAction::updateButtonStates()
{
    kDebug() << "Updating button states";
    switch (currentId()) {
    case 0:
        button(QWizard::NextButton)->setEnabled(!theProfiles->selectedItems().isEmpty() || !theUseProfile->isChecked());
        break;
    case 1:
        button(QWizard::NextButton)->setEnabled(!theButtons->selectedItems().isEmpty());
        break;
    case 2:
        button(QWizard::NextButton)->setEnabled(theFunctions->currentItem() != 0);
        break;
    case 3:
        button(QWizard::NextButton)->setEnabled(theProfileFunctions->currentItem() != 0 || theJustStart->isChecked());
        break;
    case 4:
        button(QWizard::NextButton)->setEnabled(true);
        break;
    case 5:
        button(QWizard::NextButton)->setEnabled(true);
        button(QWizard::FinishButton)->setEnabled(true);
        break;
    case 6:
        button(QWizard::NextButton)->setEnabled(false);
        button(QWizard::FinishButton)->setEnabled(theModes->currentItem() || !theSwitchMode->isChecked());
        break;
    }
}

const QStringList AddAction::getFunctions(const QString app, const QString obj)
{
    kDebug() << "creating Interface with: " << app << '/' + obj << "org.freedesktop.DBus.Introspectable";
    QDBusInterface *dBusIface = new QDBusInterface(app, '/' + obj, "org.freedesktop.DBus.Introspectable");
    QDBusReply<QString> response = dBusIface->call("Introspect");

    kDebug() << response;
    QDomDocument domDoc;
    domDoc.setContent(response);

    QDomElement node = domDoc.documentElement();
    QDomElement child = node.firstChildElement();

    QStringList ret;
    QString function;

    while (!child.isNull()) {
        if (child.tagName() == QLatin1String("interface")) {
            if (child.attribute("name") == "org.freedesktop.DBus.Properties" ||
                    child.attribute("name") == "org.freedesktop.DBus.Introspectable") {
                child = child.nextSiblingElement();
                continue;
            }
            QDomElement subChild = child.firstChildElement();
            while (!subChild.isNull()) {
                if (subChild.tagName() == QLatin1String("method")) {
                    QString method = subChild.attribute(QLatin1String("name"));
                    kDebug() << "Method: " << method;
                    function = "QString " + method + '(';
                    QDomElement arg = subChild.firstChildElement();
                    QString argStr;
                    while (!arg.isNull()) {
                        if (arg.tagName() == QLatin1String("arg")) {
                            if (arg.attribute(QLatin1String("direction")) == "in") {
                                if (!argStr.isEmpty()) {
                                    argStr += ", ";
                                }
                                if (arg.attribute(QLatin1String("type")) == "i") {
                                    argStr += "int";
                                } else if (arg.attribute(QLatin1String("type")) == "u") {
                                    argStr += "uint";
                                } else if (arg.attribute(QLatin1String("type")) == "s") {
                                    argStr += "QString";
                                } else if (arg.attribute(QLatin1String("type")) == "b") {
                                    argStr += "bool";
                                } else if (arg.attribute(QLatin1String("type")) == "d") {
                                    argStr += "double";
                                } else if (arg.attribute(QLatin1String("type")) == "as") {
                                    argStr += "QStringList";
                                } else if (arg.attribute(QLatin1String("type")) == "ay") {
                                    argStr += "QByteArray";
                                } else if (arg.attribute(QLatin1String("type")) == "(iii)") {
                                    kDebug() << "got a (iii) type";
                                    QString helper = arg.attribute("name");
                                    arg = arg.nextSiblingElement();
                                    argStr += arg.attribute(QLatin1String("value"));
                                    argStr += ' ' + helper;
                                    arg = arg.nextSiblingElement();
                                    continue;
                                } else {
                                    argStr += arg.attribute(QLatin1String("type"));
                                }
                                argStr += ' ' + arg.attribute(QLatin1String("name"));
                                kDebug() << "Arg: " << argStr;
                            }
                        }
                        arg = arg.nextSiblingElement();
                    }
                    function +=  argStr + ')';
                    ret += function;
                }
                subChild = subChild.nextSiblingElement();
            }
        }
        child = child.nextSiblingElement();
    }
    return ret;
}

void AddAction::updateProfiles()
{
    ProfileServer *theServer = ProfileServer::profileServer();
    theProfiles->clear();
    profileMap.clear();

    QHash<QString, Profile*> dict = theServer->profiles();
    QHash<QString, Profile*>::const_iterator i;
    for (i = dict.constBegin(); i != dict.constEnd(); ++i)
        profileMap[new QListWidgetItem(i.value()->name(), theProfiles)] = i.key();
}

void AddAction::updateOptions()
{
    IfMulti im;
    if (theUseProfile->isChecked()) {
        ProfileServer *theServer = ProfileServer::profileServer();
        if (!theProfiles->currentItem()) return;
        const Profile *p = theServer->profiles()[profileMap[theProfiles->currentItem()]];
        im = p->ifMulti();
        isUnique = p->unique();
    } else if (theUseDCOP->isChecked()) {
        if (!theObjects->selectedItems().first()) return;
        QTreeWidgetItem* i = theObjects->selectedItems().first()->parent();
        if (!i) return;
        isUnique = uniqueProgramMap[i];
        QRegExp r("(.*)-[0-9]+");
        program = r.exactMatch(nameProgramMap[i]) ? r.cap(1) : nameProgramMap[i];
        im = IM_DONTSEND;
    } else return;

    theIMLabel->setEnabled(!isUnique);
// theIMGroup->setEnabled(!isUnique);
    theIMLine->setEnabled(!isUnique);
    theIMTop->setEnabled(!isUnique);
    theDontSend->setEnabled(!isUnique);
    theSendToBottom->setEnabled(!isUnique);
    theSendToTop->setEnabled(!isUnique);
    theSendToAll->setEnabled(!isUnique);
    switch (im) {
    case IM_DONTSEND: theDontSend->setChecked(true); break;
    case IM_SENDTOTOP: theSendToTop->setChecked(true); break;
    case IM_SENDTOBOTTOM: theSendToBottom->setChecked(true); break;
    case IM_SENDTOALL: theSendToAll->setChecked(true); break;
    }
}

void AddAction::updateProfileFunctions()
{
    kDebug() << "updateProfileFunctions called";
    ProfileServer *theServer = ProfileServer::profileServer();
    theProfileFunctions->clear();
    profileFunctionMap.clear();
    if (theProfiles->selectedItems().isEmpty()) {
        return;
    }

    const Profile *p = theServer->profiles()[profileMap[theProfiles->currentItem()]];
    QHash<QString, ProfileAction*> dict = p->actions();
    kDebug() << "actions: " << p->actions();
    QHash<QString, ProfileAction*>::const_iterator i;
    for (i = dict.constBegin(); i != dict.constEnd(); ++i) {
        kDebug() << "got function: " << i.value()->name();
        QStringList parameters;
        parameters << i.value()->name() << QString().setNum(i.value()->arguments().count()) << i.value()->comment();
        profileFunctionMap[new QTreeWidgetItem(theProfileFunctions, parameters)] = i.key();
    }
    updateParameters();
    updateOptions();
}

void AddAction::updateParameters()
{
    theParameters->clear();
    kDebug() << "clearing arguments";
    theArguments.clear();
    if (theUseDCOP->isChecked() && theFunctions->currentItem()) {
        Prototype p(theFunctions->currentItem()->text(2));
        for (int k = 0; k < p.count(); k++) {
            QStringList parameters;
            parameters << (p.name(k).isEmpty() ? i18nc("Unknown parameter name in function", "&lt;anonymous&gt;") : p.name(k)) << "" << p.type(k) << QString().setNum(k + 1);
            new QTreeWidgetItem(theParameters, parameters);
            theArguments.append(QVariant(""));
            kDebug() << "converting argument to:" << p.type(k).toLocal8Bit();
            theArguments.back().convert(QVariant::nameToType(p.type(k).toLocal8Bit()));
        }
    } else if (theUseProfile->isChecked() && theProfiles->currentItem()) {
        ProfileServer *theServer = ProfileServer::profileServer();

        if (!theProfiles->currentItem()) return;
        if (!theProfileFunctions->currentItem()) return;
        const Profile *p = theServer->profiles()[profileMap[theProfiles->currentItem()]];
        const ProfileAction *pa = p->actions()[profileFunctionMap[theProfileFunctions->currentItem()]];

        int index = 1;
        for (int i = 0; i < pa->arguments().size(); ++i, index++) {
            theArguments.append(QVariant((pa->arguments().at(i)).getDefault()));
            kDebug() << "converting argument to:" << QVariant::nameToType(pa->arguments().at(i).type().toLocal8Bit());
            theArguments.back().convert(QVariant::nameToType(pa->arguments().at(i).type().toLocal8Bit()));
            QStringList parameters;
            parameters << pa->arguments().at(i).comment() << theArguments.back().toString() << pa->arguments().at(i).type() << QString().setNum(index);
            new QTreeWidgetItem(theParameters, parameters);
        }

        // quicky update options too...
        theRepeat->setChecked(pa->repeat());
        theAutoStart->setChecked(pa->autoStart());
    }

    updateParameter();
}

void AddAction::updateParameter()
{
    kDebug() << "Update parameter called";
    if (!theParameters->selectedItems().isEmpty()) {
        QString type = theParameters->currentItem()->text(2);
        int index = theParameters->currentItem()->text(3).toInt() - 1;
        kDebug() << "Parameter type:" << type;
        if (type.contains("int") || type.contains("short") || type.contains("long") || type.contains("uint"))
//  if(type.contains("i"))
        {
            theValue->setCurrentIndex(2);
            theValueIntNumInput->setValue(theArguments[index].toInt());
        } else if (type.contains("double") || type.contains("float")) {
            theValue->setCurrentIndex(3);
            theValueDoubleNumInput->setValue(theArguments[index].toDouble());
        } else if (type.contains("bool")) {
            theValue->setCurrentIndex(1);
            theValueCheckBox->setChecked(theArguments[index].toBool());
        } else if (type.contains("QStringList")) {
            theValue->setCurrentIndex(4);
            QStringList backup = theArguments[index].toStringList();
            // backup needed because calling clear will kill what ever has been saved.
            theValueEditListBox->clear();
            theValueEditListBox->insertStringList(backup);
            theArguments[index].toStringList() = backup;
        } else {
            theValue->setCurrentIndex(0);
            theValueLineEdit->setText(theArguments[index].toString());
        }
        theCurParameter->setText(theParameters->currentItem()->text(0));
        theCurParameter->setEnabled(true);
        theValue->setEnabled(true);
    } else {
        theCurParameter->setText("");
        theValueLineEdit->setText("");
        theValueCheckBox->setChecked(false);
        theValueIntNumInput->setValue(0);
        theValueDoubleNumInput->setValue(0.0);
        theCurParameter->setEnabled(false);
        theValue->setEnabled(false);
    }
}

// called when the textbox/checkbox/whatever changes value
void AddAction::slotParameterChanged()
{
    kDebug() << "slotParameterChanged() called";
    if (!theParameters->currentItem()) return;
    int index = theParameters->currentItem()->text(3).toInt() - 1;
    QString type = theParameters->currentItem()->text(2);
    if (type.contains("int") || type.contains("short") || type.contains("long")) {
        theArguments[index] = theValueIntNumInput->value();
    } else if (type.contains("double") || type.contains("float")) {
        theArguments[index] = theValueDoubleNumInput->value();
    } else if (type.contains("bool")) {
        theArguments[index] = theValueCheckBox->isChecked();
    } else if (type.contains("QStringList")) {
        theArguments[index] = theValueEditListBox->items();
    } else {
        theArguments[index] = theValueLineEdit->text();
        kDebug() << "setting argument" << theArguments[index];
    }

// kDebug() << "setting argument nr: " << index << " to:" << theValueLineEdit->text() << "type is:" << type;
    theArguments[theParameters->currentItem()->text(3).toInt() - 1].convert(QVariant::nameToType(theParameters->currentItem()->text(2).toLocal8Bit()));
    updateArgument(theParameters->currentItem());
}

// takes theArguments[theIndex] and puts it into theItem
void AddAction::updateArgument(QTreeWidgetItem *theItem)
{
    kDebug() << "theArgument:" << theArguments[theItem->text(3).toInt() - 1] << "index:" << theItem->text(3).toInt() - 1;
    theItem->setText(1, theArguments[theItem->text(3).toInt() - 1].toString());
}

void AddAction::updateObjects()
{
    QStringList names;
    theObjects->clear();
    uniqueProgramMap.clear();
    nameProgramMap.clear();

    QDBusConnectionInterface *dBusIface = QDBusConnection::sessionBus().interface();
    QStringList allServices = dBusIface->registeredServiceNames();

    QStringList kdeServices;

    for (QStringList::iterator i = allServices.begin(); i != allServices.end(); ++i) {
        // Use only KDE-Apps
        if (!(*i).contains("org.kde")) {
            continue;
        }

        // Remove the "org.kde."
        QString name = (*i);
        name.remove(0, 8);

	// strip trailing numbers
	QRegExp r1("[a-zA-Z]*-[0-9]*");
	if(r1.exactMatch(name)){
	    name.truncate(name.indexOf('-'));
	}

        // Remove "human unreadable" entries
        QRegExp r2("[a-zA-Z]*");
        if (! r2.exactMatch(name)) {
            continue;
        }

        //remove duplicates
        if (names.contains(name)) {
            continue;
        }
        names += name;

        // insert service into theObjects
        QStringList tmpList;
        tmpList << name;
        QTreeWidgetItem *a = new QTreeWidgetItem(theObjects, tmpList);
        uniqueProgramMap[a] = name == QString(*i);
        nameProgramMap[a] = *i;

        QDBusInterface *dBusIface = new QDBusInterface(*i, "/", "org.freedesktop.DBus.Introspectable");
        QDBusReply<QString> response = dBusIface->call("Introspect");

        QDomDocument domDoc;
        domDoc.setContent(response);

        QDomElement node = domDoc.documentElement();
        QDomElement child = node.firstChildElement();
        while (!child.isNull()) {
            if (child.tagName() == QLatin1String("node")) {
                QStringList path;
                path << child.attribute(QLatin1String("name"));
                kDebug() << "path: " << path;
                new QTreeWidgetItem(a, path);
            }
            child = child.nextSiblingElement();
        }
    }
    updateFunctions();
}

void AddAction::updateFunctions()
{
    kDebug() << "updateFunctions called";
    theFunctions->clear();
    if (theObjects->currentItem() && theObjects->currentItem()->parent()) {
        QStringList functions = getFunctions(nameProgramMap[theObjects->currentItem()->parent()], theObjects->currentItem()->text(0));
        for (QStringList::iterator i = functions.begin(); i != functions.end(); ++i) {
            Prototype p((QString)(*i));
            QStringList parameters;
            parameters << p.name() << p.argumentList() << *i;
            new QTreeWidgetItem(theFunctions, parameters);
        }
    }
    updateOptions();
}

#include "addaction.moc"
