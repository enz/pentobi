if (NOT USE_QT5)
  include(${QT_USE_FILE})
endif()

include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(svgtopng Main.cpp)

if (USE_QT5)
  qt5_use_modules(svgtopng Widgets)
endif()

# Note: svgtopng also needs QtSvg, which provides a plugin not visible as a
# link time dependency
target_link_libraries(svgtopng ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY})
