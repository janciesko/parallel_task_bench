# OMP Task Bench (OMP-TB)

OMP-TB is a collection of benchmarks to measure tasking performance and tasking-related features in OpenMP.  Currently it includes benchmark as listed below. Benchmarks in the `reductions` sub-directory target task-parallel reduction support. In general, such benchmarks are useful to evaluate compiler language support as well as its efficient implementation. 

## OMP-TB Benchmarks
- reductions/dot (Dot Product)
- reductions/fib (Fibonacci)
- reductions/powerset (Powerset Permutations)
- reductions/powerset-final (Powerset Permutations using the final OpenMP clause)
- reductions/powerset-UDR (Powerset Permutations using user-defined reductions)
- reductions/others/array_sum (Array Sum)
- reductions/others/knapsack (Knapsack)
- reductions/others/knightstour (Knights Tour)
- reductions/others/max_height_tree (Max Height)
- reductions/others/nbinarywords (n-Permutations)
- reductions/others/nqueens (N-Queens)
- reductions/others/TSP (Travelling Salesman Problem)

## Usage

This software suite uses the CMake meta build system. The following steps show an example of how to configure and build OMP-TB relative to the source root directory for the GCC/g++ compiler and a user-defined reduction over type size of 4 bytes.
```
export COMPILER=g++
export TYPE_SIZE=4
mkdir build && cd build
cmake -DCMAKE_CXX_COMPILER=$COMPILER -DRED_TYPE_SIZE=$TYPE_SIZE
make -j
```

## Performance Insights

Executing the included run scripts on a two-nodes dual-socket 24-core Intel(R) Xeon(R) Platinum 8160 CPU @ 2.10GHz gives the following performance charts. 

<img src=https://user-images.githubusercontent.com/755191/172264551-f3881597-247f-49b8-9276-8e4d1a2fb877.png width=80%>
<img src=https://user-images.githubusercontent.com/755191/172264559-c120e4bd-ea98-4afb-92b4-090b377ba443.png width=80%>
<img src=https://user-images.githubusercontent.com/755191/172264561-7e0bcf3e-5db6-45bd-be37-93ab05cced43.png width=80%>
