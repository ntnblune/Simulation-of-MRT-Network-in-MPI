#!/bin/bash

## You can use sbatch instead of salloc to run batch jobs, as usual.

#SBATCH --job-name=run_bench
#SBATCH --nodes=1
#SBATCH --time=00:10:30
#SBATCH --partition=i7-7700
#SBATCH --output=logs/bench/%j.slurmlog
#SBATCH --error=logs/bench/seq_%j.slurmlog


# Display commands being run in stdout
./bench_seq ./testcases/performance/min.in
./bench_seq ./testcases/performance/med.in
./bench_seq ./testcases/performance/stations.in
./bench_seq ./testcases/performance/trains.in
./bench_seq ./testcases/performance/traffic.in
./bench_seq ./testcases/performance/max.in
