include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
)

link_directories(${Boost_LIBRARY_DIRS})

set(unittest_libboardgame_base_STAT_SRCS
  MarkerTest.cpp
  PointTest.cpp
  PointTransformTest.cpp
  RatingTest.cpp
  RectGeometryTest.cpp
  SpreadsheetStringRepTest.cpp
  )

add_executable(unittest_libboardgame_base
  ${unittest_libboardgame_base_STAT_SRCS})

target_link_libraries(unittest_libboardgame_base
  boardgame_test_main
  boardgame_base
  boardgame_test
  boardgame_util
  boardgame_sys
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY}
  )

add_test(libboardgame_base unittest_libboardgame_base)
