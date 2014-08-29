include_directories(${CMAKE_SOURCE_DIR}/src)

add_library(pentobi_mcts STATIC
  AnalyzeGame.h
  AnalyzeGame.cpp
  BoardUtil.h
  BoardUtil.cpp
  Float.h
  LocalValue.h
  LocalValue.cpp
  Player.h
  Player.cpp
  PriorKnowledge.h
  PriorKnowledge.cpp
  Search.h
  Search.cpp
  State.h
  State.cpp
  Util.h
  Util.cpp
)
