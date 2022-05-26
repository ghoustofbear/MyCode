#!/bin/bash

echo "auto copy file(lib,dll) to destnation"

cp exports.h                   ../../ui/server/include/
cp onuiupdate.h                ../../ui/server/include/
cp servmgr.h                   ../../ui/server/include/
cp log.h                       ../../ui/server/include/
cp config/cfg*.h               ../../ui/server/include/
cp config/view*.h              ../../ui/server/include/
cp config/observable.h         ../../ui/server/include/
cp config/observer.h           ../../ui/server/include/

cp  ../../ui/server/rc/log.ini  ../../ui/server/bin/linux/x32
cp  ../../ui/server/rc/log.ini  ../../ui/server/bin/linux/x64

cp  ../lib/linux/x64/libeasysqlite* x64
# cp ../../ui/server/rc/c2msv.db ../../ui/server/bin/linux/x32
# cp ../../ui/server/rc/c2msv.db ../../ui/server/bin/linux/x64

echo "auto copy finish"

