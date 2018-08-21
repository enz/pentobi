option(LIBBOARDGAME_MCTS_SINGLE_THREAD
    "Slightly faster MCTS search if only single-threaded search is used" OFF)

find_package(Threads)

add_library(boardgame_mcts INTERFACE)

if(LIBBOARDGAME_MCTS_SINGLE_THREAD)
  target_compile_definitions(boardgame_mcts INTERFACE
      "-DLIBBOARDGAME_MCTS_SINGLE_THREAD")
endif()

target_include_directories(boardgame_mcts INTERFACE ..)

target_link_libraries(boardgame_mcts INTERFACE
    boardgame_util
    Threads::Threads
    )
