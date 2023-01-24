#!/bin/bash
# Heat test driver
# Caraway
#2x AMD EPYC 7402 24-Core Processor

BINARY=$1
TYPE=$2
HOSTNAME=$3

threads_init=12
size_init=1024
steps=10

OMP_FLAGS="-x OMP_PROC_BIND=spread -x OMP_PLACES=cores"
MPI_FLAGS="--bind-to none -x HCOLL_RCACHE=^ucs"
#OMP_FLAGS=

LIBGOMP_PATH="/g/g92/ciesko1/lustre/RAndD/MPIContinuations/software/gcc/install/lib64:/g/g92/ciesko1/lustre/RAndD/MPIContinuations/software/ompi/install/lib:/g/g92/ciesko1/lustre/RAndD/MPIContinuations/software/ucx/install/lib"

if [ x"$TYPE" = x1x1 ];
then
    echo "rank_size,num_threads,num_tasks,num_borders,elems,size(kb),num_steps,time,gups" \
        | tee -a ${BINARY}_$ranks_$TYPE.out
    ranks=1
    let size=$size_init*$ranks
    for s in {1..5..1}; do
        threads=$threads_init
        for t in {1..1..1}; do
            if [ $threads -gt 48 ]; then
                threads=48
            fi
            for rep in {1..3..1}; do
                mpirun \
                $MPI_FLAGS \
                -x OMP_NUM_THREADS=$threads $OMP_FLAGS  \
                -npernode $ranks -host $HOSTNAME \
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
    for s in {1..5..1}; do
        threads=$threads_init
        for t in {1..1..1}; do
            if [ $threads -gt 48 ]; then
                threads=48
            fi
            for rep in {1..3..1}; do
                mpirun \
                $MPI_FLAGS \
                -x OMP_NUM_THREADS=$threads $OMP_FLAGS \
                -x LD_LIBRARY_PATH=$LIBGOMP_PATH:$LD_LIBARY_PATH \
                -npernode $ranks -host $HOSTNAME \
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
    for s in {1..5..1}; do
        threads=$threads_init
        for t in {1..1..1}; do
            if [ $threads -gt 48 ]; then
                threads=48
            fi
            for rep in {1..3..1}; do
                mpirun \
                $MPI_FLAGS \
                -x OMP_NUM_THREADS=$threads $OMP_FLAGS \
                -x LD_LIBRARY_PATH=$LIBGOMP_PATH:$LD_LIBARY_PATH \
                -npernode $ranks -host $HOSTNAME \
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
    for s in {1..5..1}; do
        threads=$threads_init
        for t in {1..1..1}; do
            if [ $threads -gt 48 ]; then
                threads=48
            fi
            for rep in {1..3..1}; do
                mpirun \
                $MPI_FLAGS \
                -x OMP_NUM_THREADS=$threads $OMP_FLAGS \
                -x LD_LIBRARY_PATH=$LIBGOMP_PATH:$LD_LIBARY_PATH -x HCOLL_RCACHE=^ucs \
                -npernode 1 -host $HOSTNAME \
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
    for s in {1..5..1}; do
        threads=$threads_init
        for t in {1..1..1}; do
            if [ $threads -gt 48 ]; then
                threads=48
            fi
            for rep in {1..3..1}; do
                mpirun \
                $MPI_FLAGS \
                -x OMP_NUM_THREADS=$threads $OMP_FLAGS \
                -x LD_LIBRARY_PATH=$LIBGOMP_PATH:$LD_LIBARY_PATH -x HCOLL_RCACHE=^ucs \
                -npernode 1 -host $HOSTNAME \
                -np $ranks $BINARY -s $size -t $steps  | tee -a ${BINARY}_$ranks_$TYPE.out
            done
            let threads=$threads*2
        done
        let size=$size*2
    done
fi

