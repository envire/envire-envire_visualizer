cmake_minimum_required(VERSION 2.6)
set(ROCK_USE_CXX11 TRUE)
find_package(Rock)

rock_init(envire_visualizer 0.1)
rock_find_qt4()
rock_standard_layout()
