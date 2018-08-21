find_package(Threads)

add_library(boardgame_mcts INTERFACE)

target_include_directories(boardgame_mcts INTERFACE ..)

target_link_libraries(boardgame_mcts INTERFACE
    boardgame_util
    Threads::Threads
    )
