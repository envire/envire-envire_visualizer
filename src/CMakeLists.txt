
include(RockQt)

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

rock_qt_library(
                TARGETPREFIX envire_visualizer
                QT4_SUFFIX ""
                MISSINGQTDEPS_NOBUILD
                MOC     ${ENVIRE_VISUALIZER_MOC}
                UI      ${ENVIRE_VISUALIZER_UI}
                HEADERS ${ENVIRE_VISUALIZER_HEADERS}
                SOURCES ${ENVIRE_VISUALIZER_SOURCES}
                DEPS_QT
                    envire_visualizer_pcl_plugin
                    envire_visualizer_smurf_plugin
                DEPS_PKGCONFIG_QT vizkit3d envire_2D_structure_widget vizkit3d_plugin_information
                DEPS_PKGCONFIG envire_core base-lib)

rock_qt_library(
                TARGETPREFIX envire_visualizer_interface
                QT4_SUFFIX ""
                MISSINGQTDEPS_NOBUILD
                HEADERS EnvireVisualizerInterface.hpp
                SOURCES EnvireVisualizerInterface.cpp
                LIBS_QT envire_visualizer)

rock_qt_executable(
                TARGET_QT4 envire_visualizer_bin
                TARGET_QT5 envire_visualizer-qt5_bin
                QT4_SUFFIX ""
                MISSINGQTDEPS_NOBUILD
                SOURCES VisualizerMain.cpp
                DEPS_QT envire_visualizer
)
