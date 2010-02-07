cmake_minimum_required(VERSION 2.8)
find_package(Qt4 COMPONENTS QtCore QtNetwork REQUIRED)
include(${QT_USE_FILE})
subdirs(src)
