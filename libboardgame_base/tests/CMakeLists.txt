add_executable(unittest_libboardgame_base
    ArrayListTest.cpp
    MarkerTest.cpp
    OptionsTest.cpp
    PointTransformTest.cpp
    RatingTest.cpp
    RectGeometryTest.cpp
    StatisticsTest.cpp
    StringRepTest.cpp
    StringUtilTest.cpp
    )

target_link_libraries(unittest_libboardgame_base
    boardgame_test_main
    boardgame_base
    )

add_test(libboardgame_base unittest_libboardgame_base)
