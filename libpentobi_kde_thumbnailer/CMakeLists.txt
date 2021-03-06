# libpentobi_kde_thumbnailer contains the files needed by
# the pentobi_kde_thumbnailer plugin compiled with shared library options
# (usually -fPIC) because this is required for building shared libraries on
# some targets (e.g. x86_64).
#
# The alternative would be to add -fPIC to the global compiler flags even for
# executables but this slows down Pentobi's search by 10% on some targets.
#
# Adding the source files directly to pentobi_kde_thumbnailer/CMakeList.txt is
# not possible because the KDE CMake macros add -fno-exceptions to the
# compiler flags, which causes errors in the Pentobi sources that use
# exceptions (which should be fine as long as no exceptions are thrown
# from the thumbnailer plugin functions).

add_library(pentobi_kde_thumbnailer STATIC
  ../libboardgame_base/Assert.cpp
  ../libboardgame_base/Reader.cpp
  ../libboardgame_base/SgfError.cpp
  ../libboardgame_base/SgfNode.cpp
  ../libboardgame_base/SgfTree.cpp
  ../libboardgame_base/StringRep.cpp
  ../libboardgame_base/StringUtil.cpp
  ../libboardgame_base/TreeReader.cpp
  ../libpentobi_base/CallistoGeometry.cpp
  ../libpentobi_base/GembloQGeometry.cpp
  ../libpentobi_base/NexosGeometry.cpp
  ../libpentobi_base/NodeUtil.cpp
  ../libpentobi_base/TrigonGeometry.cpp
  ../libpentobi_base/Variant.cpp
  ../libpentobi_paint/Paint.cpp
  ../libpentobi_thumbnail/CreateThumbnail.cpp
)

target_include_directories(pentobi_kde_thumbnailer PRIVATE ..)

target_compile_options(pentobi_kde_thumbnailer PRIVATE
    ${CMAKE_SHARED_LIBRARY_CXX_FLAGS})

target_compile_definitions(pentobi_kde_thumbnailer PRIVATE
    QT_DEPRECATED_WARNINGS
    QT_DISABLE_DEPRECATED_BEFORE=0x051200)

target_link_libraries(pentobi_kde_thumbnailer Qt::Gui)
