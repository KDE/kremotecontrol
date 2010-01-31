/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

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

#include "actioncontainer.h"
#include "editdbusaction.h"

#include <kdebug.h>

ActionContainer::ActionContainer(Action::ActionType type, QWidget* parent, Qt::WFlags flags): KDialog(parent, flags) {
    QWidget *widget = new QWidget(this);
    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(widget);
    ui.setupUi(widget);
    setMainWidget(widget);
    

    QWidget *innerWidget = 0;
    switch(type){
        case Action::DBusAction:
            innerWidget = new EditDBusAction();
            break;
        case Action::ProfileAction:
  /*          innerWidget = new EditProfileAction();
            break;*/
        default:
          kDebug() << "Invalid action type. Not opening Add-Window";
    }
      
    if(innerWidget){
        QHBoxLayout *innerLayout = new QHBoxLayout();
        innerLayout->addWidget(innerWidget);
        ui.wActionWidget->setLayout(innerLayout);
    }
}
