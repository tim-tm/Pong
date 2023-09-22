#!/bin/bash
mkdir build
x86_64-w64-mingw32-cc -Wall -Wextra -o build/pong.exe src/main.c -lSDL2
