cmake_minimum_required(VERSION 2.8)
project(QtBonjour)
find_package(Qt4 COMPONENTS QtCore QtNetwork REQUIRED)
include(${QT_USE_FILE})

if ("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
    add_custom_target(doc)
endif ()

subdirs(src doc)
