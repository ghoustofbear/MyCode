#!/bin/bash

echo "auto copy file(lib,dll) to destnation"

cp c2fplay.h                     ../../include
cp c2fplay.h                     ../../../ui/client/include
cp c2fplay.h                     ../../../ui/server/include
cp playerlistener.h                     ../../include
cp playerlistener.h                     ../../../ui/client/include
cp playerlistener.h                     ../../../ui/server/include

cp x64/*                  ../../../ui/client/bin/linux/x64
cp x64/*                  ../../../ui/client/lib/linux/x64
cp x32/*                  ../../../ui/client/bin/linux/x32
cp x32/*                  ../../../ui/client/lib/linux/x32

echo "auto copy finish"

