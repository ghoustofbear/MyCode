#!/bin/bash

echo "auto copy file(lib,dll) to destnation"

cp c2play.h                     ../include
cp c2play.h                     ../../ui/client/include

cp x64/*                        ../../ui/client/lib/linux/x64
cp x64/*                        ../../ui/client/bin/linux/x64
cp x32/*                        ../../ui/client/lib/linux/x32
cp x32/*                        ../../ui/client/bin/linux/x32

echo "auto copy finish"

