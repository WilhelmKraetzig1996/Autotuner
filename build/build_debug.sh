#!/bin/bash
echo "directory: $1"
mkdir -p $1/build_debug

cd $1/build_debug
conan install -of $1/build_debug --profile=$1/build/conanprofile_debug --build=missing  ../build
cmake -G "Unix Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DBUILD_INTEGRATION_TESTS=true $1 
cmake --build . --parallel 7 
