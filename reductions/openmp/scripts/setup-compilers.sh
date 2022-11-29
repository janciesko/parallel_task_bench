#!/bin/bash

#set PATHS below to GCC and LLVM

module load hwloc/1.11.8
module load python/3.8.8

# Load GCC
export GCC_HOME=${HOME}/gcc13
export PATH=${GCC_HOME}/bin:${PATH}
export LD_LIBRARY_PATH=${GCC_HOME}/lib64:${LD_LIBRARY_PATH}

# Load CLANG
export LLVM_HOME=${HOME}/llvm14
export PATH=${LLVM_HOME}/bin:${PATH}
export LD_LIBRARY_PATH=${LLVM_HOME}/lib:${LD_LIBRARY_PATH}

