/*************************************************************************
 * Copyright (C) 2010 by Frank Scheffold <fscheffold@googlemail.com>     *
 *                                                                       *
 * This program is free software; you can redistribute it and/or         *
 * modify it under the terms of the GNU General Public License as        *
 * published by the Free Software Foundation; either version 2 of        *
 * the License or (at your option) version 3 or any later version        *
 * accepted by the membership of KDE e.V. (or its successor approved     *
 * by the membership of KDE e.V.), which shall act as a proxy            *
 * defined in Section 14 of version 3 of the license.                    *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *************************************************************************/

#include "remotetest.h"

#include <kdelirc/libkremotecontrol/remote.h>


#include <KDebug>

#include <QTest>


void RemoteTest::testGetter() {

}

void RemoteTest::testCycleChangeHandler() {
 Mode *mode1 = new Mode("mode1");
  mode1->setButton("1");
  Mode *mode2 = new Mode("mode2");
  mode2->setButton("2");
  Mode *mode3 = new Mode("mode3");
  mode3->setButton("1");
  Mode *mode4 = new Mode("mode4");
  mode4->setButton("2");
  Mode *mode5 = new Mode("mode5");
  mode5->setButton("3");
  Mode *mode6 = new Mode("mode6");
  mode6->setButton("1");
  Mode *mode7 = new Mode("mode7");
  mode7->setButton("4");
  Mode *mode8 = new Mode("mode8");
  mode8->setButton("5");
  Mode *mode9 = new Mode("mode9");
  mode9->setButton("6");
  Mode *mode10 = new Mode("mode10");
  mode10->setButton("7");



  Remote *remote = new Remote("remote1", Remote::Cycle);
  QCOMPARE(1, remote->allModes().size());
  remote->addMode(mode1);
  QCOMPARE(2, remote->allModes().size());
  remote->addMode(mode2);
  QCOMPARE(3, remote->allModes().size());
  remote->addMode(mode3);
  QCOMPARE(3, remote->allModes().size());
  remote->addMode(mode4);
  QCOMPARE(3, remote->allModes().size());
  remote->addMode(mode5);
  QCOMPARE(4, remote->allModes().size());
  remote->addMode(mode6);
  QCOMPARE(4, remote->allModes().size());
  remote->addMode(mode7);
  QCOMPARE(5, remote->allModes().size());
  remote->addMode(mode8);
  QCOMPARE(6, remote->allModes().size());
  remote->addMode(mode9);
  QCOMPARE(7, remote->allModes().size());
  remote->addMode(mode10);
  QCOMPARE(8, remote->allModes().size());
  remote->setNextModeButton("n");
  remote->setPreviousModeButton("p");
  QCOMPARE(remote->masterMode(), remote->currentMode());
  QCOMPARE(false, remote->nextMode("foobar"));
  QCOMPARE(remote->masterMode(), remote->currentMode());

  QCOMPARE(true, remote->nextMode("n"));
  QCOMPARE(mode1, remote->currentMode());
  QCOMPARE(true, remote->nextMode("n"));
  QCOMPARE(mode2, remote->currentMode());
  QCOMPARE(true, remote->nextMode("n"));
  QCOMPARE(mode5, remote->currentMode());
  QCOMPARE(true, remote->nextMode("n"));
  QCOMPARE(mode7, remote->currentMode());
  QCOMPARE(true, remote->nextMode("n"));
  QCOMPARE(mode8, remote->currentMode());
  QCOMPARE(true, remote->nextMode("n"));
  QCOMPARE(mode9, remote->currentMode());
  QCOMPARE(true, remote->nextMode("n"));
  QCOMPARE(mode10, remote->currentMode());
  QCOMPARE(true, remote->nextMode("n"));
  QCOMPARE(remote->masterMode(), remote->currentMode());

  QCOMPARE(true, remote->nextMode("p"));
  QCOMPARE(mode10, remote->currentMode());
  QCOMPARE(true, remote->nextMode("p"));
  QCOMPARE(mode9, remote->currentMode());
  QCOMPARE(true, remote->nextMode("p"));
  QCOMPARE(mode8, remote->currentMode());
  QCOMPARE(true, remote->nextMode("p"));
  QCOMPARE(mode7, remote->currentMode());
  QCOMPARE(true, remote->nextMode("p"));
  QCOMPARE(mode5, remote->currentMode());
  QCOMPARE(true, remote->nextMode("p"));
  QCOMPARE(mode2, remote->currentMode());
  QCOMPARE(true, remote->nextMode("p"));
  QCOMPARE(mode1, remote->currentMode());
  QCOMPARE(true, remote->nextMode("p"));
  QCOMPARE(remote->masterMode(), remote->currentMode());

  QCOMPARE(true, remote->nextMode("1"));
  QCOMPARE(mode1, remote->currentMode());
  QCOMPARE(false, remote->nextMode("1"));
  QCOMPARE(mode1, remote->currentMode());

  QCOMPARE(true, remote->nextMode("2"));
  QCOMPARE(mode2, remote->currentMode());
  QCOMPARE(false, remote->nextMode("2"));
  QCOMPARE(mode2, remote->currentMode());

  QCOMPARE(true, remote->nextMode("3"));
  QCOMPARE(mode5, remote->currentMode());
  QCOMPARE(false, remote->nextMode("3"));
  QCOMPARE(mode5, remote->currentMode());  
}


