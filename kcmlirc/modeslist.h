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
#ifndef MODESLIST_H
#define MODESLIST_H

#include <QTreeWidget>

//Added by qt3to4:
#include <QDropEvent>

/**

@author Gav Wood
*/

class ModesList : public QTreeWidget
{
    Q_OBJECT
public:
    virtual bool acceptDrag(QDropEvent *) const;
    ModesList(QWidget *parent = 0);
};

#endif
