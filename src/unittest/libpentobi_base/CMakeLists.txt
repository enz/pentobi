include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
)

link_directories(${Boost_LIBRARY_DIRS})

set(unittest_libpentobi_base_STAT_SRCS
  Main.cpp
  BoardUpdaterTest.cpp
  GameTest.cpp
  TreeTest.cpp
  )

add_executable(unittest_libpentobi_base
  ${unittest_libpentobi_base_STAT_SRCS})

target_link_libraries(unittest_libpentobi_base
  pentobi_base
  boardgame_base
  boardgame_sgf
  boardgame_test
  boardgame_util
  boardgame_sys
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY}
  )

add_test(libpentobi_base unittest_libpentobi_base)
