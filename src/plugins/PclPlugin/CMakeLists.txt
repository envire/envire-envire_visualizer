
rock_library(envire_visualizer_pcl_plugin 
             MOC     PclWidget.hpp
                     PclManipulator.hpp
             HEADERS PclItemFactory.hpp
                     PclWidget.hpp
                     PclManipulator.hpp
             SOURCES PclItemFactory.cpp
                     PclManipulator.cpp
             DEPS_PKGCONFIG envire_core
                            envire_pcl
                            base-lib
                            pcl_common-1.7
                            pcl_io-1.7
                            pcl_registration-1.7)    
