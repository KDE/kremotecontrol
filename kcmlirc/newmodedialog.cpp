/***************************************************************************
 *   Copyright (C) 2003 by Gav Wood                                        *
 *   gav@kde.org                                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "newmodedialog.h"

NewModeDialog::NewModeDialog( QWidget* parent )
    : QDialog( parent )
{
    setupUi(this);

    theRemotes->addColumn( i18n( "Remote Controls" ) );
    theRemotes->setFullWidth( true );

    // signals and slots connections
    connect(kPushButton6, SIGNAL(clicked()), this, SLOT(accept()));
    connect(kPushButton5, SIGNAL(clicked()), this, SLOT(reject()));
    connect(theName, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotTextChanged(const QString&)));
}

NewModeDialog::~NewModeDialog()
{
}

void NewModeDialog::slotTextChanged( const QString& newText )
{
    kPushButton6->setEnabled( !newText.isEmpty() );
}

#include "newmodedialog.moc"
