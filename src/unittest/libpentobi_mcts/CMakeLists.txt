add_executable(unittest_libpentobi_mcts
  SearchTest.cpp
)

target_link_libraries(unittest_libpentobi_mcts
    boardgame_test_main
    pentobi_mcts
    )

add_test(libpentobi_mcts unittest_libpentobi_mcts)
