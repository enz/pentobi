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

if(PENTOBI_BUILD_TESTS)
    add_subdirectory(tests)
endif()
