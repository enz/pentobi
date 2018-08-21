add_library(boardgame_sgf STATIC
  Reader.h
  Reader.cpp
  SgfError.h
  SgfError.cpp
  SgfNode.h
  SgfNode.cpp
  SgfTree.h
  SgfTree.cpp
  SgfUtil.h
  SgfUtil.cpp
  TreeReader.h
  TreeReader.cpp
  TreeWriter.h
  TreeWriter.cpp
  Writer.h
  Writer.cpp
)

target_include_directories(boardgame_sgf PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/..")

target_link_libraries(boardgame_sgf boardgame_util)
