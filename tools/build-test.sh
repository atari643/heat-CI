#!/bin/bash

# env. var. HEAT_ROOT required
[ -z "$HEAT_ROOT" ] && echo "Variable HEAT_ROOT must be set to execute this script." && exit 1

cd $HEAT_ROOT

export CFLAGS="-O0 -g -fPIC --coverage -Wall -Wunused-parameter -Wundef -Wno-long-long -Wsign-compare -Wmissing-prototypes -Wstrict-prototypes -Wcomment -pedantic -fdiagnostics-show-option -fno-inline -fno-omit-frame-pointer"

mkdir -p build
cd build
rm CMake* -rf

# configure
scan-build -v -plist --intercept-first --analyze-headers -o analyzer_reports cmake .. \
           -DCMAKE_VERBOSE_MAKEFILE=ON \
           -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
           -DCMAKE_C_FLAGS="$CFLAGS" \
           -DCMAKE_EXE_LINKER_FLAGS="--coverage" \
           -DHEAT_USE_MPI=ON

# make (+clang analyzer)
scan-build -v -plist --intercept-first --analyze-headers -o analyzer_reports make 2>&1 | tee heat-build.log

# test
ctest --no-compress-output -T Test -V --output-junit ../heat-junit.xml
