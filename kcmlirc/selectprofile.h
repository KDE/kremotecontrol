/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef SELECTPROFILE_H
#define SELECTPROFILE_H

#include <QtGui/QWidget>
#include "kicondialog.h"


class SelectProfileWidget : public QWidget
{
public:
    SelectProfileWidget (QWidget *parent = 0) : QWidget(parent)
    {
      
    }
};


class SelectProfile : public KDialog
{
  Q_OBJECT
  
  SelectProfileWidget *selectProfileWidget;
  
  SelectProfile(QWidget *parent = 0, const bool &modal = false);
};

#endif // SELECTPROFILE_H
