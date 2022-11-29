#!/bin/bash

binary=$1
N_start=$2
N_length=$3
if [ x"$binary" = x ]; then
  echo "Usage: sh run.sh binary problem_size problem_size_range"
  exit 1
fi

export OMP_PROC_BIND=true
export OMP_PLACES=cores
export QTHREAD_STACK_SIZE=8196

num_threads=1
num_tasks=$num_threads
cutoff=$4

#Print header
echo "app,flavor,N,threads,tasks,size_red_var,time,time_seq,speedup,avg_task_time"

#Run over num threads
for threads in {1..9..1}; do 
  current_size=$N_start
  #Run over sizes
  for s in $(seq 1 $N_length); do 
    export OMP_NUM_THREADS=$num_threads
    num_tasks=$num_threads
    #Run app
    ./$binary $current_size $num_threads $num_tasks $cutoff
    ./$binary $current_size $num_threads $num_tasks $cutoff
    let current_size=$current_size+1
  done
  let num_threads=$num_threads*2
done
