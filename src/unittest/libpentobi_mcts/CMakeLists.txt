include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(unittest_libpentobi_mcts
  SearchTest.cpp
)

target_link_libraries(unittest_libpentobi_mcts
  boardgame_test_main
  pentobi_mcts
  pentobi_base
  boardgame_base
  boardgame_sgf
  boardgame_test
  boardgame_util
  boardgame_sys
  )

if (DEFINED CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(unittest_libpentobi_mcts ${CMAKE_THREAD_LIBS_INIT})
endif()

add_test(libpentobi_mcts unittest_libpentobi_mcts)
