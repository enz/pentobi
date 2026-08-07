add_library(boardgame_mcts INTERFACE
    SearchBase.h
    LastGoodReply.h
    Tree.h
    Node.h
    TreeUtil.h
    PlayerMove.h
)

target_include_directories(boardgame_mcts INTERFACE ..)

target_link_libraries(boardgame_mcts INTERFACE Threads::Threads)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
