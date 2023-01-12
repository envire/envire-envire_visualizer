find_package(PCL 1.8 REQUIRED COMPONENTS common)
rock_find_qt5(Core Gui Widgets)

rock_library(envire_visualizer_pcl_plugin 
             MOC     PclWidget.hpp
                     PclManipulator.hpp
             HEADERS PclItemFactory.hpp
                     PclWidget.hpp
                     PclManipulator.hpp
             SOURCES PclItemFactory.cpp
                     PclManipulator.cpp
             LIBS    Qt5::Core
                     Qt5::Widgets
             DEPS_PKGCONFIG envire_core
                            envire_pcl
                            base-lib
                            pcl_common-${PCL_VERSION_MAJOR}.${PCL_VERSION_MINOR}
                            pcl_io-${PCL_VERSION_MAJOR}.${PCL_VERSION_MINOR}
                            pcl_registration-${PCL_VERSION_MAJOR}.${PCL_VERSION_MINOR}
)
