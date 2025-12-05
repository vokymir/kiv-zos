#!/bin/bash

file=tmp/filesystem.voky

if [ "$1" == "0" ]; then

hexdump -C "$file" | awk '{ line=""; for(i=2;i<=17;i++) line=line $i; if(line !~ /^0+$/) print }'
fi

if [ "$1" == "1" ]; then
xxd "$file" | awk '$2 !~ /^00/ {print}'
fi
