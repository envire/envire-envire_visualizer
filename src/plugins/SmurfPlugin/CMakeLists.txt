find_package(PCL 1.7 REQUIRED COMPONENTS common)

rock_library(envire_visualizer_smurf_plugin 
             MOC     SmurfWidget.hpp
             HEADERS SmurfItemFactory.h
                     SmurfWidget.hpp
             SOURCES SmurfItemFactory.cpp
             DEPS_PKGCONFIG envire_core
                            envire_smurf
                            base-lib
)
