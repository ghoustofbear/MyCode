#!/bin/bash

echo "auto copy file(lib,dll) to destnation"

cp c2cap.h                     ../include
cp c2cap.h                     ../../ui/client/include
cp c2cap.h                     ../../ui/server/include

cp x64/*                  ../../ui/client/bin/linux/x64
cp x64/*                  ../../ui/server/bin/linux/x64
cp x32/*                  ../../ui/client/bin/linux/x32
cp x32/*                  ../../ui/server/bin/linux/x32
cp x64/*                  ../../ui/client/lib/linux/x64
cp x64/*                  ../../ui/server/lib/linux/x64
cp x32/*                  ../../ui/client/lib/linux/x32
cp x32/*                  ../../ui/server/lib/linux/x32

echo "auto copy finish"

