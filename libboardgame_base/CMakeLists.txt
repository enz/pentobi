add_library(boardgame_base STATIC
    ArrayList.h
    Assert.h
    Assert.cpp
    Barrier.h
    Barrier.cpp
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
    RectGeometry.h
    RectTransform.h
    RectTransform.cpp
    Statistics.h
    StringRep.h
    StringRep.cpp
    StringUtil.h
    StringUtil.cpp
    TimeIntervalChecker.h
    TimeIntervalChecker.cpp
    Timer.h
    Timer.cpp
    TimeSource.h
    TimeSource.cpp
    Transform.h
    Transform.cpp
    WallTimeSource.h
    WallTimeSource.cpp
    )

if(PENTOBI_BUILD_GTP)
    target_sources(boardgame_base PRIVATE
        Engine.cpp
        Engine.h
        )
    target_link_libraries(boardgame_base boardgame_gtp)
endif()

target_compile_options(boardgame_base PUBLIC
    "$<$<CONFIG:DEBUG>:-DLIBBOARDGAME_DEBUG>")

target_include_directories(boardgame_base PUBLIC ..)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
