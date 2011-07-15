include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
)

set(boardgame_sys_STAT_SRCS
  CpuTime.cpp
  )

add_library(boardgame_sys STATIC ${boardgame_sys_STAT_SRCS})
