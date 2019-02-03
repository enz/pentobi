add_library(boardgame_base STATIC
    CoordPoint.h
    CoordPoint.cpp
    Geometry.h
    GeometryUtil.h
    Grid.h
    Marker.h
    Point.h
    PointTransform.h
    Rating.h
    Rating.cpp
    RectGeometry.h
    RectTransform.h
    RectTransform.cpp
    StringRep.h
    StringRep.cpp
    Transform.h
    Transform.cpp
    )
target_link_libraries(boardgame_base boardgame_util)

if(PENTOBI_BUILD_GTP)
    target_sources(boardgame_base PRIVATE
        Engine.cpp
        Engine.h
        )
    target_link_libraries(boardgame_base boardgame_gtp)
endif()

target_include_directories(boardgame_base PUBLIC ..)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
