add_subdirectory(plugins)
include_directories(${CMAKE_CURRENT_BINARY_DIR}) #to be able to include ui_mainwindow.h

set(ENVIRE_VISUALIZER_MOC
                     EnvireVisualizerWindow.hpp
                     EnvireGraphVisualizer.hpp
                     TransformModel.hpp
                     DoubleSpinboxItemDelegate.hpp
                     AddTransformDialog.hpp
                     AddItemDialog.hpp
                     ItemTableModel.hpp
                     ItemManipulatorWidget.hpp
    )
set(ENVIRE_VISUALIZER_UI
                     mainwindow.ui
                     addtransformdialog.ui
                     additemdialog.ui
    )
set(ENVIRE_VISUALIZER_HEADERS
                     Helpers.hpp
                     EnvireVisualizerWindow.hpp
                     EnvireGraphVisualizer.hpp
                     TransformModel.hpp
                     DoubleSpinboxItemDelegate.hpp
                     AddTransformDialog.hpp
                     AddItemDialog.hpp
                     ItemTableModel.hpp
                     plugins/ItemFactoryInterface.hpp
                     ItemManipulatorWidget.hpp
    )
set(ENVIRE_VISUALIZER_SOURCES
                     EnvireGraphVisualizer.cpp
                     TransformModel.cpp
                     DoubleSpinboxItemDelegate.cpp
                     AddTransformDialog.cpp
                     AddItemDialog.cpp
                     ItemTableModel.cpp
                     EnvireVisualizerWindow.cpp
                     ItemManipulatorWidget.cpp
    )

if(ROCK_QT_VERSION_4)
    rock_library(envire_visualizer
                 MOC     ${ENVIRE_VISUALIZER_MOC}
                 UI      ${ENVIRE_VISUALIZER_UI}
                 HEADERS ${ENVIRE_VISUALIZER_HEADERS}
                 SOURCES ${ENVIRE_VISUALIZER_SOURCES}
                 DEPS envire_visualizer_pcl_plugin
                      envire_visualizer_smurf_plugin
                      Qt4::QtCore
                 DEPS_PKGCONFIG vizkit3d envire_core envire_2D_structure_widget vizkit3d_plugin_information
                 DEPS_PKGCONFIG base-lib)

    rock_library(envire_visualizer_interface
                 HEADERS EnvireVisualizerInterface.hpp
                 SOURCES EnvireVisualizerInterface.cpp
                 LIBS envire_visualizer)

    rock_executable(envire_visualizer_bin
                    SOURCES VisualizerMain.cpp
                    DEPS envire_visualizer
    )
endif(ROCK_QT_VERSION_4)

if(ROCK_QT_VERSION_5)
    rock_library(envire_visualizer-qt5
                 MOC5    ${ENVIRE_VISUALIZER_MOC}
                 UI5     ${ENVIRE_VISUALIZER_UI}
                 HEADERS ${ENVIRE_VISUALIZER_HEADERS}
                 SOURCES ${ENVIRE_VISUALIZER_SOURCES}
                 DEPS envire_visualizer_pcl_plugin-qt5
                      envire_visualizer_smurf_plugin-qt5
                      Qt5::Core Qt5::Gui
                 DEPS_PKGCONFIG vizkit3d-qt5 envire_core envire_2D_structure_widget-qt5 vizkit3d_plugin_information
                 DEPS_PKGCONFIG base-lib)

    rock_library(envire_visualizer_interface-qt5
                 HEADERS EnvireVisualizerInterface.hpp
                 SOURCES EnvireVisualizerInterface.cpp
                 LIBS envire_visualizer-qt5)

    rock_executable(envire_visualizer-qt5_bin
                    SOURCES VisualizerMain.cpp
                    DEPS envire_visualizer-qt5
    )
endif(ROCK_QT_VERSION_5)
