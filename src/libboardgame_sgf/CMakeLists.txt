include_directories(${CMAKE_SOURCE_DIR}/src)

set(boardgame_sgf_STAT_SRCS
  InvalidPropertyValue.cpp
  InvalidTree.cpp
  MissingProperty.cpp
  Node.cpp
  Reader.cpp
  Tree.cpp
  TreeIterator.cpp
  TreeReader.cpp
  TreeWriter.cpp
  Util.cpp
  Writer.cpp
  )

add_library(boardgame_sgf STATIC ${boardgame_sgf_STAT_SRCS})
