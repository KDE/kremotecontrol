//
//
// C++ Interface: $MODULE$
//
// Description:
//
//
// Author: Gav Wood <gav@kde.org>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EDITMODE_H
#define EDITMODE_H

#include "ui_editmodebase.h"

/**
@author Gav Wood
*/
class EditMode : public QDialog, public Ui::EditModeBase
{
	Q_OBJECT

public:
	void slotCheckText(const QString &newText);
	void slotClearIcon();

	explicit EditMode(QWidget *parent = 0, Qt::WFlags fl = 0);
	~EditMode();
};

#endif
