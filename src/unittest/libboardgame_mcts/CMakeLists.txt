include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
)

link_directories(${Boost_LIBRARY_DIRS})

set(unittest_libboardgame_mcts_STAT_SRCS
  NodeTest.cpp
  )

add_executable(unittest_libboardgame_mcts
  ${unittest_libboardgame_mcts_STAT_SRCS})

target_link_libraries(unittest_libboardgame_mcts
  boardgame_test_main
  boardgame_test
  boardgame_sgf
  boardgame_util
  boardgame_sys
  )

add_test(libboardgame_mcts unittest_libboardgame_mcts)
