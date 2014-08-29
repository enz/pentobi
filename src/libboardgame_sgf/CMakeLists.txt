include_directories(${CMAKE_SOURCE_DIR}/src)

add_library(boardgame_sgf STATIC
  InvalidPropertyValue.h
  InvalidTree.h
  MissingProperty.h
  MissingProperty.cpp
  Node.h
  Node.cpp
  Reader.h
  Reader.cpp
  Tree.h
  Tree.cpp
  TreeIterator.h
  TreeIterator.cpp
  TreeReader.h
  TreeReader.cpp
  TreeWriter.h
  TreeWriter.cpp
  Util.h
  Util.cpp
  Writer.h
  Writer.cpp
)
