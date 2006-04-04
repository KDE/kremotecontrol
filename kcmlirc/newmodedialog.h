/***************************************************************************
 *   Copyright (C) 2003 by Gav Wood                                        *
 *   gav@kde.org                                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef NEWMODEDIALOG_H
#define NEWMODEDIALOG_H

#include <QDialog>
#include "newmode.h"

class NewModeDialog : public QDialog, public Ui_NewMode
{
    Q_OBJECT

public:
    explicit NewModeDialog( QWidget* parent );
    ~NewModeDialog();

private slots:
    void slotTextChanged( const QString& newText );
};


#endif /* NEWMODEDIALOG_H */
