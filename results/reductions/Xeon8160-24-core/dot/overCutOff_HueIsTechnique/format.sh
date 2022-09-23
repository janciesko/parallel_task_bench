#!/bin/bash

clang_files=`find $1 -maxdepth 1 -name "*blake01_clang*"`
gnu_files=`find $1 -maxdepth 1 -name "*blake01_g++*"`


APP=dot

rm formatted/*

first=1
for file in $clang_files
do
  THR=1
  for iters in $(seq 1 9); do
    if [ -f "$file" ];then
       if [ $first -eq 1 ]; then 
       	echo "app,flavor,N,threads,tasks,size_red_var,time,time_seq,speedup,avg_task_time" >> formatted/$APP-clang_overCutOff_"$THR"Threads.res
       	
       fi
       cat $file | grep "24, $THR," >> formatted/$APP-clang_overCutOff_"$THR"Threads.res
    fi
    let THR=$THR*2
  done
  let first=0
done


first=1
for file in $gnu_files
do
  THR=1
  for iters in $(seq 1 9); do
    if [ -f "$file" ];then
       if [ $first -eq 1 ]; then
	echo "app,flavor,N,threads,tasks,size_red_var,time,time_seq,speedup,avg_task_time" >> formatted/$APP-g++_overCutOff_"$THR"Threads.res
       fi
       cat $file | grep "24, $THR," >> formatted/$APP-g++_overCutOff_"$THR"Threads.res
    fi
    let THR=$THR*2
  done
  let first=0
done

