#!/bin/bash

## You can use sbatch instead of salloc to run batch jobs, as usual.

#SBATCH --job-name=run_mpi
#SBATCH --nodes=3
#SBATCH --ntasks-per-node=5
#SBATCH --time=00:10:00
#SBATCH --output=logs/%j.slurmlog
#SBATCH --error=logs/opt/run_3_5_%j.slurmlog

# Run mpirun with debugging options to display mapping and binding
mpirun ./trains ./testcases/performance/min.in > ./logs/opt/tmp.out
mpirun ./trains ./testcases/performance/med.in > ./logs/opt/tmp.out
mpirun ./trains ./testcases/performance/stations.in > ./logs/opt/tmp.out
mpirun ./trains ./testcases/performance/trains.in > ./logs/opt/tmp.out
mpirun ./trains ./testcases/performance/traffic.in > ./logs/opt/tmp.out
mpirun ./trains ./testcases/performance/max.in > ./logs/opt/tmp.out
