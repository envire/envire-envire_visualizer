cmake_minimum_required(VERSION 3.0)
project(envire_visualizer VERSION 0.1)

find_package(Rock)

# Default to C++14 (PCL requirement)
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

rock_init()
rock_feature(NOCURDIR)
rock_find_qt4(OPTIONAL)
rock_find_qt5(OPTIONAL)
rock_standard_layout()
