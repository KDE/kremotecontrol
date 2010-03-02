/*
    Copyright (C) 2010 Michael Zanetti <michael_zanetti@gmx.net>

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

#ifndef KRCDNOTIFIERITEM_H
#define KRCDNOTIFIERITEM_H

#include <kstatusnotifieritem.h>
#include <kmenu.h>

class KrcdNotifierItem: public KStatusNotifierItem
{
    Q_OBJECT
    public:
        KrcdNotifierItem();
        
    private:
        KMenu m_menu;
        
        void updateTray();
        
    private slots:
        void slotConfigure();
        void slotModeSelected(QAction *action);
        void flash();
        void flashOff();
        void updateContextMenu();
};

#endif // KRCDNOTIFICATIONITEM_H
