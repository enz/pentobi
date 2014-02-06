include_directories(${CMAKE_SOURCE_DIR}/src)

set(unittest_libboardgame_gtp_STAT_SRCS
  ArgumentsTest.cpp
  CmdLineTest.cpp
  EngineTest.cpp
  ResponseTest.cpp
  )

add_executable(unittest_libboardgame_gtp
  ${unittest_libboardgame_gtp_STAT_SRCS})

target_link_libraries(unittest_libboardgame_gtp
  boardgame_test_main
  boardgame_test
  boardgame_util
  boardgame_gtp
  )

add_test(libboardgame_gtp unittest_libboardgame_gtp)
