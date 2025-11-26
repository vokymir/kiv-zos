#!/bin/bash

mkdir -p build
mkdir -p bin
cmake -B build -DCMAKE_BUILD_TYPE=Debug
