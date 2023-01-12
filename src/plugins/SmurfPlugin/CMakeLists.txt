find_package(PCL 1.8 REQUIRED COMPONENTS common)

rock_library(envire_visualizer_smurf_plugin 
             MOC     SmurfWidget.hpp
             HEADERS SmurfItemFactory.h
                     SmurfWidget.hpp
             SOURCES SmurfItemFactory.cpp
             LIBS    Qt5::Core
                     Qt5::Widgets
             DEPS_PKGCONFIG envire_core
                            envire_smurf
                            base-lib
)
