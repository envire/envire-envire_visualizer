find_package(PCL 1.8 REQUIRED COMPONENTS common)
IF("${PCL_VERSION_MAJOR}.${PCL_VERSION_MINOR}" VERSION_LESS 1.14)
    SET(PCL_VERSION_SUFFIX "-${PCL_VERSION_MAJOR}.${PCL_VERSION_MINOR}")
ENDIF()

rock_qt_library(
                TARGETPREFIX envire_visualizer_pcl_plugin
                QT4_SUFFIX ""
                MISSINGQTDEPS_NOBUILD
                MOC     PclWidget.hpp
                        PclManipulator.hpp
                HEADERS PclItemFactory.hpp
                        PclWidget.hpp
                        PclManipulator.hpp
                SOURCES PclItemFactory.cpp
                        PclManipulator.cpp
                DEPS_QT5    Qt5::Widgets
                DEPS_PKGCONFIG envire_core
                            envire_pcl
                            base-lib
                            pcl_common${PCL_VERSION_SUFFIX}
                            pcl_io${PCL_VERSION_SUFFIX}
                            pcl_registration${PCL_VERSION_SUFFIX}
)
