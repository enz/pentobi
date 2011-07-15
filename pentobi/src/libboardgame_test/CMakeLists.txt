include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
)

set(boardgame_test_STAT_SRCS
  Test.cpp
  )

add_library(boardgame_test STATIC ${boardgame_test_STAT_SRCS})
