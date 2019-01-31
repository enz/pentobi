add_executable(unittest_libboardgame_gtp
  ArgumentsTest.cpp
  CmdLineTest.cpp
  EngineTest.cpp
  ResponseTest.cpp
)

target_link_libraries(unittest_libboardgame_gtp
    boardgame_test_main
    boardgame_gtp
    )

add_test(libboardgame_gtp unittest_libboardgame_gtp)
