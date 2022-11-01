cmake_minimum_required(VERSION 3.3)

find_package(Rock)

# Default to C++14 (PCL requirement)
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

rock_init(envire_visualizer 0.1)
rock_find_qt5(Core Gui Widgets)
find_package(Boost COMPONENTS system thread serialization iostreams date_time filesystem REQUIRED)
rock_standard_layout()
