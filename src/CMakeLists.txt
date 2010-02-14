
set(sources
    BonjourRecord.h
    BonjourRecord.cpp
    BonjourBrowser.h
    BonjourBrowser.cpp
    BonjourResolver.h
    BonjourResolver.cpp
    BonjourRegistrar.h
    BonjourRegistrar.cpp
    )

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    )

qt4_automoc(${sources})

add_library(QtBonjour ${sources})
