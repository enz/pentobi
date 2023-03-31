add_executable(test_libboardgame_gtp
  ArgumentsTest.cpp
  CmdLineTest.cpp
  GtpEngineTest.cpp
)

target_link_libraries(test_libboardgame_gtp
    boardgame_test_main
    boardgame_gtp
    )

add_test(libboardgame_gtp test_libboardgame_gtp)
