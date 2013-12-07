include_directories(${CMAKE_SOURCE_DIR}/src)

set(boardgame_gtp_STAT_SRCS
  Arguments.cpp
  CmdLine.cpp
  CmdLineRange.cpp
  Engine.cpp
  Response.cpp
  )

add_library(boardgame_gtp STATIC ${boardgame_gtp_STAT_SRCS})
