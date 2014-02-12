include_directories(${CMAKE_SOURCE_DIR}/src)

add_library(boardgame_sys STATIC
  Compiler.h
  CpuTime.cpp
  CpuTime.h
  Memory.cpp
  Memory.h
)