#!/bin/bash

# if any argument is given
if [ "$#" -gt 0 ]; then
    echo ">>> CMAKE CONFIGURE"
    ./auto/config.bash
fi

echo ">>> CMAKE BUILD"
cmake --build build
