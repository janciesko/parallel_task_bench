#!/bin/bash
# Heat test driver

#2x AMD EPYC 7402 24-Core Processor


let threads_init=2
let size_init=1024
steps=10

binary=$1

echo "rank_size, num_threads, num_tasks, elems, size(kb), num_steps, time, gups"
rank=1
let size=size_init*$rank
for s in {1..5..1}; do
    threads=$threads_init
    for t in {1..7..1}; do
        if [ $threads -gt 48 ]; then
            threads=48
        fi
        for rep in {1..4..1}; do
            mpirun \
            -x OMP_NUM_THREADS=$threads \
            -x LD_LIBRARY_PATH=/home/jciesko/RAndD/MPIContinuations/software/gcc/install_caraway/lib64:$LD_LIBARY_PATH \
            -npernode 2 \
            -np 1 $binary -s $size -t $steps  | tee ${binary}_$rank.txt
        done
        let threads=$threads*2
    done
    let size=$size*2
done

