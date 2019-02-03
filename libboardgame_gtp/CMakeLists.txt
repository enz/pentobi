add_library(boardgame_gtp STATIC
  Arguments.h
  Arguments.cpp
  CmdLine.h
  CmdLine.cpp
  Engine.h
  Engine.cpp
  Failure.h
  Response.h
  Response.cpp
)

target_include_directories(boardgame_gtp PUBLIC ..)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
