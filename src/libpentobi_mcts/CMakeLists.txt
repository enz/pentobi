include_directories(${CMAKE_SOURCE_DIR}/src)

add_library(pentobi_mcts STATIC
  AnalyzeGame.cpp
  AnalyzeGame.h
  BoardUtil.cpp
  BoardUtil.h
  Float.h
  LocalValue.cpp
  LocalValue.h
  Player.cpp
  Player.h
  PriorKnowledge.cpp
  PriorKnowledge.h
  Search.cpp
  Search.h
  State.cpp
  State.h
  Util.cpp
  Util.h
)
