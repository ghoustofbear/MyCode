#!/bin/bash

echo "auto copy file(lib,dll) to destnation"

cp c2sdk.h                     ../include
cp c2types.h                   ../include
cp c2sdk.h                     ../../ui/client/include
cp c2types.h                   ../../ui/client/include
cp c2sdk.h                     ../../ui/server/include
cp c2types.h                   ../../ui/server/include

cp x64/*                  ../../ui/client/bin/linux/x64
cp x64/*                  ../../ui/server/bin/linux/x64
cp x32/*                  ../../ui/client/bin/linux/x32
cp x32/*                  ../../ui/server/bin/linux/x32
cp x64/*                  ../../ui/client/lib/linux/x64
cp x64/*                  ../../ui/server/lib/linux/x64
cp x32/*                  ../../ui/client/lib/linux/x32
cp x32/*                  ../../ui/server/lib/linux/x32

echo "auto copy finish"

