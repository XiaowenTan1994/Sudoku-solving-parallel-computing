#!/bin/bash
#SBATCH --ntasks-per-node=4
#SBATCH --nodes=16
#SBATCH --output=mpijob.out
#SBATCH --error=mpijob.err
#SBATCH --export=NONE

source /usr/usc/openmpi/default/setup.sh

srun --ntasks=${SLURM_NTASKS} --mpi=pmi2 ./parallel input.cnf


