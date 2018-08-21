add_library(pentobi_mcts STATIC
  AnalyzeGame.h
  AnalyzeGame.cpp
  Float.h
  History.h
  History.cpp
  LocalPoints.h
  LocalPoints.cpp
  Player.h
  Player.cpp
  PlayoutFeatures.h
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

target_include_directories(pentobi_mcts PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/..")

target_link_libraries(pentobi_mcts pentobi_base boardgame_mcts)
