cmake_minimum_required(VERSION 3.10)

cmake_policy(SET CMP0074 NEW)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/modules)

set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)

# set the project name
project(omp-continue-test VERSION 1.0 LANGUAGES C CXX)

option(GDB "start the attach loops so that gdb can be attached to each proc" OFF)
option(ENABLE_OPENMP "Whether to build implementation using OPENMP" OFF)
option(ENABLE_ARGOBOTS "Whether to build implementation using ARGOBOTS" OFF)
option(ENABLE_QTHREADS "Whether to build implementation using QTHREADS" OFF)
option(ENABLE_PTREADS "Whether to build implementation using PTHREADS" OFF)

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
if (NOT CONTINUATIONS_FOUND)
  message(SEND_ERROR "A build of MPI supporting the continuations feature is required to build continue-basic")
endif(NOT CONTINUATIONS_FOUND)

if(ENABLE_OPENMP)
  set(BACKEND_NAME "OpenMP")

  find_package(OpenMP ${OMP_MIN_VERSION})
  if(OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
  else()
    message(SEND_ERROR "OPENMP SUPPORT NOT FOUND")
  endif()

endif()

if(ENABLE_PTHREADS)
  set(BACKEND_NAME "Pthreads")
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
list(APPEND HEADER_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/util")

# add subdirectories
string(TOLOWER ${BACKEND_NAME} BACKEND_NAME_LOWER)
list(APPEND SOURCE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/${BACKEND_NAME_LOWER}")

message(STATUS ${BACKEND_NAME_LOWER})

#Add utils lib
file(GLOB SUBDIR_UTIL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/util/*.c)
file(GLOB SUBDIR_UTIL_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/util/*.h)  
add_library( osu_util SHARED ${SUBDIR_UTIL_SOURCES})
target_include_directories(osu_util PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/util/)
message (STATUS ${SUBDIR_UTIL_SOURCES})

#Add targets
foreach(DIR ${SOURCE_DIRS})
  file(GLOB SUBDIR_SOURCES ${DIR}/*.c)
  file(GLOB SUBDIR_HEADERS ${DIR}/*.h)  

  foreach(SRC_FILE ${SUBDIR_SOURCES})
    get_filename_component(TARGET_NAME ${SRC_FILE} NAME)
    string(REGEX REPLACE "\\.[^.]*$" "" TARGET_NAME ${TARGET_NAME})
    set(TARGET_NAME "${TARGET_NAME}_${BACKEND_NAME}")

    message(STATUS "DIR: ${DIR}")
    message(STATUS "TARGET_NAME: " ${TARGET_NAME})

    add_executable(${TARGET_NAME} ${SRC_FILE})
    target_link_libraries(${TARGET_NAME} osu_util)
    target_include_directories(${TARGET_NAME} PRIVATE ${HEADER_DIRS})
    add_compile_options(-Wall -Wextra -pedantic -O3)

    if(ENABLE_ARGOBOTS)
       target_include_directories(${TARGET_NAME} PRIVATE ${ABT_INCLUDE_PATH})
       target_link_libraries(${TARGET_NAME} ${ABT_LINK_LIBRARIES})
    endif()

    if(ENABLE_QTHREADS)
      target_include_directories(${TARGET_NAME} PRIVATE ${QT_INCLUDE_PATH})
      target_link_libraries(${TARGET_NAME} ${QT_LINK_LIBRARIES})    
    endif()

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
      target_compile_definitions(${TARGET_NAME} PUBLIC "DEBUG=1")
      message(STATUS "CMAKE_BUILD_TYPE=DEBUG")  
    endif()
    if (GDB)
      target_compile_definitions(${TARGET_NAME} PUBLIC "GDB=1")
      message(STATUS "GDB ATTACH LOOP ENABLED")
    endif()
  endforeach()
endforeach()