void RemoteTest::testGroupChangeHandler() {
  Mode *mode1 = new Mode("mode1");
  mode1->setButton("1");
  Mode *mode2 = new Mode("mode2");
  mode2->setButton("2");
  Mode *mode3 = new Mode("mode3");
  mode3->setButton("1");
  Mode *mode4 = new Mode("mode4");
  mode4->setButton("2");
  Mode *mode5 = new Mode("mode5");
  mode5->setButton("3");
  Mode *mode6 = new Mode("mode6");
  mode6->setButton("1");
  Mode *mode7 = new Mode("mode7");
  mode7->setButton("1");
  Mode *mode8 = new Mode("mode8");
  mode8->setButton("2");
  Mode *mode9 = new Mode("mode9");
  mode9->setButton("2");
  Mode *mode10 = new Mode("mode10");
  mode10->setButton("2");
  
 

  Remote *remote = new Remote("remote1");
  QCOMPARE(1, remote->allModes().size());
  remote->addMode(mode1);
  remote->addMode(mode2);
  remote->addMode(mode3);
  remote->addMode(mode4);
  remote->addMode(mode5);
  remote->addMode(mode6);
  remote->addMode(mode7);
  remote->addMode(mode8);
  remote->addMode(mode9);
  remote->addMode(mode10);
  QCOMPARE(11, remote->allModes().size());
  QCOMPARE(remote->masterMode(), remote->currentMode());
  QCOMPARE(false, remote->nextMode("foobar"));
  QCOMPARE(remote->masterMode(), remote->currentMode());
  QCOMPARE(true,remote->nextMode("1"));
  QCOMPARE( mode1,  remote->currentMode());
  QCOMPARE(true,remote->nextMode("1"));
  QCOMPARE(mode3,remote->currentMode());
  QCOMPARE(true,remote->nextMode("1"));
  QCOMPARE(mode6,remote->currentMode());
  QCOMPARE(true,remote->nextMode("1"));
  QCOMPARE(mode7,remote->currentMode());
  QCOMPARE(true,remote->nextMode("1"));
  QCOMPARE(remote->masterMode(), remote->currentMode());
  
  
  QCOMPARE(true,remote->nextMode("3"));
  QCOMPARE(mode5,remote->currentMode());
  QCOMPARE(true,remote->nextMode("3"));
  QCOMPARE(remote->masterMode(), remote->currentMode());
  
  remote->setCurrentMode(mode10);
  QCOMPARE(true,remote->nextMode("2"));
  QCOMPARE(remote->masterMode(), remote->currentMode());
  
  remote->setCurrentMode(mode10);
  QCOMPARE(true,remote->nextMode("1"));
  QCOMPARE(mode1,remote->currentMode());
}


void RemoteTest::testOperatorEquals() {
   Mode *mode1 = new Mode("mode1");
   Mode *mode2 = new Mode("mode1");
   QCOMPARE(*mode1, *mode2);
   mode1->setButton("1");
   QCOMPARE(false, *mode1==  *mode2);
   mode2->setButton("1");
   QCOMPARE(*mode1, *mode2);
   
   mode2->setName("blubber");
   QCOMPARE(false, *mode1==  *mode2);
   
   mode2->setName("mode1");
   mode2->setButton("1");
   mode2->setIconName("icon");
   
   QCOMPARE(false, *mode1 ==  *mode2);
   
   mode1->setIconName("icon");
   QCOMPARE(*mode1, *mode2);
}

void RemoteTest::toggleModeChangeHandler() {
 Mode *mode1 = new Mode("mode1");
  mode1->setButton("1");
  Mode *mode2 = new Mode("mode2");
  mode2->setButton("2");
  Mode *mode3 = new Mode("mode3");
  mode3->setButton("1");
  Mode *mode4 = new Mode("mode4");
  mode4->setButton("2");
  Mode *mode5 = new Mode("mode5");
  mode5->setButton("3");
  Mode *mode6 = new Mode("mode6");
  mode6->setButton("1");
  Mode *mode7 = new Mode("mode7");
  mode7->setButton("1");
  Mode *mode8 = new Mode("mode8");
  mode8->setButton("2");
  Mode *mode9 = new Mode("mode9");
  mode9->setButton("2");
  Mode *mode10 = new Mode("mode10");
  mode10->setButton("2");



  Remote *remote = new Remote("remote1");
  QCOMPARE(1, remote->allModes().size());
  remote->addMode(mode1);
  remote->addMode(mode2);
  remote->addMode(mode3);
  remote->addMode(mode4);
  remote->addMode(mode5);
  remote->addMode(mode6);
  remote->addMode(mode7);
  remote->addMode(mode8);
  remote->addMode(mode9);
  remote->addMode(mode10);
  QCOMPARE(11, remote->allModes().size());
  remote->setModeChangeMode(Remote::Cycle);  
  QCOMPARE(11, remote->allModes().size());
  QCOMPARE(QString("1"),mode1->button());
  QCOMPARE(QString("2"),mode2->button());
  QCOMPARE(QString(),mode3->button());
  QCOMPARE(QString(),mode4->button());
  QCOMPARE(QString("3"),mode5->button());
  QCOMPARE(QString(),mode6->button());
  QCOMPARE(QString(),mode7->button());
  QCOMPARE(QString(),mode8->button());
  QCOMPARE(QString(),mode9->button());
  QCOMPARE(QString(),mode10->button());

  remote->setNextModeButton("1");
  QCOMPARE(QString(),mode1->button());
  remote->setPreviousModeButton("2");
  QCOMPARE(QString(),mode2->button());
}


QTEST_MAIN( RemoteTest )
