include_directories(${CMAKE_SOURCE_DIR}/src)

set(boardgame_base_STAT_SRCS
  CoordPoint.cpp
  Engine.cpp
  Rating.cpp
  SpShtStrRep.cpp
  Transform.cpp
  )

add_library(boardgame_base STATIC ${boardgame_base_STAT_SRCS})
