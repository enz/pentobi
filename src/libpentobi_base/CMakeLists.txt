set(pentobi_base_SRCS
  BoardConst.h
  BoardConst.cpp
  Board.h
  Board.cpp
  BoardUpdater.h
  BoardUpdater.cpp
  BoardUtil.h
  BoardUtil.cpp
  Book.h
  Book.cpp
  CallistoGeometry.h
  CallistoGeometry.cpp
  Color.h
  Color.cpp
  ColorMap.h
  ColorMove.h
  Game.h
  Game.cpp
  GembloQGeometry.h
  GembloQGeometry.cpp
  GembloQTransform.h
  GembloQTransform.cpp
  Geometry.h
  Grid.h
  Marker.h
  Move.h
  MoveInfo.h
  MoveList.h
  MoveMarker.h
  MovePoints.h
  NexosGeometry.h
  NexosGeometry.cpp
  NodeUtil.h
  NodeUtil.cpp
  PentobiSgfUtil.h
  PentobiSgfUtil.cpp
  PentobiTree.h
  PentobiTree.cpp
  PentobiTreeWriter.h
  PentobiTreeWriter.cpp
  Piece.h
  PieceInfo.h
  PieceInfo.cpp
  PieceMap.h
  PieceTransformsClassic.h
  PieceTransformsClassic.cpp
  PieceTransformsGembloQ.h
  PieceTransformsGembloQ.cpp
  PieceTransforms.h
  PieceTransforms.cpp
  PieceTransformsTrigon.h
  PieceTransformsTrigon.cpp
  PlayerBase.h
  PlayerBase.cpp
  Point.h
  PointList.h
  PointState.h
  PointState.cpp
  PrecompMoves.h
  ScoreUtil.h
  Setup.h
  StartingPoints.h
  StartingPoints.cpp
  SymmetricPoints.h
  SymmetricPoints.cpp
  TreeUtil.h
  TreeUtil.cpp
  TrigonGeometry.h
  TrigonGeometry.cpp
  TrigonTransform.h
  TrigonTransform.cpp
  Variant.h
  Variant.cpp
)

if (PENTOBI_BUILD_GTP)
  set(pentobi_base_SRCS ${pentobi_base_SRCS}
    Engine.cpp
    Engine.h
  )
endif()

add_library(pentobi_base STATIC ${pentobi_base_SRCS})

target_include_directories(pentobi_base PUBLIC ..)

target_link_libraries(pentobi_base boardgame_sgf boardgame_base)
