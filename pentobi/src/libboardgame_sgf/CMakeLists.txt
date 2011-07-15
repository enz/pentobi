include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
)

set(boardgame_sgf_STAT_SRCS
  InvalidPropertyValue.cpp
  MissingProperty.cpp
  Node.cpp
  Reader.cpp
  Tree.cpp
  TreeReader.cpp
  Util.cpp
  Writer.cpp
  )

add_library(boardgame_sgf STATIC ${boardgame_sgf_STAT_SRCS})
