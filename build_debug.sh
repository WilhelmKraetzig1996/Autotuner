#!/bin/bash

docker compose build
# this is necessary because otherwise the debug symbols will not point to the correct path and cannot be debugged on the host
docker run -v conancache:/root/.conan -v conan2cache:/root/.conan2 -v .:$(pwd) -it autotune-compiler:latest $(pwd)/build/build_debug.sh $(pwd)
