include_directories(${CMAKE_SOURCE_DIR}/src)

add_library(boardgame_gtp STATIC
  Arguments.h
  Arguments.cpp
  CmdLine.h
  CmdLine.cpp
  CmdLineRange.h
  CmdLineRange.cpp
  Engine.h
  Engine.cpp
  Failure.h
  Response.h
  Response.cpp
)
