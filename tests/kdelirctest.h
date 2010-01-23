/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <2009>  <Frank Scheffold (fscheffold@googlemail.com)>

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


#ifndef KDELIRCTEST_H
#define KDELIRCTEST_H

#include <QObject>

class KdelLircTest : public QObject
{
  
 Q_OBJECT
  
  public:
    void setup();
    void runAll();
    void cleanup();
  
    //tests
  void testActionDelegator();  

};
#endif // KDELIRCTEST_H
