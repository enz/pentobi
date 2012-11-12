include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
)

set(boardgame_mcts_STAT_SRCS
  BiasTerm.cpp
  )

add_library(boardgame_mcts STATIC ${boardgame_mcts_STAT_SRCS})
