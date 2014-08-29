include_directories(${CMAKE_SOURCE_DIR}/src)

add_library(boardgame_base STATIC
  CoordPoint.h
  CoordPoint.cpp
  Engine.h
  Engine.cpp
  Geometry.h
  GeometryUtil.h
  Grid.h
  Marker.h
  Point.h
  PointList.h
  PointTransform.h
  Rating.h
  Rating.cpp
  RectGeometry.h
  SpShtStrRep.h
  SpShtStrRep.cpp
  Transform.h
  Transform.cpp
  TrigonGeometry.h
)

