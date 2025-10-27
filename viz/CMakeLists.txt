
rock_qt_vizkit_plugin(
        TARGETPREFIX envire_visualizer-viz
        QT4_SUFFIX ""
        MISSINGQTDEPS_NOBUILD
        SOURCES EnvireVisualizerWidget.cpp
        DEPS_QT envire_visualizer
        DEPS_PKGCONFIG envire_core
        MOC EnvireVisualizerWidget.hpp
        HEADERS EnvireVisualizerWidget.hpp)

