/***************************************************************************
 *   Copyright (C) 2003 by Gav Wood                                        *
 *   gav@kde.org                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

#include "modeslist.h"

void KCMLircBase::init()
{
	delete theModes;
	modeLayout->removeItem(modeButtonLayout);
	theModes = new ModesList(dynamic_cast<QWidget *>(modeLayout->parent()), "theModes");
	theModes->addColumn(i18n("Remote Control"));
	theModes->addColumn(i18n("Default"));
	theModes->addColumn(i18n("Icon"));
	theModes->setAcceptDrops(true);
	theModes->setRootIsDecorated(true);
	theModes->setItemsRenameable(true);
	modeLayout->addWidget(theModes);
	modeLayout->addLayout(modeButtonLayout);
	modeLayout->invalidate();
	modeLayout->activate();
}
