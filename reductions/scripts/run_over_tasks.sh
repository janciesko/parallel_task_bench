#!/bin/bash

binary=$1
N_start=$2
cutoff_range=$3
if [ x"$binary" = x ]; then
  echo "Usage: sh run.sh binary problem_size cutoff_range"
  exit 1
fi

export OMP_PROC_BIND=true
export OMP_PLACES=cores
export QTHREAD_STACK_SIZE=8196

num_threads=1

#Print header
echo "app,flavor,N,threads,tasks,size_red_var,time,time_seq,speedup,avg_task_time"

#Run over num threads
for threads in {1..8..1}; do 
  current_size=$N_start
  num_tasks=1
  #Run over cutoff
  for nt in $(seq 1 $cutoff_range); do 
    export OMP_NUM_THREADS=$num_threads
    let num_tasks=$num_tasks*2
    #Repeat
    ./$binary $current_size $num_threads $num_tasks 0 #cutoff unused
    ./$binary $current_size $num_threads $num_tasks 0 #cutoff unused
  done  
  let num_threads=$num_threads*2
done
