#!/bin/bash -e

mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
cmake --build .
