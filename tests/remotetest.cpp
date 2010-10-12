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
#include "remote.h"

#include <KDebug>

#include <QtTest/QTest>


void RemoteTest::testGetter() {

}

void RemoteTest::testCycleChangeHandler() {
 Mode *mode1 = new Mode(QLatin1String("mode1"));
  mode1->setButton(QLatin1String( "1" ));
  Mode *mode2 = new Mode(QLatin1String( "mode2" ));
  mode2->setButton(QLatin1String( "2" ));
  Mode *mode3 = new Mode(QLatin1String( "mode3" ));
  mode3->setButton(QLatin1String( "1" ));
  Mode *mode4 = new Mode(QLatin1String( "mode4" ));
  mode4->setButton(QLatin1String( "2" ));
  Mode *mode5 = new Mode(QLatin1String( "mode5" ));
  mode5->setButton(QLatin1String( "3" ));
  Mode *mode6 = new Mode(QLatin1String( "mode6" ));
  mode6->setButton(QLatin1String( "1" ));
  Mode *mode7 = new Mode(QLatin1String( "mode7" ));
  mode7->setButton(QLatin1String( "4" ));
  Mode *mode8 = new Mode(QLatin1String( "mode8" ));
  mode8->setButton(QLatin1String( "5" ));
  Mode *mode9 = new Mode(QLatin1String( "mode9" ));
  mode9->setButton(QLatin1String( "6" ));
  Mode *mode10 = new Mode(QLatin1String( "mode10" ));
  mode10->setButton(QLatin1String( "7" ));



  Remote *remote = new Remote(QLatin1String( "remote1" ), Remote::Cycle);
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
  remote->setNextModeButton(QLatin1String( "n" ));
  remote->setPreviousModeButton(QLatin1String( "p" ));
  QCOMPARE(remote->masterMode(), remote->currentMode());
  QCOMPARE(false, remote->nextMode(QLatin1String( "foobar" )));
  QCOMPARE(remote->masterMode(), remote->currentMode());

  QCOMPARE(true, remote->nextMode(QLatin1String( "n" )));
  QCOMPARE(mode1, remote->currentMode());
  QCOMPARE(true, remote->nextMode(QLatin1String( "n" )));
  QCOMPARE(mode2, remote->currentMode());
  QCOMPARE(true, remote->nextMode(QLatin1String( "n" )));
  QCOMPARE(mode5, remote->currentMode());
  QCOMPARE(true, remote->nextMode(QLatin1String( "n" )));
  QCOMPARE(mode7, remote->currentMode());
  QCOMPARE(true, remote->nextMode(QLatin1String( "n" )));
  QCOMPARE(mode8, remote->currentMode());
  QCOMPARE(true, remote->nextMode(QLatin1String( "n" )));
  QCOMPARE(mode9, remote->currentMode());
  QCOMPARE(true, remote->nextMode(QLatin1String( "n" )));
  QCOMPARE(mode10, remote->currentMode());
  QCOMPARE(true, remote->nextMode(QLatin1String( "n" )));
  QCOMPARE(remote->masterMode(), remote->currentMode());

  QCOMPARE(true, remote->nextMode(QLatin1String( "p" )));
  QCOMPARE(mode10, remote->currentMode());
  QCOMPARE(true, remote->nextMode(QLatin1String( "p" )));
  QCOMPARE(mode9, remote->currentMode());
  QCOMPARE(true, remote->nextMode(QLatin1String( "p" )));
  QCOMPARE(mode8, remote->currentMode());
  QCOMPARE(true, remote->nextMode(QLatin1String( "p" )));
  QCOMPARE(mode7, remote->currentMode());
  QCOMPARE(true, remote->nextMode(QLatin1String( "p" )));
  QCOMPARE(mode5, remote->currentMode());
  QCOMPARE(true, remote->nextMode(QLatin1String( "p" )));
  QCOMPARE(mode2, remote->currentMode());
  QCOMPARE(true, remote->nextMode(QLatin1String( "p" )));
  QCOMPARE(mode1, remote->currentMode());
  QCOMPARE(true, remote->nextMode(QLatin1String( "p" )));
  QCOMPARE(remote->masterMode(), remote->currentMode());

  QCOMPARE(true, remote->nextMode(QLatin1String( "1" )));
  QCOMPARE(mode1, remote->currentMode());
  QCOMPARE(false, remote->nextMode(QLatin1String( "1" )));
  QCOMPARE(mode1, remote->currentMode());

  QCOMPARE(true, remote->nextMode(QLatin1String( "2" )));
  QCOMPARE(mode2, remote->currentMode());
  QCOMPARE(false, remote->nextMode(QLatin1String( "2" )));
  QCOMPARE(mode2, remote->currentMode());

  QCOMPARE(true, remote->nextMode(QLatin1String( "3" )));
  QCOMPARE(mode5, remote->currentMode());
  QCOMPARE(false, remote->nextMode(QLatin1String( "3" )));
  QCOMPARE(mode5, remote->currentMode());
}


