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


#include "editmode.h"
#include "mode.h"


EditMode::EditMode(Mode mode, bool isDefault, QWidget *parent, const bool &modal): KDialog(parent)
{
    editModeBaseWidget = new EditModeBaseWidget();
    setMainWidget(editModeBaseWidget);
    setButtons( Ok | Cancel);
    setDefaultButton(Ok);
    setModal(modal);

    editModeBaseWidget->theName->setText(mode.name().isEmpty() ? mode.remoteName() : mode.name());
    editModeBaseWidget->theName->setEnabled(! mode.name().isEmpty());

    editModeBaseWidget->theIcon->setIconType(KIconLoader::Panel, KIconLoader::Any);
    if (!mode.iconFile().isNull()) {
      editModeBaseWidget->theIcon->setIcon(mode.iconFile());
      editModeBaseWidget->theIcon->setEnabled(true);
      editModeBaseWidget->checkBox->setChecked(true);
    } else {
        clearIcon();
    }
    editModeBaseWidget->theDefault->setChecked(isDefault);
    editModeBaseWidget->theDefault->setEnabled(!isDefault);

    connect(editModeBaseWidget->theName, SIGNAL(textChanged(QString)), this, SLOT(slotCheckText(QString)));
    connect(editModeBaseWidget->checkBox, SIGNAL(toggled(bool)),  editModeBaseWidget->theIcon, SLOT(setEnabled(bool)));
}

EditMode::~EditMode()
{
}

Mode EditMode::getMode(){
  Mode mode;
  mode.setIconFile(editModeBaseWidget->checkBox->isChecked()  ?
      editModeBaseWidget->theIcon->icon() : QString::null);
  mode.setName(editModeBaseWidget->theName->text());
  return mode;
}

bool EditMode::isDefaultMode()
{
 return editModeBaseWidget->theDefault->isChecked();
}

void EditMode::clearIcon()
{
  editModeBaseWidget->theIcon->setIcon("irkick");
}

void EditMode::slotCheckText(const QString &newText)
{
  enableButtonOk(!newText.isEmpty());
}

#include "editmode.moc"
