include(CheckIncludeFiles)

add_library(boardgame_sys STATIC
  Compiler.h
  CpuTime.h
  CpuTime.cpp
  Memory.h
  Memory.cpp
)

check_include_files(sys/sysctl.h HAVE_SYS_SYSCTL_H)
if(HAVE_UNISTD_H)
    target_compile_definitions(boardgame_sys PRIVATE HAVE_UNISTD_H)
endif()
check_include_files(sys/times.h HAVE_SYS_TIMES_H)
if(HAVE_SYS_TIMES_H)
    target_compile_definitions(boardgame_sys PRIVATE HAVE_SYS_TIMES_H)
endif()
check_include_files(unistd.h HAVE_UNISTD_H)
if(HAVE_UNISTD_H)
    target_compile_definitions(boardgame_sys PRIVATE HAVE_UNISTD_H)
endif()

target_include_directories(boardgame_sys PUBLIC ..)
