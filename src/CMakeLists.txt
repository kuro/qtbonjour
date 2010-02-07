
set(sources
    BonjourRecord.h
    BonjourRecord.cpp
    BonjourBrowser.h
    BonjourBrowser.cpp
    BonjourResolver.h
    BonjourResolver.cpp
    )

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    )

qt4_automoc(${sources})

add_library(salut ${sources})
