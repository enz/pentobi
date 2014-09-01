include_directories(${CMAKE_SOURCE_DIR}/src)

add_library(boardgame_sgf STATIC
  InvalidPropertyValue.h
  InvalidTree.h
  MissingProperty.h
  MissingProperty.cpp
  Reader.h
  Reader.cpp
  SgfNode.h
  SgfNode.cpp
  SgfTree.h
  SgfTree.cpp
  SgfUtil.h
  SgfUtil.cpp
  TreeIterator.h
  TreeIterator.cpp
  TreeReader.h
  TreeReader.cpp
  TreeWriter.h
  TreeWriter.cpp
  Writer.h
  Writer.cpp
)
