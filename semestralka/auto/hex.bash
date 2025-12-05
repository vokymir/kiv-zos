#!/bin/bash

if [ "$1" == "0" ]; then

hexdump -C filesystem.voky | awk '{ line=""; for(i=2;i<=17;i++) line=line $i; if(line !~ /^0+$/) print }'
fi

if [ "$1" == "1" ]; then
xxd filesystem.voky | awk '$2 !~ /^00/ {print}'
fi
