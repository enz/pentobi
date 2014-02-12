include_directories(${CMAKE_SOURCE_DIR}/src)

add_library(boardgame_gtp STATIC
  Arguments.cpp
  Arguments.h
  CmdLine.cpp
  CmdLine.h
  CmdLineRange.cpp
  CmdLineRange.h
  Engine.cpp
  Engine.h
  Failure.h
  Response.cpp
  Response.h
)
