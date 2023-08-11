find_package(PCL 1.8 REQUIRED COMPONENTS common)

if(ROCK_QT_VERSION_4)
    rock_library(envire_visualizer_smurf_plugin
                 MOC     SmurfWidget.hpp
                 HEADERS SmurfItemFactory.h
                         SmurfWidget.hpp
                 SOURCES SmurfItemFactory.cpp
                 DEPS    Qt4::QtGui
                 DEPS_PKGCONFIG envire_core
                                envire_smurf
                                base-lib
    )
endif(ROCK_QT_VERSION_4)

if(ROCK_QT_VERSION_5)
    rock_library(envire_visualizer_smurf_plugin-qt5
                 MOC5    SmurfWidget.hpp
                 HEADERS SmurfItemFactory.h
                         SmurfWidget.hpp
                 SOURCES SmurfItemFactory.cpp
                 DEPS    Qt5::Widgets
                 DEPS_PKGCONFIG envire_core
                                envire_smurf
                                base-lib
    )
endif(ROCK_QT_VERSION_5)
