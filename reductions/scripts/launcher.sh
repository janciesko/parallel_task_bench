#!/bin/bash

#Run this in your build dir
#Make sure g++ and clang++ are in your PATH
source setup-blake-compilers.sh

TYPE=$1

# Helpers:

CLEANUP="rm CMakeCache.txt"
HOSTNAME=`hostname | cut -f 1 -d "."`

benchmark1="fib"

APP10=$benchmark1-parallel-untied
APP11=$benchmark1-taskgroup-untied
APP12=$benchmark1-atomic
APP13=$benchmark1-nored
APP14=$benchmark1-parallel
APP15=$benchmark1-taskgroup
APP16=$benchmark1-threadpriv

benchmark2="powerset"

APP20=$benchmark2-taskloop
APP21=$benchmark2-atomic
APP22=$benchmark2-nored
APP23=$benchmark2-parallel
APP24=$benchmark2-taskgroup
APP25=$benchmark2-threadpriv

benchmark3="dot"

APP30=$benchmark3-taskloop
APP31=$benchmark3-atomic
APP32=$benchmark3-parfor
APP33=$benchmark3-taskloop-untied
APP34=$benchmark3-threadpriv

benchmark4="powerset-UDR"

APP40=$benchmark4-taskloop
APP41=$benchmark4-atomic
APP42=$benchmark4-nored
APP43=$benchmark4-parallel
APP44=$benchmark4-taskgroup
APP45=$benchmark4-threadpriv

TYPE_SIZE=1


# ===================================================
# ===================================================
# Try to compile everything first
# ===================================================
# ===================================================

$CLEANUP
COMPILER="g++"
cmake .. -DCMAKE_CXX_COMPILER=$COMPILER -DRED_TYPE_SIZE=$TYPE_SIZE
make -j

$CLEANUP
COMPILER="clang++"
cmake .. -DCMAKE_CXX_COMPILER=$COMPILER -DRED_TYPE_SIZE=$TYPE_SIZE
make -j


# ===================================================
# ===================================================
# benchmark="fib"
# ===================================================
# ===================================================

if [ x"$TYPE" = x1 ];
then

# ====================================================
# ====================================================
# RUN OVER N
# ====================================================
# ====================================================


# ====================================================
COMPILER="g++"
# ====================================================

run_type="run_over_N.sh"

echo "RUNNING: $run_type"

$CLEANUP
cmake .. -DCMAKE_CXX_COMPILER=$COMPILER -DRED_TYPE_SIZE=$TYPE_SIZE
make -j

APP=$APP10

CUTOFF=0

bash ../scripts/$run_type ./$APP 15 17 $CUTOFF | tee ../../results/$benchmark1/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res

APP=$APP11

bash ../scripts/$run_type ./$APP 15 17 $CUTOFF | tee ../../results/$benchmark1/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res

APP=$APP12

bash ../scripts/$run_type ./$APP 15 17 $CUTOFF | tee ../../results/$benchmark1/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res

APP=$APP13

bash ../scripts/$run_type ./$APP 15 17 $CUTOFF | tee ../../results/$benchmark1/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res

APP=$APP14

bash ../scripts/$run_type ./$APP 15 17 $CUTOFF | tee ../../results/$benchmark1/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res

APP=$APP15

bash ../scripts/$run_type ./$APP 15 17 $CUTOFF | tee ../../results/$benchmark1/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res

APP=$APP16

bash ../scripts/$run_type ./$APP 15 17 $CUTOFF | tee ../../results/$benchmark1/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res


# ====================================================
COMPILER="clang++"
# ====================================================

$CLEANUP
cmake .. -DCMAKE_CXX_COMPILER=$COMPILER -DRED_TYPE_SIZE=$TYPE_SIZE
make -j

APP=$APP10

bash ../scripts/$run_type ./$APP 15 17 $CUTOFF | tee ../../results/$benchmark1/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res

APP=$APP11

bash ../scripts/$run_type ./$APP 15 17 $CUTOFF | tee ../../results/$benchmark1/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res

APP=$APP12

bash ../scripts/$run_type ./$APP 15 17 $CUTOFF | tee ../../results/$benchmark1/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res

