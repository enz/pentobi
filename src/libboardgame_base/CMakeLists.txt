set(boardgame_base_SRCS
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
if (PENTOBI_BUILD_GTP)
    set(boardgame_base_SRCS ${boardgame_base_SRCS}
        Engine.cpp
        Engine.h
        )
endif()

add_library(boardgame_base STATIC ${boardgame_base_SRCS})

target_link_libraries(boardgame_base boardgame_util)
if (PENTOBI_BUILD_GTP)
    target_link_libraries(boardgame_base boardgame_gtp)
endif()

target_include_directories(boardgame_base PUBLIC ..)
