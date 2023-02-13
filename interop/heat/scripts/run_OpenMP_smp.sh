#!/bin/bash
# Heat test driver
# Caraway
#2x AMD EPYC 7402 24-Core Processor

BINARY=$1
TYPE=$2
HOSTNAME=$3

threads_init=1
size_init=256
steps=10

OMP_FLAGS="-x OMP_PROC_BIND=spread -x OMP_PLACES=cores"
MPI_FLAGS="--bind-to none -x HCOLL_RCACHE=^ucs"
#OMP_FLAGS=

LIBGOMP_PATH="/home/jciesko/RAndD/MPIContinuations/software/gcc/install_caraway/lib64/"

if [ x"$TYPE" = x1x1 ];
then
    echo "rank_size,num_threads,num_tasks,num_borders,elems,size(kb),num_steps,time,gups" \
        | tee -a ${BINARY}_$ranks_$TYPE.out
    ranks=1
    let size=$size_init*$ranks
    for s in {1..5..1}; do
        threads=$threads_init
        for t in {1..7..1}; do
            if [ $threads -gt 48 ]; then
                threads=48
            fi
            for rep in {1..3..1}; do
                mpirun \
                $MPI_FLAGS \
                -x OMP_NUM_THREADS=$threads $OMP_FLAGS  \
		-x LD_LIBRARY_PATH=$LIBGOMP_PATH:$LD_LIBARY_PATH \
                -npernode $ranks -host $HOSTNAME \
                -np $ranks $BINARY -s $size -t $steps  | tee -a ${BINARY}_$ranks_$TYPE.out
            done
            let threads=$threads*2
        done
        let size=$size*2
    done
fi
