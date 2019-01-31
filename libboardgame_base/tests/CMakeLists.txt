add_executable(unittest_libboardgame_base
  MarkerTest.cpp
  PointTransformTest.cpp
  RatingTest.cpp
  RectGeometryTest.cpp
  StringRepTest.cpp
)

target_link_libraries(unittest_libboardgame_base
    boardgame_test_main
    boardgame_base
    )

add_test(libboardgame_base unittest_libboardgame_base)
