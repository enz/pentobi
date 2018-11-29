add_library(boardgame_sys STATIC
  Compiler.h
  CpuTime.h
  CpuTime.cpp
  Memory.h
  Memory.cpp
)

target_include_directories(boardgame_sys PUBLIC ..)
