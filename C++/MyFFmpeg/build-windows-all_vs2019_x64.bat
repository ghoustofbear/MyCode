call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\Common7\Tools\VsDevCmd.bat"
set VS_PATH=%PATH%

mkdir build-win_vs2019_x64
cd    build-win_vs2019_x64

set PATH=%VS_PATH%
del CMakeCache.txt
cmake -DCMAKE_INSTALL_PREFIX=.\install -DBUILD_SHARED_LIBS=ON -DCMAKE_DEBUG_POSTFIX="_d"-G "Visual Studio 16 2019" .. 


pause


