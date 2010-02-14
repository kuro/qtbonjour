
find_package(Doxygen)

configure_file(
    qtbonjour.dox.in
    ${CMAKE_CURRENT_BINARY_DIR}/qtbonjour.dox
    )

if (DOXYGEN_FOUND)
    add_custom_target(
        qtbonjour-doc
        ${DOXYGEN_EXECUTABLE} qtbonjour.dox
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
    add_dependencies(doc qtbonjour-doc)
endif()
