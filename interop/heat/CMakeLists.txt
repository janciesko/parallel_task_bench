cmake_minimum_required(VERSION 3.10)

cmake_policy(SET CMP0074 NEW)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/modules)

set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)

# set the project name
project(omp-continue-test VERSION 1.0 LANGUAGES C CXX)

option(ENABLE_OPENMP "Whether to build implementation using OPENMP" OFF)
option(ENABLE_ARGOBOTS "Whether to build implementation using ARGOBOTS" OFF)
option(ENABLE_QTHREADS "Whether to build implementation using QTHREADS" OFF)
option(ENABLE_BENCHMARK_OUTPUT "Whether to build with printf outputs for benchmarking" OFF)
option(ENABLE_USE_BLOCKINGMPI "Whether to use blocking MPI calls" OFF)
option(ENABLE_USE_TAKWAIT "Whether to use a taskwait at solve" OFF)

set(OMP_MIN_VERSION 4.5)

find_package(MPI REQUIRED)
if (NOT MPI_FOUND)
  message(SEND_ERROR "This application cannot compile without MPI")
endif (NOT MPI_FOUND)

set(CONT_SEARCH_DIRS)
foreach(DIR ${MPI_INCLUDE_PATH})
  list(APPEND CONT_SEARCH_DIRS "${DIR}/openmpi/mpiext")
endforeach()

find_path(CONTINUATIONS_FOUND "mpiext_continue_c.h" PATHS ${CONT_SEARCH_DIRS})
message (STATUS ${CONT_SEARCH_DIRS})
if (NOT CONTINUATIONS_FOUND)
  message(SEND_ERROR "A build of MPI supporting the continuations feature is required to build continue-basic")
endif(NOT CONTINUATIONS_FOUND)

if(ENABLE_OPENMP)
  set(BACKEND_NAME "OpenMP")
  find_package(OpenMP ${OMP_MIN_VERSION} REQUIRED)
  set(OpenMP_CXX "${CMAKE_CXX_COMPILER}")
  set(OpenMP_CXX_FLAGS "-fopenmp=libomp -Wno-unused-command-line-argument")
  set(OpenMP_CXX_LIB_NAMES "libomp" "libgomp" "libiomp5")
  set(OpenMP_libomp_LIBRARY ${OpenMP_CXX_LIB_NAMES})
  set(OpenMP_libgomp_LIBRARY ${OpenMP_CXX_LIB_NAMES})
  set(OpenMP_libiomp5_LIBRARY ${OpenMP_CXX_LIB_NAMES})
endif()

if(ENABLE_ARGOBOTS)
  set(BACKEND_NAME "Argobots")
  find_package(Argobots REQUIRED)
endif()

if(ENABLE_QTHREADS)
  set(BACKEND_NAME "Qthreads")
  find_package(Qthreads REQUIRED)
endif()

list(LENGTH BACKEND_NAME N_BACKENDS)
if (NOT ${N_BACKENDS} EQUAL "1")
  message(FATAL_ERROR "Must give a single valid backend")
endif()

message(STATUS "Compiling for the ${BACKEND_NAME} backend.")

set(HEADER_DIRS)
list(APPEND HEADER_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/utils")

# add subdirectories
string(TOLOWER ${BACKEND_NAME} BACKEND_NAME_LOWER)
list(APPEND SOURCE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/${BACKEND_NAME_LOWER}")

message(STATUS ${BACKEND_NAME_LOWER})

foreach(DIR ${SOURCE_DIRS})
  file(GLOB SUBDIR_SOURCES ${DIR}/*.cpp)
  file(GLOB SUBDIR_HEADERS ${DIR}/*.hpp)  

  set(TARGET_NAME "heat_${BACKEND_NAME}")

  message(STATUS "DIR: ${DIR}")
  message(STATUS "TARGET_NAME: " ${TARGET_NAME})

  add_executable(${TARGET_NAME} ${SUBDIR_SOURCES} ${SUBDIR_HEADERS})
  target_include_directories(${TARGET_NAME} PRIVATE ${DIR})
  target_include_directories(${TARGET_NAME} PRIVATE ${HEADER_DIRS})
  add_compile_options(-Wall -Wextra -pedantic -O3)

  if(ENABLE_ARGOBOTS)
     target_include_directories(${TARGET_NAME} PRIVATE ${ABT_INCLUDE_PATH})
     target_link_libraries(${TARGET_NAME} PUBLIC ${ABT_LINK_LIBRARIES} MPI::MPI_CXX)
  endif()

  if(ENABLE_QTHREADS)
    target_include_directories(${TARGET_NAME} PRIVATE ${QT_INCLUDE_PATH})
    target_link_libraries(${TARGET_NAME} PUBLIC ${QT_LINK_LIBRARIES} MPI::MPI_CXX)
  endif()

  if(ENABLE_OPENMP)
     target_link_libraries(${TARGET_NAME} PUBLIC OpenMP::OpenMP_CXX MPI::MPI_CXX)
  endif()

  if(ENABLE_BENCHMARK_OUTPUT)
    target_compile_definitions(${TARGET_NAME} PRIVATE "BENCHOUTPUT")
    message(STATUS "ENABLE_BENCHMARK_OUTPUT enabled.")
  endif()

  if(ENABLE_USE_BLOCKINGMPI)
    target_compile_definitions(${TARGET_NAME} PRIVATE "BLOCKINGMPI")
    message(STATUS "ENABLE_USE_BLOCKINGMPI enabled.")
  endif()

  if(ENABLE_USE_TAKWAIT)
  target_compile_definitions(${TARGET_NAME} PRIVATE "TASKWAIT")
  message(STATUS "ENABLE_USE_TAKWAIT enabled.")
endif()

  if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${TARGET_NAME} PUBLIC "DEBUG=1")
    message(STATUS "CMAKE_BUILD_TYPE=DEBUG")  
  endif()
endforeach()

