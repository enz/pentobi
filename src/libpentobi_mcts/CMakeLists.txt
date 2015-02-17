include_directories(${CMAKE_SOURCE_DIR}/src)

add_library(pentobi_mcts STATIC
  AnalyzeGame.h
  AnalyzeGame.cpp
  Float.h
  History.h
  History.cpp
  Player.h
  Player.cpp
  PlayoutFeatures.h
  PlayoutFeatures.cpp
  PriorKnowledge.h
  PriorKnowledge.cpp
  SearchParamConst.h
  SharedConst.h
  SharedConst.cpp
  Search.h
  Search.cpp
  State.h
  State.cpp
  StateUtil.h
  StateUtil.cpp
  Util.h
  Util.cpp
)
