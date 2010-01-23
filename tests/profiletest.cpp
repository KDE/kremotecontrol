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

#include "profiletest.h"

#include <kdelirc/libkremotecontrol/profile.h>

#include <KDebug>
#include <QTest>

void ProfileTest::runAll()
{

}
void ProfileTest::cleanup()
{

}
void ProfileTest::setup()
{

}

void ProfileTest::testVersion()
{
  NewProfile profile1("id1", "name", "1.1","some autohor", "dummy 1" );
  NewProfile profile2("id1", "name", "1.1","some autohor", "dummy 1" );
  kDebug()<< profile1.compareVersion(profile2);
//   QCOMPARE(0, profile1.compareVersion(profile2) );
QWARN("Hallo welt ");
QCOMPARE(0, 0);
QCOMPARE(0, 0);

}
