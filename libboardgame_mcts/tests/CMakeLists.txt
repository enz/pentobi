add_executable(test_libboardgame_mcts
  NodeTest.cpp
)

target_link_libraries(test_libboardgame_mcts
    boardgame_test_main
    boardgame_mcts
    )

add_test(libboardgame_mcts test_libboardgame_mcts)
