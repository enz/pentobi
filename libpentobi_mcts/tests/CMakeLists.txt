add_executable(test_libpentobi_mcts
  SearchTest.cpp
)

target_link_libraries(test_libpentobi_mcts
    boardgame_test_main
    pentobi_mcts
    )

add_test(libpentobi_mcts test_libpentobi_mcts)
