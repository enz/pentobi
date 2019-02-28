add_executable(test_libboardgame_base
    ArrayListTest.cpp
    MarkerTest.cpp
    OptionsTest.cpp
    PointTransformTest.cpp
    RatingTest.cpp
    RectGeometryTest.cpp
    SgfNodeTest.cpp
    SgfTreeTest.cpp
    SgfUtilTest.cpp
    StatisticsTest.cpp
    StringRepTest.cpp
    StringUtilTest.cpp
    TreeReaderTest.cpp
    )

target_link_libraries(test_libboardgame_base
    boardgame_test_main
    boardgame_base
    )

add_test(libboardgame_base test_libboardgame_base)
