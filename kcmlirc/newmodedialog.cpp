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


#include "newmodedialog.h"
#include "mode.h"

NewModeDialog::NewModeDialog(const Modes &allModes, QWidget *parent, const bool &modal): KDialog(parent), allModes(allModes)
{
    newModeBaseWidget = new NewModeBaseWidget();
    setMainWidget(newModeBaseWidget);
    setButtons( Ok | Cancel);
    setDefaultButton(Ok);
    setModal(modal);

    QStringList remoteList = allModes.getRemotes();
    remoteList.sort();
    for (QStringList::iterator it = remoteList.begin(); it != remoteList.end(); ++it) {
        new QTreeWidgetItem(newModeBaseWidget->theRemotes, (QStringList() << *it));
    }
    newModeBaseWidget->theIcon->setIcon("irkick");
    connect(newModeBaseWidget->theName, SIGNAL(textChanged(const QString&)), this, SLOT(checkForComplete()));
    connect(newModeBaseWidget->theRemotes, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this, SLOT(checkForComplete()));
    connect(newModeBaseWidget->checkBox, SIGNAL(toggled(bool)),  newModeBaseWidget->theIcon, SLOT(setEnabled(bool)));
    enableButtonOk(false);
}

NewModeDialog::~NewModeDialog()
{
}

void NewModeDialog::checkForComplete()
{
    if(newModeBaseWidget->theRemotes->currentItem() && !newModeBaseWidget->theName->text().isEmpty()){
	foreach(const Mode &mode, allModes.getModes(newModeBaseWidget->theRemotes->currentItem()->text(0))){
	    if(mode.name() == newModeBaseWidget->theName->text()){
		enableButtonOk(false);
		return;
	    }
	}
	enableButtonOk(true);
	return;
    }
    enableButtonOk(false);
}

Mode NewModeDialog::getMode()
{

    Mode mode = Mode();
    mode.setRemote(newModeBaseWidget->theRemotes->currentItem()->text(0));
    mode.setName(newModeBaseWidget->theName->text());
    mode.setIconFile(newModeBaseWidget->checkBox->isChecked()  ?
                     newModeBaseWidget->theIcon->icon() : QString());
    return mode;
}

bool NewModeDialog::isDefaultMode()
{
    return newModeBaseWidget->theDefault->isChecked();
}

#include "newmodedialog.moc"
