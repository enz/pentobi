include_directories(${CMAKE_SOURCE_DIR}/src)

add_library(boardgame_base STATIC
  CoordPoint.cpp
  CoordPoint.h
  Engine.cpp
  Engine.h
  Geometry.h
  GeometryUtil.h
  Grid.h
  Marker.h
  MarkerWithCount.h
  Point.h
  PointList.h
  PointTransform.h
  Rating.cpp
  Rating.h
  RectGeometry.h
  SpShtStrRep.cpp
  SpShtStrRep.h
  Transform.cpp
  Transform.h
  TrigonGeometry.h
)
  
