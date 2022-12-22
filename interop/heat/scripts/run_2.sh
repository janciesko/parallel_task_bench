#!/bin/bash
# Heat test driver
# Caraway
#2x AMD EPYC 7402 24-Core Processor

BINARY=$1
TYPE=$2

threads_init=48
size_init=1024
steps=10

if [ x"$TYPE" = x1x1 ];
then
    echo "rank_size,num_threads,num_tasks,num_borders,elems,size(kb),num_steps,time,gups" \
        | tee -a ${BINARY}_$ranks_$TYPE.out
    ranks=1
    let size=$size_init*$ranks
    for s in {1..6..1}; do
        threads=$threads_init
        for t in {1..1..1}; do
            if [ $threads -gt 48 ]; then
                threads=48
            fi
            for rep in {1..3..1}; do
                mpirun \
                -x OMP_NUM_THREADS=$threads -x OMP_PROC_BIND=spread -x OMP_PLACES=cores \
                -x LD_LIBRARY_PATH=/home/jciesko/RAndD/MPIContinuations/software/gcc/install_caraway/lib64:$LD_LIBARY_PATH \
                -npernode $ranks -host caraway02 \
                -np $ranks $BINARY -s $size -t $steps  | tee -a ${BINARY}_$ranks_$TYPE.out
            done
            let threads=$threads*2
        done
        let size=$size*2
    done
fi

if [ x"$TYPE" = x1x2 ];
then
    echo "rank_size,num_threads,num_tasks,num_borders,elems,size(kb),num_steps,time,gups" \
        | tee -a ${BINARY}_$ranks_$TYPE.out
    ranks=2
    let size=$size_init*$ranks
    for s in {1..6..1}; do
        threads=$threads_init
        for t in {1..1..1}; do
            if [ $threads -gt 48 ]; then
                threads=48
            fi
            for rep in {1..3..1}; do
                mpirun \
                -x OMP_NUM_THREADS=$threads -x OMP_PROC_BIND=spread -x OMP_PLACES=cores \
                -x LD_LIBRARY_PATH=/home/jciesko/RAndD/MPIContinuations/software/gcc/install_caraway/lib64:$LD_LIBARY_PATH \
                -npernode $ranks -host caraway02,caraway02 \
                -np $ranks $BINARY -s $size -t $steps  | tee -a ${BINARY}_$ranks_$TYPE.out
            done
            let threads=$threads*2
        done
        let size=$size*2
    done
fi

if [ x"$TYPE" = x1x4 ];
then
    echo "rank_size,num_threads,num_tasks,num_borders,elems,size(kb),num_steps,time,gups" \
        | tee -a ${BINARY}_$ranks_$TYPE.out
    ranks=4
    let size=$size_init*$ranks
    for s in {1..6..1}; do
        threads=$threads_init
        for t in {1..1..1}; do
            if [ $threads -gt 48 ]; then
                threads=48
            fi
            for rep in {1..3..1}; do
                mpirun \
                -x OMP_NUM_THREADS=$threads -x OMP_PROC_BIND=spread -x OMP_PLACES=cores \
                -x LD_LIBRARY_PATH=/home/jciesko/RAndD/MPIContinuations/software/gcc/install_caraway/lib64:$LD_LIBARY_PATH \
                -npernode $ranks -host caraway02,caraway02,caraway02,caraway02  \
                -np $ranks $BINARY -s $size -t $steps  | tee -a ${BINARY}_$ranks_$TYPE.out
            done
            let threads=$threads*2
        done
        let size=$size*2
    done
fi

if [ x"$TYPE" = x2x1 ];
then
    echo "rank_size,num_threads,num_tasks,num_borders,elems,size(kb),num_steps,time,gups" \
        | tee -a ${BINARY}_$ranks_$TYPE.out
    ranks=2
    let size=$size_init*$ranks
    for s in {1..6..1}; do
        threads=$threads_init
        for t in {1..1..1}; do
            if [ $threads -gt 48 ]; then
                threads=48
            fi
            for rep in {1..3..1}; do
                mpirun \
                -x OMP_NUM_THREADS=$threads -x OMP_PROC_BIND=spread -x OMP_PLACES=cores \
                -x LD_LIBRARY_PATH=/home/jciesko/RAndD/MPIContinuations/software/gcc/install_caraway/lib64:$LD_LIBARY_PATH \
                -npernode 1 -host caraway02,caraway03 \
                -np $ranks $BINARY -s $size -t $steps  | tee -a ${BINARY}_$ranks_$TYPE.out
            done
            let threads=$threads*2
        done
        let size=$size*2
    done
fi

if [ x"$TYPE" = x4x1 ];
then
    echo "rank_size,num_threads,num_tasks,num_borders,elems,size(kb),num_steps,time,gups" \
        | tee -a ${BINARY}_$ranks_$TYPE.out
    ranks=4
    let size=$size_init*$ranks
    for s in {1..6..1}; do
        threads=$threads_init
        for t in {1..1..1}; do
            if [ $threads -gt 48 ]; then
                threads=48
            fi
            for rep in {1..3..1}; do
                mpirun \
                -x OMP_NUM_THREADS=$threads -x OMP_PROC_BIND=spread -x OMP_PLACES=cores \
                -x LD_LIBRARY_PATH=/home/jciesko/RAndD/MPIContinuations/software/gcc/install_caraway/lib64:$LD_LIBARY_PATH \
                -npernode 1 -host caraway01,caraway02,caraway03,caraway04 \
                -np $ranks $BINARY -s $size -t $steps  | tee -a ${BINARY}_$ranks_$TYPE.out
            done
            let threads=$threads*2
        done
        let size=$size*2
    done
fi

