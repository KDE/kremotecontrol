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
	theModes->addColumn(tr2i18n("Remote Control"));
	theModes->addColumn(tr2i18n("Default"));
	theModes->setAcceptDrops(true);
	theModes->setRootIsDecorated(true);
	modeLayout->addWidget(theModes);
	modeLayout->addLayout(modeButtonLayout);
	modeLayout->invalidate();
	modeLayout->activate();
}
