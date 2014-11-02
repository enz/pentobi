# Install the opening book files. If you change the destination, you need to
# update the default for PENTOBI_BOOKS_DIR in the main CMakeLists.txt
install(FILES
  book_classic.blksgf
  book_classic_2.blksgf
  book_classic_3.blksgf
  book_duo.blksgf
  book_junior.blksgf
  book_trigon.blksgf
  book_trigon_2.blksgf
  book_trigon_3.blksgf
  DESTINATION ${CMAKE_INSTALL_DATADIR}/pentobi/books)
