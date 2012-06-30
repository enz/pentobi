include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
)

set(boardgame_test_main_STAT_SRCS
  Main.cpp
  )

add_library(boardgame_test_main STATIC ${boardgame_test_main_STAT_SRCS})
