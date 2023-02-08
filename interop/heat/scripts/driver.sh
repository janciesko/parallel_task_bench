#!/bin/bash

TYPE=$1

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
    bash ../scripts/run.sh $bin $conf
    #=========================================
    conf=1x2
    echo "$bin $conf"
    #bash ../scripts/run.sh $bin $conf
    #=========================================
    conf=1x4
    echo "$bin $conf"
    #bash ../scripts/run.sh $bin $conf
    #=========================================
    conf=2x1
    echo "$bin $conf"
    bash ../scripts/run.sh $bin $conf
    #=========================================
    conf=4x1
    echo "$bin $conf"
    bash ../scripts/run.sh $bin $conf
fi

if [ x"$TYPE" = heat_OpenMP_TW ]; then
    #=========================================
    #=========================================
    #=========================================
    bin=heat_OpenMP_TW
    #=========================================
    #=========================================
    #=========================================
    conf=1x1
    echo "$bin $conf"
    bash ../scripts/run.sh $bin $conf
    #=========================================
    conf=1x2
    echo "$bin $conf"
    #bash ../scripts/run.sh $bin $conf
    #=========================================
    conf=1x4
    echo "$bin $conf"
    #bash ../scripts/run.sh $bin $conf
    #=========================================
    conf=2x1
    echo "$bin $conf"
    bash ../scripts/run.sh $bin $conf
    #=========================================
    conf=4x1
    echo "$bin $conf"
    bash ../scripts/run.sh $bin $conf
fi

if [ x"$TYPE" = heat_OpenMP_BLOCKING ]; then
    #=========================================
    #=========================================
    #=========================================
    bin=heat_OpenMP_BLOCKING
    #=========================================
    #=========================================
    #=========================================
    conf=1x1
    echo "$bin $conf"
    bash ../scripts/run.sh $bin $conf
    #=========================================
    conf=1x2
    echo "$bin $conf"
    #bash ../scripts/run.sh $bin $conf
    #=========================================
    conf=1x4
    echo "$bin $conf"
    #bash ../scripts/run.sh $bin $conf
    #=========================================
    conf=2x1
    echo "$bin $conf"
    bash ../scripts/run.sh $bin $conf
    #=========================================
    conf=4x1
    echo "$bin $conf"
    bash ../scripts/run.sh $bin $conf
fi