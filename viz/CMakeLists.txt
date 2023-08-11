
if(ROCK_QT_VERSION_4)
    rock_vizkit_plugin(envire_visualizer-viz
        EnvireVisualizerWidget.cpp
        DEPS envire_visualizer
        DEPS_PKGCONFIG envire_core
        MOC EnvireVisualizerWidget.hpp
        HEADERS EnvireVisualizerWidget.hpp)
endif()

if(ROCK_QT_VERSION_5)
    rock_vizkit_plugin_qt5(envire_visualizer-viz-qt5
        EnvireVisualizerWidget.cpp
        DEPS envire_visualizer-qt5
        DEPS_PKGCONFIG envire_core
        MOC5 EnvireVisualizerWidget.hpp
        HEADERS EnvireVisualizerWidget.hpp)
endif()
