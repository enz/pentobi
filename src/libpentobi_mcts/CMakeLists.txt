include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
)

set(pentobi_mcts_STAT_SRCS
  AnalyzeGame.cpp
  BookBuilder.cpp
  Player.cpp
  Search.cpp
  State.cpp
  Util.cpp
  )

add_library(pentobi_mcts STATIC ${pentobi_mcts_STAT_SRCS})