APP=$APP13

bash ../scripts/$run_type ./$APP 15 17 $CUTOFF | tee ../../results/$benchmark1/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res

APP=$APP14

bash ../scripts/$run_type ./$APP 15 17 $CUTOFF | tee ../../results/$benchmark1/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res

APP=$APP15

bash ../scripts/$run_type ./$APP 15 17 $CUTOFF | tee ../../results/$benchmark1/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res

APP=$APP16

bash ../scripts/$run_type ./$APP 15 17 $CUTOFF | tee ../../results/$benchmark1/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res

fi

if [ x"$TYPE" = x2 ];
then

# ====================================================
# ====================================================
# RUN OVER CUTOFF
# ====================================================
# ====================================================


# ====================================================
COMPILER="g++"
# ====================================================

run_type="run_over_cutoff.sh"

echo "RUNNING: $run_type"

$CLEANUP
cmake .. -DCMAKE_CXX_COMPILER=$COMPILER -DRED_TYPE_SIZE=$TYPE_SIZE
make -j

APP=$APP10

bash ../scripts/$run_type ./$APP 33 20 | tee ../../results/$benchmark1/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overCutOff.res

APP=$APP11

bash ../scripts/$run_type ./$APP 33 20 | tee ../../results/$benchmark1/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overCutOff.res

APP=$APP12

bash ../scripts/$run_type ./$APP 33 20 | tee ../../results/$benchmark1/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overCutOff.res

APP=$APP13

bash ../scripts/$run_type ./$APP 33 20 | tee ../../results/$benchmark1/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overCutOff.res

APP=$APP14

bash ../scripts/$run_type ./$APP 33 20 | tee ../../results/$benchmark1/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overCutOff.res

APP=$APP15

bash ../scripts/$run_type ./$APP 33 20 | tee ../../results/$benchmark1/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overCutOff.res

APP=$APP16

bash ../scripts/$run_type ./$APP 33 20 | tee ../../results/$benchmark1/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overCutOff.res

# ====================================================
COMPILER="clang++"
# ====================================================

$CLEANUP
cmake .. -DCMAKE_CXX_COMPILER=$COMPILER -DRED_TYPE_SIZE=$TYPE_SIZE
make -j

APP=$APP10

bash ../scripts/$run_type ./$APP 33 20 | tee ../../results/$benchmark1/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overCutOff.res

APP=$APP11

bash ../scripts/$run_type ./$APP 33 20 | tee ../../results/$benchmark1/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overCutOff.res

APP=$APP12

bash ../scripts/$run_type ./$APP 33 20 | tee ../../results/$benchmark1/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overCutOff.res

APP=$APP13

bash ../scripts/$run_type ./$APP 33 20 | tee ../../results/$benchmark1/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overCutOff.res

APP=$APP14

bash ../scripts/$run_type ./$APP 33 20 | tee ../../results/$benchmark1/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overCutOff.res

APP=$APP15

bash ../scripts/$run_type ./$APP 33 20 | tee ../../results/$benchmark1/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overCutOff.res

APP=$APP16

bash ../scripts/$run_type ./$APP 33 20 | tee ../../results/$benchmark1/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overCutOff.res

fi


# ===================================================
# ===================================================
# benchmark="powerset"
# ===================================================
# ===================================================


if [ x"$TYPE" = x3 ];
then

# ====================================================
# ====================================================
# RUN OVER N
# ====================================================
# ====================================================


# ====================================================
COMPILER="g++"
# ====================================================

run_type="run_over_N.sh"

echo "RUNNING: $run_type"

$CLEANUP
cmake .. -DCMAKE_CXX_COMPILER=$COMPILER -DRED_TYPE_SIZE=$TYPE_SIZE
make -j

CUTOFF=500 #Make this big so we do not cutoff in this run

APP=$APP20

bash ../scripts/$run_type ./$APP 10 8 $CUTOFF| tee ../../results/$benchmark2/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res

APP=$APP21

bash ../scripts/$run_type ./$APP 10 8 $CUTOFF| tee ../../results/$benchmark2/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res

APP=$APP22

