#!/usr/bin/env bash
set -e

# compile C++
g++ main.cpp -o cubeIkeda  `capd-config --cflags --libs`

# run C++ → generate CSV
./cubeIkeda output/result.csv

# run Python → generate plot
python3 plot_traj.py