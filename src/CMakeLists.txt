
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

find_path(BONJOUR_INCLUDE_DIR
    NAMES dns_sd.h
    PATHS
    "C:/Program Files/Bonjour SDK/Include"
    )

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${BONJOUR_INCLUDE_DIR}
    )

qt4_automoc(${sources})

add_library(QtBonjour ${sources})
