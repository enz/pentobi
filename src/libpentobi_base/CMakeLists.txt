include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
)

set(pentobi_base_STAT_SRCS
  Board.cpp
  BoardConst.cpp
  BoardUpdater.cpp
  BoardUtil.cpp
  Book.cpp
  Color.cpp
  Engine.cpp
  Game.cpp
  Piece.cpp
  Player.cpp
  PieceTransforms.cpp
  PieceTransformsClassic.cpp
  PieceTransformsTrigon.cpp
  PieceValueHeuristic.cpp
  PointState.cpp
  PointStateExt.cpp
  SymmetricPoints.cpp
  Tree.cpp
  )

add_library(pentobi_base STATIC ${pentobi_base_STAT_SRCS})
