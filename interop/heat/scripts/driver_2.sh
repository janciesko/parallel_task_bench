#!/bin/bash
TYPE=$1
HOSTNAME=$2
if [ x"$TYPE" = xheat_OpenMP ]; then
    #=========================================
    #=========================================
    #=========================================
    bin=heat_OpenMP
    #=========================================
    #=========================================
    #=========================================
    conf=1x1
    echo "$bin $conf"
    bash ../scripts/run_2.sh $bin $conf $HOSTNAME
    #=========================================
    conf=1x2
    echo "$bin $conf"
    #bash ../scripts/run_2.sh $bin $conf $HOSTNAME
    #=========================================
    conf=1x4
    echo "$bin $conf"
    #bash ../scripts/run_2.sh $bin $conf $HOSTNAME
    #=========================================
    conf=2x1
    echo "$bin $conf"
    bash ../scripts/run_2.sh $bin $conf $HOSTNAME
    #=========================================
    conf=4x1
    echo "$bin $conf"
    bash ../scripts/run_2.sh $bin $conf $HOSTNAME
fi

if [ x"$TYPE" = xheat_OpenMP_TW ]; then
    #=========================================
    #=========================================
    #=========================================
    bin=heat_OpenMP_TW
    #=========================================
    #=========================================
    #=========================================
    conf=1x1
    echo "$bin $conf"
    bash ../scripts/run_2.sh $bin $conf $HOSTNAME
    #=========================================
    conf=1x2
    echo "$bin $conf"
    #bash ../scripts/run_2.sh $bin $conf $HOSTNAME
    #=========================================
    conf=1x4
    echo "$bin $conf"
    #bash ../scripts/run_2.sh $bin $conf $HOSTNAME
    #=========================================
    conf=2x1
    echo "$bin $conf"
    bash ../scripts/run_2.sh $bin $conf $HOSTNAME
    conf=4x1
    echo "$bin $conf"
    bash ../scripts/run_2.sh $bin $conf $HOSTNAME
fi

if [ x"$TYPE" = xheat_OpenMP_BLOCKING ]; then
    #=========================================
    #=========================================
    #=========================================
    bin=heat_OpenMP_BLOCKING
    #=========================================
    #=========================================
    #=========================================
    conf=1x1
    echo "$bin $conf"
    bash ../scripts/run_2.sh $bin $conf $HOSTNAME
    #=========================================
    conf=1x2
    echo "$bin $conf"
    #bash ../scripts/run_2.sh $bin $conf $HOSTNAME
    #=========================================
    conf=1x4
    echo "$bin $conf"
    #bash ../scripts/run_2.sh $bin $conf $HOSTNAME
    #=========================================
    conf=2x1
    echo "$bin $conf"
    bash ../scripts/run_2.sh $bin $conf $HOSTNAME
    #=========================================
    conf=4x1
    echo "$bin $conf"
    bash ../scripts/run_2.sh $bin $conf $HOSTNAME
fi
