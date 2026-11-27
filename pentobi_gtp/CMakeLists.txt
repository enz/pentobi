add_executable(pentobi-gtp
    GtpEngine.h
    GtpEngine.cpp
    Main.cpp
    )

target_compile_definitions(pentobi-gtp PRIVATE VERSION="${PENTOBI_VERSION}")

target_link_libraries(pentobi-gtp pentobi_gtp pentobi_mcts)

if(BUILD_TESTING)
    add_test(NAME pentobi_gtp_setboard
        COMMAND ${CMAKE_COMMAND} -DENGINE=$<TARGET_FILE:pentobi-gtp>
            -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/Setboard.cmake)
    add_test(NAME pentobi_gtp_setboard_variants
        COMMAND ${CMAKE_COMMAND} -DENGINE=$<TARGET_FILE:pentobi-gtp>
            -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/SetboardVariants.cmake)
endif()

