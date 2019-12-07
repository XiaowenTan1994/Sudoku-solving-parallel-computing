#!/bin/sh
#SBATCH --gres=gpu:1

#SBATCH -o cuda1.out

#SBATCH -e cuda1.error

source /usr/usc/cuda/default/setup.sh

nvcc  -o cudasudoku debug.cu

./cudasudoku 1 1 input.txt



