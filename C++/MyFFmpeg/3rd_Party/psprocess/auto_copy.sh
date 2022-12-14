#!/bin/bash

echo "auto copy file(lib,dll) to destnation"

cp PsProcessApi.h                     ../include
cp PsProcessApi.h                     ../../ui/client/include
cp PsProcessApi.h                     ../../ui/server/include

cp x64/*                  ../../ui/client/bin/linux/x64
cp x64/*                  ../../ui/server/bin/linux/x64
cp x32/*                  ../../ui/client/bin/linux/x32
cp x32/*                  ../../ui/server/bin/linux/x32
cp x64/*                  ../../ui/client/lib/linux/x64
cp x64/*                  ../../ui/server/lib/linux/x64
cp x32/*                  ../../ui/client/lib/linux/x32
cp x32/*                  ../../ui/server/lib/linux/x32

echo "auto copy finish"

