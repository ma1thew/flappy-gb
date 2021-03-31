#!/bin/bash

shopt -s globstar

CC="lcc"
OUT_NAME="out.gb"

if $CC -o "$OUT_NAME" -Iinclude src/**/*.c; then
    echo "Emitted: $OUT_NAME"
else
    echo "Build Failed"
fi
