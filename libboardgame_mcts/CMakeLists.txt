option(LIBBOARDGAME_MCTS_SINGLE_THREAD
    "Slightly faster MCTS search if only single-threaded search is used" OFF)

add_library(boardgame_mcts INTERFACE
    Atomic.h
    SearchBase.h
    LastGoodReply.h
    Tree.h
    Node.h
    TreeUtil.h
    PlayerMove.h
)

if(LIBBOARDGAME_MCTS_SINGLE_THREAD)
  target_compile_definitions(boardgame_mcts INTERFACE
      LIBBOARDGAME_MCTS_SINGLE_THREAD)
endif()

target_include_directories(boardgame_mcts INTERFACE ..)

target_link_libraries(boardgame_mcts INTERFACE Threads::Threads)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
