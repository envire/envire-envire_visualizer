rock_library(envire_visualizer_pcl_plugin 
           #  MOC     EnvireVisualizerWindow.hpp
             HEADERS PclItemFactory.hpp
             SOURCES PclItemFactory.cpp
             DEPS envire_visualizer 
             DEPS_PKGCONFIG envire_core base-lib)    
