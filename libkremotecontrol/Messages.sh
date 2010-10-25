#! /bin/sh
$EXTRACTRC --tag=name --tag=description ../profiles/*.xml >> rc.cpp
$EXTRACTATTR --attr=action,name --attr=action,description --attr=argument,comment ../profiles/*.xml >> rc.cpp
$XGETTEXT *.cpp -o $podir/libkremotecontrol.pot
