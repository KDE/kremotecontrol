/*
    Copyright [yyyy] [name of copyright owner]

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

*/

#include "remotetest.h"

#include <kdelirc/libkremotecontrol/remote.h>


#include <KDebug>

#include <QTest>


void RemoteTest::testGetter() {

}


void RemoteTest::testnextMode() {
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
  mode6->setButton("1");
  Mode *mode8 = new Mode("mode8");
  mode6->setButton("2");
  Mode *mode9 = new Mode("mode9");
  mode6->setButton("2");
  Mode *mode10 = new Mode("mode10");
  mode6->setButton("2");
  
  QList<Mode*> modeList;
  modeList.append(mode1);
  modeList.append(mode2);
  modeList.append(mode3);
  modeList.append(mode4);
  modeList.append(mode5);
  modeList.append(mode6);
  modeList.append(mode7);
  modeList.append(mode8);
  modeList.append(mode9);
  modeList.append(mode10);
  Remote *remote = new Remote("remote1", modeList);
  
  
  QCOMPARE(remote->masterMode(), remote->currentMode());
  QCOMPARE(false, remote->nextMode("foobar"));
  QCOMPARE(remote->masterMode(), remote->currentMode());
  QCOMPARE(true,remote->nextMode("1"));
  QCOMPARE( mode1,  remote->currentMode());
  QCOMPARE(true,remote->nextMode("1"));
  QCOMPARE(mode3,remote->currentMode());
  
  QCOMPARE(true,remote->nextMode("3"));
  QCOMPARE(mode5,remote->currentMode());
  QCOMPARE(false,remote->nextMode("3"));
  QCOMPARE(mode5,remote->currentMode());
  
  remote->setCurrentMode(mode10);
  QCOMPARE(true,remote->nextMode("2"));
  QCOMPARE(mode2,remote->currentMode());
  
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



QTEST_MAIN( RemoteTest )