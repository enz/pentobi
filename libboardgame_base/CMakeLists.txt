add_library(boardgame_base STATIC
    ArrayList.h
    Assert.h
    Assert.cpp
    Compiler.h
    CoordPoint.h
    CoordPoint.cpp
    CpuTime.h
    CpuTime.cpp
    CpuTimeSource.h
    CpuTimeSource.cpp
    FmtSaver.h
    Geometry.h
    GeometryUtil.h
    Grid.h
    IntervalChecker.h
    IntervalChecker.cpp
    Log.h
    Log.cpp
    Marker.h
    MathUtil.h
    Memory.h
    Memory.cpp
    Options.h
    Options.cpp
    Point.h
    PointTransform.h
    RandomGenerator.h
    RandomGenerator.cpp
    Range.h
    Rating.h
    Rating.cpp
    Reader.h
    Reader.cpp
    RectGeometry.h
    RectTransform.h
    RectTransform.cpp
    SgfError.h
    SgfError.cpp
    SgfNode.h
    SgfNode.cpp
    SgfTree.h
    SgfTree.cpp
    SgfUtil.h
    SgfUtil.cpp
    Statistics.h
    StringRep.h
    StringRep.cpp
    StringUtil.h
    StringUtil.cpp
    Timer.h
    Timer.cpp
    TimeSource.h
    TimeSource.cpp
    Transform.h
    Transform.cpp
    TreeReader.h
    TreeReader.cpp
    TreeWriter.h
    TreeWriter.cpp
    WallTimeSource.h
    WallTimeSource.cpp
    Writer.h
    Writer.cpp
    )

target_compile_options(boardgame_base PUBLIC
    "$<$<CONFIG:DEBUG>:-DLIBBOARDGAME_DEBUG>")

if(ANDROID)
    target_link_libraries(boardgame_base INTERFACE debug log)
endif()

target_include_directories(boardgame_base PUBLIC ..)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
