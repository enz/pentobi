include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
)

set(pentobi_mcts_STAT_SRCS
  BookBuilder.cpp
  Player.cpp
  Search.cpp
  State.cpp
  )

add_library(pentobi_mcts STATIC ${pentobi_mcts_STAT_SRCS})
