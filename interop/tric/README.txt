Examples for configuring with cmake and building:
mkdir build
cd build
=== Default ===
cmake  .. -DCMAKE_CXX_COMPILER=mpicxx -DCMAKE_C_COMPILER=mpicc -DMPI_INCLUDE_PATH=$MPI_HOME/include -DCMAKE_BUILD_TYPE=Debug
=== QTHREADS ===
cmake  .. -DCMAKE_CXX_COMPILER=mpicxx -DCMAKE_C_COMPILER=mpicc -DMPI_INCLUDE_PATH=$MPI_HOME/include -DCMAKE_BUILD_TYPE=Debug -DQthreads_ROOT=$QTHREADS_HOME -DENABLE_QTHREADS=True
=== ARGOBOTS ===
cmake  .. -DCMAKE_CXX_COMPILER=mpicxx -DCMAKE_C_COMPILER=mpicc -DMPI_INCLUDE_PATH=$MPI_HOME/include -DCMAKE_BUILD_TYPE=Debug -DArgobots_ROOT=$ABT_HOME -DENABLE_ARGOBOTS=True
=== OPENMP ===
cmake  .. -DCMAKE_CXX_COMPILER=mpicxx -DCMAKE_C_COMPILER=mpicc -DMPI_INCLUDE_PATH=$MPI_HOME -DENABLE_OPENMP=True -DCMAKE_BUILD_TYPE=Debug

Run:
mpirun -np 2 ./tric_$BACKEND

