find_package(Threads)

add_executable(learn-tool Main.cpp)

target_link_libraries(learn-tool
  pentobi_mcts
  pentobi_base
  boardgame_base
  boardgame_sgf
  boardgame_util
  Threads::Threads
)
