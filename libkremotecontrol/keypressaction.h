/*
    Copyright (C) 2010  Michael Zanetti <mzanetti@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#ifndef KEYPRESSACTION_H
#define KEYPRESSACTION_H

#include "action.h"

#include <QKeySequence>

class KREMOTECONTROL_EXPORT KeypressAction: public Action
{
    public:
        KeypressAction(const QString &button);
        KeypressAction();
        virtual ~KeypressAction(){};

        virtual QString name() const;
        virtual QString description() const;

        virtual void saveToConfig(KConfigGroup &config);
        virtual void loadFromConfig(const KConfigGroup &config);

        virtual Action *clone() const;

        void setKeySequenceList(const QList<QKeySequence> &keySequenceList);
        QList<QKeySequence> keySequenceList() const;

    protected:
        QList<QKeySequence> m_keySequenceList;
};

#endif // KEYPRESSACTION_H