bash ../scripts/$run_type ./$APP 10 8 $CUTOFF| tee ../../results/$benchmark2/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res

APP=$APP23

bash ../scripts/$run_type ./$APP 10 8 $CUTOFF| tee ../../results/$benchmark2/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res

APP=$APP24

bash ../scripts/$run_type ./$APP 10 8 $CUTOFF| tee ../../results/$benchmark2/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res

APP=$APP25

bash ../scripts/$run_type ./$APP 10 8 $CUTOFF| tee ../../results/$benchmark2/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res


# ====================================================
COMPILER="clang++"
# ====================================================

$CLEANUP
cmake .. -DCMAKE_CXX_COMPILER=$COMPILER -DRED_TYPE_SIZE=$TYPE_SIZE
make -j

APP=$APP20

bash ../scripts/$run_type ./$APP 10 8 $CUTOFF| tee ../../results/$benchmark2/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res

APP=$APP21

bash ../scripts/$run_type ./$APP 10 8 $CUTOFF| tee ../../results/$benchmark2/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res

APP=$APP22

bash ../scripts/$run_type ./$APP 10 8 $CUTOFF| tee ../../results/$benchmark2/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res

APP=$APP23

bash ../scripts/$run_type ./$APP 10 8 $CUTOFF| tee ../../results/$benchmark2/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res

APP=$APP24

bash ../scripts/$run_type ./$APP 10 8 $CUTOFF| tee ../../results/$benchmark2/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res

APP=$APP25

bash ../scripts/$run_type ./$APP 10 8 $CUTOFF| tee ../../results/$benchmark2/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res

fi

if [ x"$TYPE" = x4 ];
then

# ====================================================
# ====================================================
# RUN OVER CUTOFF
# ====================================================
# ====================================================


# ====================================================
COMPILER="g++"
# ====================================================

run_type="run_over_cutoff.sh"

echo "RUNNING: $run_type"

$CLEANUP
cmake .. -DCMAKE_CXX_COMPILER=$COMPILER -DRED_TYPE_SIZE=$TYPE_SIZE
make -j

APP=$APP20

bash ../scripts/$run_type ./$APP 18 18 | tee ../../results/$benchmark2/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overCutOff.res

APP=$APP21

bash ../scripts/$run_type ./$APP 18 18 | tee ../../results/$benchmark2/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overCutOff.res

APP=$APP22

bash ../scripts/$run_type ./$APP 18 18 | tee ../../results/$benchmark2/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overCutOff.res

APP=$APP23

bash ../scripts/$run_type ./$APP 18 18 | tee ../../results/$benchmark2/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overCutOff.res

APP=$APP24

bash ../scripts/$run_type ./$APP 18 18 | tee ../../results/$benchmark2/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overCutOff.res

APP=$APP25

bash ../scripts/$run_type ./$APP 18 18 | tee ../../results/$benchmark2/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overCutOff.res

# ====================================================
COMPILER="clang++"
# ====================================================

$CLEANUP
cmake .. -DCMAKE_CXX_COMPILER=$COMPILER -DRED_TYPE_SIZE=$TYPE_SIZE
make -j

APP=$APP20

bash ../scripts/$run_type ./$APP 18 18 | tee ../../results/$benchmark2/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overCutOff.res

APP=$APP21

bash ../scripts/$run_type ./$APP 18 18 | tee ../../results/$benchmark2/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overCutOff.res

APP=$APP22

bash ../scripts/$run_type ./$APP 18 18 | tee ../../results/$benchmark2/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overCutOff.res

APP=$APP23

bash ../scripts/$run_type ./$APP 18 18 | tee ../../results/$benchmark2/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overCutOff.res

APP=$APP24

bash ../scripts/$run_type ./$APP 18 18 | tee ../../results/$benchmark2/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overCutOff.res

APP=$APP25

bash ../scripts/$run_type ./$APP 18 18 | tee ../../results/$benchmark2/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overCutOff.res

fi

# ===================================================
# ===================================================
# benchmark="dot"
# ===================================================
# ===================================================


if [ x"$TYPE" = x5 ];
then

# ====================================================
# ====================================================
# RUN OVER N
# ====================================================
# ====================================================