void RemoteTest::testGroupChangeHandler() {
  Mode *mode1 = new Mode(QLatin1String( "mode1" ));
  mode1->setButton(QLatin1String( "1" ));
  Mode *mode2 = new Mode(QLatin1String( "mode2" ));
  mode2->setButton(QLatin1String( "2" ));
  Mode *mode3 = new Mode(QLatin1String( "mode3" ));
  mode3->setButton(QLatin1String( "1" ));
  Mode *mode4 = new Mode(QLatin1String( "mode4" ));
  mode4->setButton(QLatin1String( "2" ));
  Mode *mode5 = new Mode(QLatin1String( "mode5" ));
  mode5->setButton(QLatin1String( "3" ));
  Mode *mode6 = new Mode(QLatin1String( "mode6" ));
  mode6->setButton(QLatin1String( "1" ));
  Mode *mode7 = new Mode(QLatin1String( "mode7" ));
  mode7->setButton(QLatin1String( "1" ));
  Mode *mode8 = new Mode(QLatin1String( "mode8" ));
  mode8->setButton(QLatin1String( "2" ));
  Mode *mode9 = new Mode(QLatin1String( "mode9" ));
  mode9->setButton(QLatin1String( "2" ));
  Mode *mode10 = new Mode(QLatin1String( "mode10" ));
  mode10->setButton(QLatin1String( "2" ));



  Remote *remote = new Remote(QLatin1String( "remote1" ));
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
  QCOMPARE(false, remote->nextMode(QLatin1String( "foobar" )));
  QCOMPARE(remote->masterMode(), remote->currentMode());
  QCOMPARE(true,remote->nextMode(QLatin1String( "1" )));
  QCOMPARE( mode1,  remote->currentMode());
  QCOMPARE(true,remote->nextMode(QLatin1String( "1" )));
  QCOMPARE(mode3,remote->currentMode());
  QCOMPARE(true,remote->nextMode(QLatin1String( "1" )));
  QCOMPARE(mode6,remote->currentMode());
  QCOMPARE(true,remote->nextMode(QLatin1String( "1" )));
  QCOMPARE(mode7,remote->currentMode());
  QCOMPARE(true,remote->nextMode(QLatin1String( "1" )));
  QCOMPARE(remote->masterMode(), remote->currentMode());


  QCOMPARE(true,remote->nextMode(QLatin1String( "3" )));
  QCOMPARE(mode5,remote->currentMode());
  QCOMPARE(true,remote->nextMode(QLatin1String( "3" )));
  QCOMPARE(remote->masterMode(), remote->currentMode());

  remote->setCurrentMode(mode10);
  QCOMPARE(true,remote->nextMode(QLatin1String( "2" )));
  QCOMPARE(remote->masterMode(), remote->currentMode());

  remote->setCurrentMode(mode10);
  QCOMPARE(true,remote->nextMode(QLatin1String( "1" )));
  QCOMPARE(mode1,remote->currentMode());
}


void RemoteTest::testOperatorEquals() {
   Mode *mode1 = new Mode(QLatin1String( "mode1" ));
   Mode *mode2 = new Mode(QLatin1String( "mode1" ));
   QCOMPARE(*mode1, *mode2);
   mode1->setButton(QLatin1String( "1" ));
   QCOMPARE(false, *mode1==  *mode2);
   mode2->setButton(QLatin1String( "1" ));
   QCOMPARE(*mode1, *mode2);

   mode2->setName(QLatin1String( "blubber" ));
   QCOMPARE(false, *mode1==  *mode2);

   mode2->setName(QLatin1String( "mode1" ));
   mode2->setButton(QLatin1String( "1" ));
   mode2->setIconName(QLatin1String( "icon" ));

   QCOMPARE(false, *mode1 ==  *mode2);

   mode1->setIconName(QLatin1String( "icon" ));
   QCOMPARE(*mode1, *mode2);
}

void RemoteTest::toggleModeChangeHandler() {
 Mode *mode1 = new Mode(QLatin1String( "mode1" ));
  mode1->setButton(QLatin1String( "1" ));
  Mode *mode2 = new Mode(QLatin1String( "mode2" ));
  mode2->setButton(QLatin1String( "2" ));
  Mode *mode3 = new Mode(QLatin1String( "mode3" ));
  mode3->setButton(QLatin1String( "1" ));
  Mode *mode4 = new Mode(QLatin1String( "mode4" ));
  mode4->setButton(QLatin1String( "2" ));
  Mode *mode5 = new Mode(QLatin1String( "mode5" ));
  mode5->setButton(QLatin1String( "3" ));
  Mode *mode6 = new Mode(QLatin1String( "mode6" ));
  mode6->setButton(QLatin1String( "1" ));
  Mode *mode7 = new Mode(QLatin1String( "mode7" ));
  mode7->setButton(QLatin1String( "1" ));
  Mode *mode8 = new Mode(QLatin1String( "mode8" ));
  mode8->setButton(QLatin1String( "2" ));
  Mode *mode9 = new Mode(QLatin1String( "mode9" ));
  mode9->setButton(QLatin1String( "2" ));
  Mode *mode10 = new Mode(QLatin1String( "mode10" ));
  mode10->setButton(QLatin1String( "2" ));



  Remote *remote = new Remote(QLatin1String( "remote1" ));
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
  QCOMPARE(QLatin1String( "1"),mode1->button());
  QCOMPARE(QLatin1String( "2"),mode2->button());
  QCOMPARE(QString(),mode3->button());
  QCOMPARE(QString(),mode4->button());
  QCOMPARE(QLatin1String( "3"),mode5->button());
  QCOMPARE(QString(),mode6->button());
  QCOMPARE(QString(),mode7->button());
  QCOMPARE(QString(),mode8->button());
  QCOMPARE(QString(),mode9->button());
  QCOMPARE(QString(),mode10->button());

  remote->setNextModeButton(QLatin1String( "1" ));
  QCOMPARE(QString(),mode1->button());
  remote->setPreviousModeButton(QLatin1String( "2" ));
  QCOMPARE(QString(),mode2->button());
}


QTEST_MAIN( RemoteTest )
