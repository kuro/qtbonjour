
find_package(Doxygen)

configure_file(
    bonjour.dox.in
    ${CMAKE_CURRENT_BINARY_DIR}/bonjour.dox
    )

if (DOXYGEN_FOUND)
    add_custom_target(
        bonjour-doc
        ${DOXYGEN_EXECUTABLE} bonjour.dox
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
    add_dependencies(doc bonjour-doc)
endif()
