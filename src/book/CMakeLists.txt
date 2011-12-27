# Install the opening book files. If you change the destination, you need to
# update the directory used in
# src/libpentobi_mcts/Player.cpp:Player::load_book()
install(FILES
  book_classic.blksgf
  book_classic_2.blksgf
  book_duo.blksgf
  book_trigon.blksgf
  book_trigon_2.blksgf
  book_trigon_3.blksgf
  DESTINATION share/games/pentobi)
