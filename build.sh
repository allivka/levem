# !/bin/sh

gcc levem.c -o levem $(pkg-config --libs --cflags libevdev) -O3 -s