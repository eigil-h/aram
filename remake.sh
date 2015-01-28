#!/bin/sh
autoreconf -ivf
./configure
./odb-compile.sh
make

