include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
)

link_directories(${Boost_LIBRARY_DIRS})

set(unittest_libpentobi_mcts_STAT_SRCS
  SearchTest.cpp
  )

add_executable(unittest_libpentobi_mcts
  ${unittest_libpentobi_mcts_STAT_SRCS})

target_link_libraries(unittest_libpentobi_mcts
  boardgame_test_main
  pentobi_mcts
  pentobi_base
  boardgame_mcts
  boardgame_base
  boardgame_sgf
  boardgame_test
  boardgame_util
  boardgame_sys
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_THREAD_LIBRARY}
  ${Boost_CHRONO_LIBRARY}
  ${CMAKE_THREAD_LIBS_INIT}
  )

add_test(libpentobi_mcts unittest_libpentobi_mcts)
