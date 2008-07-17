//
//
// C++ Implementation: $MODULE$
//
// Description:
//
//
// Author: Gav Wood <gav@kde.org>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <kicondialog.h>
#include <kiconloader.h>
#include <kpushbutton.h>

#include "editmode.h"

EditMode::EditMode(QWidget *parent, const char *name, bool modal, Qt::WFlags fl)// : Ui::EditModeBase(parent, name, modal, fl)
{
	setupUi(this);
	theIcon->setIconType(KIconLoader::Panel, KIconLoader::Any);
}

EditMode::~EditMode()
{
}

void EditMode::slotClearIcon()
{
	theIcon->resetIcon();
}

void EditMode::slotCheckText(const QString &newText)
{
	theOK->setEnabled(!newText.isEmpty());
}

#include "editmode.moc"
