add_executable(pentobi-gtp
  Engine.h
  Engine.cpp
  Main.cpp
)

target_link_libraries(pentobi-gtp pentobi_mcts)

