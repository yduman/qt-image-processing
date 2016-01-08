cmake_minimum_required(VERSION 2.8.11)
project(Praktikum3)

#
# Environment variable QT4_QMAKE_DIR should be defined and point to the
# directory containing the qmake executable
#

set(CMAKE_PREFIX_PATH $ENV{QT4_QMAKE_DIR})
MESSAGE(STATUS "QT4_QMAKE_DIR: " ${CMAKE_PREFIX_PATH})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_AUTOMOC ON)

# Possibly set this to DEBUG for testing
set(CMAKE_BUILD_TYPE Release)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/src/gui)
include_directories(${PROJECT_SOURCE_DIR}/src/algorithms)

add_definitions(-DBUILD_QT)
find_package(Qt4 REQUIRED)
message("Qt4 Gui support activated.")
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})


find_package(OpenMP)
if (OPENMP_FOUND)
   message("OPENMP FOUND")
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
   set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

if(UNIX)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

add_executable(lab3
   src/algorithms/Algorithm.h

   src/algorithms/Greenify.cpp
   src/algorithms/Greenify.h

   src/algorithms/Lucy.cpp
   src/algorithms/Lucy.h
   src/algorithms/LucyOMP.cpp
   src/algorithms/LucyOMP.h
   src/algorithms/Cartoonize.cpp
   src/algorithms/Cartoonize.h
   src/algorithms/CartoonizeOMP.cpp
   src/algorithms/CartoonizeOMP.h

   src/gui/Callback.h
   src/gui/MainWindow.cpp
   src/gui/MainWindow.h
   src/algorithms/Parameters.cpp
   src/algorithms/Parameters.h
   src/algorithms/Image.cpp
   src/algorithms/Image.h
   src/main.cpp
)

set_property(TARGET lab3 PROPERTY CXX_STANDARD 11)
target_link_libraries(lab3 ${QT_LIBRARIES})
