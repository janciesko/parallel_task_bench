cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0074 NEW)

set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)

# set the project name
project(omp-continue-test VERSION 1.0 LANGUAGES C)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/modules")
message(STATUS "CMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}")

option(GDB "start the attach loops so that gdb can be attached to each proc" OFF)

set(HEADER_DIRS)
set(SOURCE_DIRS)
set(SOURCES)
set(HEADERS)
list(APPEND HEADER_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/common")

# add subdirectories
list(APPEND SOURCE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/basic")

#set(OMP_MIN_VERSION 5.0)
#
#find_package(OpenMP ${OMP_MIN_VERSION})
#if(OPENMP_FOUND)
#  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#else()
#  message(STATUS "OPENMP SUPPORT NOT FOUND")  
#endif()

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
  message(SEND_ERROR "a build of MPI supporting the continuations feature is required to build continue-basic")
endif(NOT CONTINUATIONS_FOUND)

find_package(Qthreads REQUIRED)

foreach(DIR ${SOURCE_DIRS})
  file(GLOB SUBDIR_HEADERS ${DIR}/*.h)  
  file(GLOB SUBDIR_SOURCES ${DIR}/*.c)
  list(APPEND HEADERS ${SUBDIR_HEADERS})
  list(APPEND SOURCES ${SUBDIR_SOURCES})
endforeach()

foreach(SRC_FILE ${SOURCES})
  get_filename_component(SRC_FILE_NAME ${SRC_FILE} NAME)
  string(REGEX REPLACE "\\.[^.]*$" "" SRC_FILE_NAME ${SRC_FILE_NAME})
  add_executable(${SRC_FILE_NAME} ${SRC_FILE} ${HEADERS})  
  target_include_directories(${SRC_FILE_NAME} PRIVATE ${MPI_INCLUDE_PATH})
  message(STATUS "QT INCL PATH ${QT_INCLUDE_PATH}")
  target_include_directories(${SRC_FILE_NAME} PRIVATE ${QT_INCLUDE_PATH})
  target_link_libraries(${SRC_FILE_NAME} ${MPI_LIBRARIES})
  target_link_libraries(${SRC_FILE_NAME} ${QT_LINK_LIBRARIES})
  add_compile_options(-Wall -Wextra -pedantic -Werror -O3)
  if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${SRC_FILE_NAME} PUBLIC "DEBUG=1")
    message(STATUS "CMAKE_BUILD_TYPE=DEBUG")  
  endif()
  if (GDB)
    target_compile_definitions(${SRC_FILE_NAME} PUBLIC "GDB=1")
    message(STATUS "GDB ATTACH LOOP ENABLED")
  endif()  
endforeach()

#install(TARGETS test DESTINATION bin)
#install(FILES "${PROJECT_BINARY_DIR}/TutorialConfig.h" DESTINATION include)
