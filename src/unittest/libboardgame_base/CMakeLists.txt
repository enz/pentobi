include_directories(${CMAKE_SOURCE_DIR}/src)

set(unittest_libboardgame_base_STAT_SRCS
  MarkerTest.cpp
  PointTest.cpp
  PointTransformTest.cpp
  RatingTest.cpp
  RectGeometryTest.cpp
  SpShtStrRepTest.cpp
  )

add_executable(unittest_libboardgame_base
  ${unittest_libboardgame_base_STAT_SRCS})

target_link_libraries(unittest_libboardgame_base
  boardgame_test_main
  boardgame_base
  boardgame_test
  boardgame_util
  boardgame_sys
  )

add_test(libboardgame_base unittest_libboardgame_base)