# ====================================================
COMPILER="g++"
# ====================================================

run_type="run_over_N.sh"

echo "RUNNING: $run_type"

$CLEANUP
cmake .. -DCMAKE_CXX_COMPILER=$COMPILER -DRED_TYPE_SIZE=$TYPE_SIZE
make -j

CUTOFF=500 #Unused

APP=$APP30

bash ../scripts/$run_type ./$APP 22 8 $CUTOFF| tee ../../results/$benchmark3/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res

APP=$APP31

bash ../scripts/$run_type ./$APP 22 8 $CUTOFF| tee ../../results/$benchmark3/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res

APP=$APP32

bash ../scripts/$run_type ./$APP 22 8 $CUTOFF| tee ../../results/$benchmark3/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res

APP=$APP33

bash ../scripts/$run_type ./$APP 22 8 $CUTOFF| tee ../../results/$benchmark3/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res

APP=$APP34

bash ../scripts/$run_type ./$APP 22 8 $CUTOFF| tee ../../results/$benchmark3/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res


# ====================================================
COMPILER="clang++"
# ====================================================

$CLEANUP
cmake .. -DCMAKE_CXX_COMPILER=$COMPILER -DRED_TYPE_SIZE=$TYPE_SIZE
make -j

APP=$APP30

bash ../scripts/$run_type ./$APP 22 8 $CUTOFF| tee ../../results/$benchmark3/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res

APP=$APP31

bash ../scripts/$run_type ./$APP 22 8 $CUTOFF| tee ../../results/$benchmark3/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res

APP=$APP32

bash ../scripts/$run_type ./$APP 22 8 $CUTOFF| tee ../../results/$benchmark3/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res

APP=$APP33

bash ../scripts/$run_type ./$APP 22 8 $CUTOFF| tee ../../results/$benchmark3/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res

APP=$APP34

bash ../scripts/$run_type ./$APP 22 8 $CUTOFF| tee ../../results/$benchmark3/overN/"$APP"_"$HOSTNAME"_"$COMPILER"_overN.res

fi

if [ x"$TYPE" = x6 ];
then

# ====================================================
# ====================================================
# RUN OVER NUM_TASKS
# ====================================================
# ====================================================


# ====================================================
COMPILER="g++"
# ====================================================

run_type="run_over_tasks.sh"

echo "RUNNING: $run_type"

$CLEANUP
cmake .. -DCMAKE_CXX_COMPILER=$COMPILER -DRED_TYPE_SIZE=$TYPE_SIZE
make -j

CUTOFF=500 #Unused

APP=$APP30

bash ../scripts/$run_type ./$APP 24 17 $CUTOFF| tee ../../results/$benchmark3/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overNT.res

APP=$APP31

bash ../scripts/$run_type ./$APP 24 17 $CUTOFF| tee ../../results/$benchmark3/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overNT.res

APP=$APP32

bash ../scripts/$run_type ./$APP 24 17 $CUTOFF| tee ../../results/$benchmark3/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overNT.res

APP=$APP33

bash ../scripts/$run_type ./$APP 24 17 $CUTOFF| tee ../../results/$benchmark3/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overNT.res

APP=$APP34

bash ../scripts/$run_type ./$APP 24 17 $CUTOFF| tee ../../results/$benchmark3/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overNT.res


# ====================================================
COMPILER="clang++"
# ====================================================

$CLEANUP
cmake .. -DCMAKE_CXX_COMPILER=$COMPILER -DRED_TYPE_SIZE=$TYPE_SIZE
make -j

APP=$APP30

bash ../scripts/$run_type ./$APP 24 17 $CUTOFF| tee ../../results/$benchmark3/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overNT.res

APP=$APP31

bash ../scripts/$run_type ./$APP 24 17 $CUTOFF| tee ../../results/$benchmark3/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overNT.res

APP=$APP32

bash ../scripts/$run_type ./$APP 24 17 $CUTOFF| tee ../../results/$benchmark3/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overNT.res

APP=$APP33

bash ../scripts/$run_type ./$APP 24 17 $CUTOFF| tee ../../results/$benchmark3/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overNT.res

APP=$APP34

