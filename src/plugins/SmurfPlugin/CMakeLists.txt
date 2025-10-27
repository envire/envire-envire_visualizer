rock_qt_library(
                TARGETPREFIX envire_visualizer_smurf_plugin
                QT4_SUFFIX ""
                MISSINGQTDEPS_NOBUILD
                MOC     SmurfWidget.hpp
                HEADERS SmurfItemFactory.h
                        SmurfWidget.hpp
                SOURCES SmurfItemFactory.cpp
                DEPS_QT5    Qt5::Widgets
                DEPS_PKGCONFIG envire_core
                            envire_smurf
                            base-lib
)

