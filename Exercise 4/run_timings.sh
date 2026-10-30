#!/bin/bash
# Exercise 4 - collect Time vs Number of Processors for Exercise 2 (sum) and Exercise 3 (pi)
# Usage: ./run_timings.sh      (produces timings.csv, then run: python3 plot.py)
mpicxx -O2 "../Exercise 2/sum.cc"   -o sum
mpicxx -O2 "../Exercise 3/pi_mc.cc" -o pi_mc
RUNS=5
echo "program,processes,run,time" > timings.csv
for prog in sum pi_mc; do
  for np in 1 2 4 8; do
    for r in $(seq 1 $RUNS); do
      t=$(mpirun --oversubscribe -np $np ./$prog | grep "Time (seconds)" | awk '{print $4}')
      echo "$prog,$np,$r,$t" | tee -a timings.csv
    done
  done
done
