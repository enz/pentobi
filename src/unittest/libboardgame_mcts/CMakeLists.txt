add_executable(unittest_libboardgame_mcts
  NodeTest.cpp
)

target_link_libraries(unittest_libboardgame_mcts
    boardgame_test_main
    boardgame_mcts
    )

add_test(libboardgame_mcts unittest_libboardgame_mcts)
