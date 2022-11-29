#!/bin/bash

binary=$1
size=$2
RT_SIZE=$3

export OMP_PROC_BIND=true
export OMP_PLACES=cores

num_threads=48
num_tasks=$num_threads
cut_off=500

#Print header once as we keep appending into a file.
if [ x"$RT_SIZE" = x1 ];
then
  echo "app,flavor,N,threads,tasks,size_red_var,time,time_seq,speedup,avg_task_time"
fi

export OMP_NUM_THREADS=48
#Repeat
./$binary $size $num_threads $num_tasks $cut_off
./$binary $size $num_threads $num_tasks $cut_off

  
  

