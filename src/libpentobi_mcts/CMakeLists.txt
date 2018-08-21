set(LIBPENTOBI_MCTS_FLOAT_TYPE "float" CACHE STRING
    "Floating-point type for MCTS values")

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

if(NOT LIBPENTOBI_MCTS_FLOAT_TYPE STREQUAL "float")
  target_compile_definitions(pentobi_mcts PUBLIC
      "-DLIBPENTOBI_MCTS_FLOAT_TYPE=${LIBPENTOBI_MCTS_FLOAT_TYPE}")
endif()

target_include_directories(pentobi_mcts PUBLIC ..)

target_link_libraries(pentobi_mcts pentobi_base boardgame_mcts)
