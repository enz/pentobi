add_library(boardgame_mcts INTERFACE)

target_sources(boardgame_mcts INTERFACE
    Atomic.h
    LastGoodReply.h
    Node.h
    PlayerMove.h
    SearchBase.h
    Tree.h
    TreeUtil.h
    )
