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

NewModeDialog::NewModeDialog(Remote *remote, QWidget *parent): KDialog(parent), m_remote(remote)
{
    QWidget *widget = new QWidget(this);
    ui.setupUi(widget);
    setMainWidget(widget);
    setButtons( Ok | Cancel);
    setDefaultButton(Ok);

/*    QStringList remoteList = allModes.getRemotes();
    remoteList.sort();
    for (QStringList::const_iterator it = remoteList.constBegin(); it != remoteList.constEnd(); ++it) {
        new QTreeWidgetItem(newModeBaseWidget->theRemotes, (QStringList() << *it));
    }
    
    
    newModeBaseWidget->theIcon->setIcon("irkick");*/
    connect(ui.leName, SIGNAL(textChanged(const QString&)), this, SLOT(checkForComplete()));
    enableButtonOk(false);
}

NewModeDialog::~NewModeDialog()
{
}

void NewModeDialog::checkForComplete()
{
    if(ui.leName->text().isEmpty()){
        enableButtonOk(false);
        return;
    }
    foreach(const Mode *mode, m_remote->allModes()){
        if(mode->name() == ui.leName->text()){
            enableButtonOk(false);
            return;
        }
    }
    enableButtonOk(true);
}

void NewModeDialog::slotButtonClicked(int button) {
    if(button == KDialog::Ok){
        m_remote->addMode(new Mode(ui.leName->text(),ui.ibIcon->icon()));
    }
    KDialog::slotButtonClicked(button);
}


// Mode NewModeDialog::getMode()
// {
// 
//     Mode mode = Mode();
//     mode.setRemote(newModeBaseWidget->theRemotes->currentItem()->text(0));
//     mode.setName(newModeBaseWidget->theName->text());
//     mode.setIconFile(newModeBaseWidget->checkBox->isChecked()  ?
//                      newModeBaseWidget->theIcon->icon() : QString());
//     return mode;
// }

// bool NewModeDialog::isDefaultMode() const
// {
//     return newModeBaseWidget->theDefault->isChecked();
// }

#include "newmodedialog.moc"
