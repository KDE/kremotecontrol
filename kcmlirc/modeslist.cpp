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
#include <qwidget.h>

#include <kdebug.h>

#include "modeslist.h"

ModesList::ModesList(QWidget *parent, const char *name) : KListView(parent, name)
{
	setAcceptDrops(true);
	setDropVisualizer(false);
	setDropHighlighter(true);
}

bool ModesList::acceptDrag(QDropEvent *) const
{
	// TODO: make safer by checking source/mime type
	// TODO: make safer by only allowing drops on the correct remote control's modes
	return true;
}

#include "modeslist.moc"
