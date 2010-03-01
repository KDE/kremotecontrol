/*
    Copyright (C) 2010  Michael Zanetti <michael_zanetti@gmx.net>

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

#ifndef ACTION_H
#define ACTION_H

#include "mode.h"
#include "kremotecontrol_export.h"

#include <solid/control/remotecontrolbutton.h>
#include <solid/control/remotecontrol.h>
#include <kconfiggroup.h>

class KREMOTECONTROL_EXPORT Action: public QObject
{
  Q_OBJECT
  
  public:
      enum ActionType {DBusAction, ProfileAction};
    
      Action(ActionType type, const QString &button);
      Action(ActionType type);
      Action(const Action &action);
    
      ActionType type() const;

      QString button() const;
      void setButton(const QString &button);

      virtual QString name() const = 0;
      virtual QString description() const = 0;

      virtual void saveToConfig(KConfigGroup &config);
      virtual void loadFromConfig(const KConfigGroup &config);
      
      virtual Action *clone() const = 0;
    
  protected:
      ActionType m_type;
      QString m_button;    
};

Q_DECLARE_METATYPE(Action*)
Q_DECLARE_METATYPE(Action::ActionType)

#endif // ACTION_H
