include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
)

set(boardgame_base_STAT_SRCS
  CoordPoint.cpp
  Engine.cpp
  Rating.cpp
  SpreadsheetStringRep.cpp
  Transform.cpp
  )

add_library(boardgame_base STATIC ${boardgame_base_STAT_SRCS})
