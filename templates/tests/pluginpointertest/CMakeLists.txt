cmake_minimum_required(VERSION 2.8.9)
project(sometest)

find_package(Qt5 REQUIRED Core)

set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR}/../../corelib )

set(myplugin_headers myobject.h)
set(myplugin_sources myobject.cpp)
set(myplugin_metadata myobject.json)

add_library(myplugin SHARED ${myplugin_sources} ${_plugin_moc_srcs})
set_target_properties(myplugin
  PROPERTIES PREFIX ""
)
qt5_use_modules(myplugin
  Core
)

add_executable(main_app main.cpp ${myplugin_sources} ${_plugin_moc_srcs})
qt5_use_modules(main_app
  Core
)
