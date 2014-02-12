include_directories(${CMAKE_SOURCE_DIR}/src)

add_library(boardgame_sgf STATIC
  InvalidPropertyValue.cpp
  InvalidPropertyValue.h
  InvalidTree.cpp
  InvalidTree.h
  MissingProperty.cpp
  MissingProperty.h
  Node.cpp
  Node.h
  Reader.cpp
  Reader.h
  Tree.cpp
  Tree.h
  TreeIterator.cpp
  TreeIterator.h
  TreeReader.cpp
  TreeReader.h
  TreeWriter.cpp
  TreeWriter.h
  Util.cpp
  Util.h
  Writer.cpp
  Writer.h
)