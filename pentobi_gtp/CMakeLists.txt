add_executable(pentobi-gtp
    GtpEngine.h
    GtpEngine.cpp
    Main.cpp
    )

target_compile_definitions(pentobi-gtp PRIVATE VERSION="${PENTOBI_VERSION}")

target_link_libraries(pentobi-gtp pentobi_gtp pentobi_mcts)

