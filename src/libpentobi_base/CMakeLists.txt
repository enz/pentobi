include_directories(${CMAKE_SOURCE_DIR}/src)

set(pentobi_base_STAT_SRCS
  Board.cpp
  BoardConst.cpp
  BoardUpdater.cpp
  BoardUtil.cpp
  Book.cpp
  Color.cpp
  Game.cpp
  GameStateHistory.cpp
  NodeUtil.cpp
  PieceInfo.cpp
  Player.cpp
  PieceTransforms.cpp
  PieceTransformsClassic.cpp
  PieceTransformsTrigon.cpp
  PointState.cpp
  SgfUtil.cpp
  StartingPoints.cpp
  SymmetricPoints.cpp
  Tree.cpp
  TreeUtil.cpp
  TreeWriter.cpp
  Variant.cpp
  )

if (PENTOBI_BUILD_GTP)
  set(pentobi_base_STAT_SRCS ${pentobi_base_STAT_SRCS}
    Engine.cpp
    )
endif()

add_library(pentobi_base STATIC ${pentobi_base_STAT_SRCS})
