
set(headers
    BonjourRecord.h
    BonjourBrowser.h
    BonjourResolver.h
    BonjourRegistrar.h
    )

set(sources
    BonjourRecord.cpp
    BonjourBrowser.cpp
    BonjourResolver.cpp
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

install(FILES ${headers}
    DESTINATION include/QtBonjour
    )

install(TARGETS QtBonjour
    DESTINATION lib
    )