bash ../scripts/$run_type ./$APP 24 17 $CUTOFF| tee ../../results/$benchmark3/overCutOff/"$APP"_"$HOSTNAME"_"$COMPILER"_overNT.res

fi


# ===================================================
# ===================================================
# benchmark="powerset-UDR"
# ===================================================
# ===================================================


if [ x"$TYPE" = x7 ];
then

# ====================================================
# ====================================================
# RUN OVER UDR SIZE
# ====================================================
# ====================================================


# ====================================================
COMPILER="g++"
# ====================================================

run_type="run_single.sh"

echo "RUNNING: $run_type"

CUTOFF=500

TYPE_SIZE=1

for RT_SIZE in {1..16..1}; do 

    $CLEANUP
    cmake .. -DCMAKE_CXX_COMPILER=$COMPILER -DRED_TYPE_SIZE=$TYPE_SIZE
    make -j

    APP=$APP40

    bash ../scripts/$run_type ./$APP 18 $TYPE_SIZE| tee -a ../../results/$benchmark4/overRT/"$APP"_"$HOSTNAME"_"$COMPILER"_overRT.res

    APP=$APP41

    bash ../scripts/$run_type ./$APP 18 $TYPE_SIZE| tee -a ../../results/$benchmark4/overRT/"$APP"_"$HOSTNAME"_"$COMPILER"_overRT.res

    APP=$APP42

    bash ../scripts/$run_type ./$APP 18 $TYPE_SIZE| tee -a ../../results/$benchmark4/overRT/"$APP"_"$HOSTNAME"_"$COMPILER"_overRT.res

    APP=$APP43

    bash ../scripts/$run_type ./$APP 18 $TYPE_SIZE| tee -a ../../results/$benchmark4/overRT/"$APP"_"$HOSTNAME"_"$COMPILER"_overRT.res

    APP=$APP44

    bash ../scripts/$run_type ./$APP 18 $TYPE_SIZE| tee -a ../../results/$benchmark4/overRT/"$APP"_"$HOSTNAME"_"$COMPILER"_overRT.res

    APP=$APP45

    bash ../scripts/$run_type ./$APP 18 $TYPE_SIZE| tee -a ../../results/$benchmark4/overRT/"$APP"_"$HOSTNAME"_"$COMPILER"_overRT.res

    let TYPE_SIZE=$TYPE_SIZE*2

done

# ====================================================
COMPILER="clang++"
# ====================================================

TYPE_SIZE=1

for RT_SIZE in {1..16..1}; do 

    $CLEANUP
    cmake .. -DCMAKE_CXX_COMPILER=$COMPILER -DRED_TYPE_SIZE=$TYPE_SIZE
    make -j

    APP=$APP40

    bash ../scripts/$run_type ./$APP 18 $TYPE_SIZE| tee -a ../../results/$benchmark4/overRT/"$APP"_"$HOSTNAME"_"$COMPILER"_overRT.res

    APP=$APP41

    bash ../scripts/$run_type ./$APP 18 $TYPE_SIZE| tee -a ../../results/$benchmark4/overRT/"$APP"_"$HOSTNAME"_"$COMPILER"_overRT.res

    APP=$APP42

    bash ../scripts/$run_type ./$APP 18 $TYPE_SIZE| tee -a ../../results/$benchmark4/overRT/"$APP"_"$HOSTNAME"_"$COMPILER"_overRT.res

    APP=$APP43

    bash ../scripts/$run_type ./$APP 18 $TYPE_SIZE| tee -a ../../results/$benchmark4/overRT/"$APP"_"$HOSTNAME"_"$COMPILER"_overRT.res

    APP=$APP44

    bash ../scripts/$run_type ./$APP 18 $TYPE_SIZE| tee -a ../../results/$benchmark4/overRT/"$APP"_"$HOSTNAME"_"$COMPILER"_overRT.res

    APP=$APP45

    bash ../scripts/$run_type ./$APP 18 $TYPE_SIZE| tee -a ../../results/$benchmark4/overRT/"$APP"_"$HOSTNAME"_"$COMPILER"_overRT.res

    let TYPE_SIZE=$TYPE_SIZE*2

done

fi
