include_directories(${CMAKE_SOURCE_DIR}/src)

add_library(pentobi_mcts STATIC
  AnalyzeGame.h
  AnalyzeGame.cpp
  StateUtil.h
  StateUtil.cpp
  Float.h
  Player.h
  Player.cpp
  PlayoutFeatures.h
  PlayoutFeatures.cpp
  PriorKnowledge.h
  PriorKnowledge.cpp
  Search.h
  Search.cpp
  State.h
  State.cpp
  StateUtil.h
  StateUtil.cpp
  Util.h
  Util.cpp
)
