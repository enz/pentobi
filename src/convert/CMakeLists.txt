if (NOT USE_QT5)
  include(${QT_USE_FILE})
endif()

include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(convert Main.cpp)

if (USE_QT5)
  qt5_use_modules(convert Widgets)
endif()

# Note: convert also needs QtSvg, which provides a plugin not visible as a
# link time dependency
target_link_libraries(convert ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY})
