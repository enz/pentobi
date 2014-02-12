include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(unittest_libboardgame_mcts
  NodeTest.cpp
)

target_link_libraries(unittest_libboardgame_mcts
  boardgame_test_main
  boardgame_test
  boardgame_sgf
  boardgame_util
  boardgame_sys
  )

add_test(libboardgame_mcts unittest_libboardgame_mcts)
