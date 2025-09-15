Pentobi Release Notes
=====================

Version 28.0-dev (main)
-----------------------

* 

Version 27.2-dev (v27-fixes)
----------------------------

* Fixed aspect ratio of exported Trigon images
* Disabled text selection in help viewer, which sometimes triggered
  Qt bugs that made clicking on links no longer work

Version 27.1 (12 Sep 2025)
--------------------------

* Workaround for a Qt bug on Android that sometimes resulted in empty
  window if app becomes active again
* Enable android.jniLibs.useLegacyPackaging for smaller APK size

Version 27.0 (10 Sep 2025)
--------------------------

* Minimum required Qt version is now 6.8
* Always use system theme (Note that on Ubunutu/Flatpak, you might
  have to install the Flatpak package org.gtk.Gtk3theme.Yaru-dark
  for dark mode support)
* Menu no longer covers menu button in mobile view (if not on Android)
  for better usability on mobile Linux devices
* Added close link in help viewer
* Workaround for QTBUG-140033 (Scrollbar not painted in Fusion style)

Version 26.4 (01 Sep 2025)
--------------------------

* Fixed comment area colors in dark mode
* Workaround for QTBUG-139679 which broke entering text in comment area
  in Qt 6.9.2

Version 26.3 (06 Feb 2025)
--------------------------

* Disabled native text rendering on desktop, which did not work well
  on some platforms.
* Indicate support for mobile devices in metainfo file.

Version 26.2 (23 Jan 2025)
----------------------------

* Added release info to appstream file, which is required on Flathub.

Version 26.1 (23 Jan 2025)
----------------------------

* Added developer id to appstream file, which is now required on Flathub.

Version 26.0 (23 Jan 2025)
--------------------------

* French translation of the manual.
* Fixed default window size.
* Mobile layout is now used by default on devices with small screens.
* New option --desktop.
* Minimum required Qt version is now 6.5.
* Removed dependency on qml6-module-qt-labs-settings (Debian).
* itstool is no longer required during the normal build process
  (see pentobi/help/create-translations)

Version 25.2 (12 Nov 2024)
--------------------------

* Fixed display problems in help window in some situations.
* Workaround for a Qt bug that made window disappear on some platforms
  when returning from fullscreen.

Version 25.1 (31 Jul 2024)
--------------------------

* Fixed menu text rendering issues on newer versions of KDE.

Version 25.0 (26 Apr 2024)
--------------------------

* Don't ask for continuing computer move anymore if computer is to play
  in current position after program startup.
* Don't support magic MIME type detection on Unix anymore, which was
  not reliable anyway and not working if installed via Flatpak.
* Workaround for QTBUG-119198 (Disabled menu item indistinguishable from
  enabled one).
* Compile QML sources into executable, which avoids problems of
  QML cache sometimes not being updated if installed via Flatpak.
* Fixed formatting of written game files.
* Improved visibility of analysis graph lines in theme System.
* Minor fixes.

Version 24.0 (22 Jan 2024)
--------------------------

* Removed GNOME thumbnailer and man pages, which didn't work anyway if
  installed via Flatpak. Use --help or --help-all to list the command
  line options for pentobi.
* Removed rarely used menu item Export Ascii Art.
* Enable only menu item Save As but not Save if game has no filename yet.
* Don't refuse to open SGF files with non-SGF text after tree (e.g.
  files that were corrupted on Android by bug fixed in version 23.1).
* Fixed rendering issues in help viewer on Android with Qt 6.5.
* Don't use Qt.labs.platform anymore. This replaces the run-time
  dependency on qml6-module-qt-labs-platform by qml6-module-qtcore
  (on Debian).

Version 23.1 (12 Mar 2023)
--------------------------

* Overwriting game file on Android with shorter file created corrupted
  file.

Version 23.0 (06 Mar 2023)
--------------------------

* Use native file dialogs on Linux to make loading and saving of games
  work with flatpak. This adds a run-time dependency on package
  qml6-module-qtquick-dialogs (on Debian).
* Prefix MIME config and icons with io.sourceforge.pentobi, such that
  registering Pentobi as an app for its game files works with flatpak.
* Removed option to disable animations.
* Removed menu item Open Clipboard.
* Require C++ compiler with C++20 support.
* Minimum required Qt version is now 6.4
* Removed custom CMake module to find path of Docbook XSL and let the
  local XML catalog do the mapping between URL and path.
* Don't initialize move number dialog with current move but show
  it as placeholder text.
* Don't require device with touchscreen on Android.

Version 22.1 (31 Jan 2023)
--------------------------

* Fixed game analysis hanging on some platforms.
* Don't use custom menu implementation, which caused problems in dark
  mode on Android and KDE 5. As a consequence, shortcuts are no longer
  shown on the menu but documented in the Pentobi manual.
* Made navigation links in help viewer part of the scrollable page to
  avoid a bug in Qt that made the text sometimes disappear if a
  navigation link was pressed with a scrolled page.
* Help window shortcuts stopped working after mouse click with Qt 6.4.
* Use Material style on Android.
* Android status and navigation bar colors now fit theme background.
* Busy indicator was not visible on Android if animations were disabled.

Version 22.0 (24 Jan 2023)
--------------------------

* Dropped support for Qt 5 (minimum required Qt version is now 6.2).
* Removed support for KDE thumbnailer.
* Minimum required CMake version is now 3.19.
* Don't use transparent background in thumbnails, which is displayed as
  a checkerboard in recent Gnome versions.
* Fixed removal of no longer existing files from recent files menu.
* Don't enforce compilation option -Wall to avoid false-positve
  array-bounds warnings with GCC.
* Use native file dialogs on Windows.
* Other minor fixes.

Version 21.0 (17 May 2022)
--------------------------

* New help viewer implementation, which includes DocBook files in the
  resources. This avoids using an external browser, which did not work
  with all installation directories on Ubuntu 21.10 because Firefox
  and Chromium have restricted access to local files, or using WebView,
  which is not supported on all Linux architectures or in Qt 6.0/6.1.
* Fixed visibility of labels on Callisto 1x1-piece in dark theme.
* Fixed rendering issues on Android with Qt 6.3.0.
* Several fixes to make Pentobi work on Windows
  (see https://github.com/lwchkg/pentobi/releases)
* Fixed detection of loaded files or autosaved games modified by
  another application.

Version 20.0 (21 Jan 2022)
--------------------------

* More general workaround to avoid crashes with Qt 5.15 related to
  storing QDateTime in QSettings.
* Workarounds for bugs in Qt 6.2 that sometimes caused wrong positions
  of unplayed pieces.
* Install user manual to /usr/share/doc to better conform to the GNU
  standard directories and because Firefox and Chromium on Ubuntu 21.10
  have no permissions to open files in /usr/share/help.
* Support for compiling Android app with Qt >=6.2.
* Install application and mimetype bitmap icons for multiple sizes.
* Show error message if launching web browser failed.
* Minimum required CMake version is now 3.18.

Version 19.1 (05 Dec 2021)
--------------------------

* Workaround to reduce the likelihood of crashes with Qt 5.15 related
  to storing QDateTime in QSettings.
* Dialog for starting rated game was not shown after accepting a dialog
  to discard current game.
* Tuned landscape layout on Android for 16:10 tablets.
* Avoid deprecated mimetypes tag in AppStream file.
* Avoid warnings with Qt 6 caused by deprecated signal handler syntax.

Version 19.0 (16 Apr 2021)
--------------------------

* The minimum required Qt version is now 5.15.
* Support compilation with Qt 6.
* The desktop version now always displays the manual with an external
  browser, which removes the dependency on the Qt WebView library and
  the compilation option PENTOBI_OPEN_HELP_EXTERNALLY.
* The Qt SVG library is no longer needed.
* Loading game files in encodings other than UTF-8 is no longer
  supported.
* Enabled Fusion style on desktop again now that QTBUG-77107 no longer
  appears in newer Fedora versions.
* Saving games on Android now uses the Storage Access Framework instead
  of legacy external storage, which supports building the app with
  target SDK version 30 (Android 11) and adds support for cloud-based
  storage.
* Enabled support for landscape orientation on Android.
* Migrated build system for Android app from qmake to CMake.
* Support for different comment showing modes (Always/As Needed/Never)
  on Android. 
* Remaining pieces on Android are now always shown in order to play
  even in game variants where a player plays two colors.

Version 18.5 (04 Mar 2021)
--------------------------

* Require GNU gettext >=0.19.6, which has built-in support for AppData,
  such that the appstream package is no longer needed for compilation
  (issue #11).
* Added missing include that broke the compilation with GCC 11
  (Debian bug 984290).
* Complete Russian translation of the manual.

Version 18.4 (25 Nov 2020)
--------------------------

* Added workaround for QTBUG-85449, which made the Android app crash
  on exit on some Huawei devices.
* Avoid unnecessary dependency on QtQml QML module (see also Debian bug
  973855).
* Added explicit dependency on metainfo.its in
  pentobi/unix/CMakeLists.txt.
* Aborting computer play with the Stop button did not work reliably
  when the computer played multiple colors at low levels.
* Russian translation of most of the manual.

Version 18.3 (04 Nov 2020)
--------------------------

* Made opening of recent rated games from rating dialog work again.
* New Russian UI translation (thanks to Viktor Erukhin)
* Back key now needs to be pressed twice to exit Android app to avoid
  unintentional exit with gesture navigation on newer Android versions.
* Better visual feedback of button presses on newer Android devices.
* Added workaround for white single-pixel line visible on some
  Android devices when switching fullscreen mode (QTBUG-55600).
* Added requestLegacyExternalStorage to Android manifest, which supports
  saving files if compiled with target SDK version 29 (Android 10).
* Menu shortcuts are no longer supported.

Version 18.2 (10 Oct 2020)
--------------------------

* Fixed truncated submenu in desktop version with Qt 5.15.
* Updated bugtracker link.

Version 18.1 (25 Jun 2020)
--------------------------

* Fixed compilation with Qt 5.15.
* Updated AndroidManifest.xml for usage with Qt 5.15.
* HTML manual generation failed with older versions of DocBook XSL.

Version 18.0 (11 May 2020)
--------------------------

* The minimum required Qt version is now 5.12.
* Experimental support for landscape orientation on Android (not
  enabled by default yet, see comment in GameViewMobile.qml).
* Fixed crash if application was closed while game analysis was running.
* Added missing include that broke the compilation with GCC 10 if
  compiled with PENTOBI_BUILD_GTP=1.

Version 17.3 (05 Nov 2019)
--------------------------

* Don't use Fusion style on desktop anymore because it is currently
  broken on some Linux distributions (QTBUG-77107).
* Added more search hints for location of DocBook XSL.

Version 17.2 (13 Sep 2019)
--------------------------

* New Spanish translation (thanks to Francisco Zamorano).
* Support for different analysis speeds on Android (only fast and normal
  speed).
* Added more search hints for location of DocBook XSL.
* PENTOBI_OPEN_HELP_EXTERNALLY will no longer be set automatically if
  Qt5WebView is not found, but must be set explicitly to avoid that it
  is accidentally used if the Qt5WebView package exists on the platform
  but has not been installed.
* New Game button and menu item were not enabled if no moves were played
  but an analysis graph existed (e.g. after analyzing a game and then
  undoing all moves).
* Clear autosaved analysis if autosaved game corresponded to a file
  that no longer exists.

Version 17.1 (12 Jun 2019)
--------------------------

* Move generation hung if a move generation using search was aborted
  and the next move generation used the opening book.
* Added search hint for DocBook XSL for compilation on OpenSUSE.

Version 17.0 (05 Jun 2019)
--------------------------

* Small increase in playing strength, mainly in Callisto and Classic.
* The user manual, desktop entry, AppData, MIME info and manpages are
  now translatable (GitHub issue #6). See INSTALL for the new build-time
  dependencies.
* New UI translation: Simplified Chinese (thanks to Cherry)
* Show message if move generation fails, which can happen in certain
  setup positions.
* Recommend that Blokus SGF files start with the GM property and reduced
  the maximum pattern offset for automatic MIME type detection to reduce
  the probability of false positives.
* The compiler now needs to support C++17.
* The SVG to bitmap conversions at build time are now done using
  LibRsvg instead of a custom compiled Qt-based build helper to make
  cross-compilation easier.
* The MetaInfo file for the KDE thumbnailer was removed as current
  software installations GUIs do not show addons anyway.
* Renamed build option PENTOBI_BUILD_TESTS to BUILD_TESTING for
  compatibility with CTest.

### Bug Fixes

* KDE thumbnailer crashed with unhandled exception on certain invalid
  Blokus SGF files.

Version 16.3 (17 Apr 2019)
--------------------------

* Piece in GembloQ could temporarily disappear if the rotate backward
  button was hit quickly multiple times.
* Piece could become stuck partially flipped if orientation change was
  triggered while last animation was still running.
* Current file name was not cleared and button New Game not enabled
  after opening a file from clipboard.
* Comment scrolling did not always work.
* Avoid jumping of splashscreen icon on Android.
* Shared MIME Info was missing a pattern for detecting Classic 3-player
  game files independent of the file extension.
* The Android version now interactively asks for permission when opening
  or saving a game if storage permission has not yet been granted.
* Development tool twogtp did not resolve ties as a win for the second
  player in two-player Callisto.
* Disabled QtQuickCompiler for desktop version to avoid the need for
  recompilation after upgrading the Qt libraries (see also Ubuntu bug
  #1824560).

Version 16.2 (16 Jan 2019)
--------------------------

* As a workaround for platforms without support for Qt5WebView, Pentobi
  can now be built such that the help is displayed in an external web
  browser. This option will automatically be used if Qt5WebView is not
  found or if the cmake option -DPENTOBI_OPEN_HELP_EXTERNALLY=ON is
  used. Note that this requires that a web browser is installed.
* The help files are no longer compiled into the resources but installed
  again in DATAROOTDIR/help.
* Fixed keyboard navigation in file dialog.
* Status message was not shown after successful Export/ASCII Art and
  Android Media Scanner was not informed to make the saved file
  immediately visible to MTP-connected devices.
* Changed new-game icon, which looked too much like a ratings/bookmarks
  icon.
* Fixed compilation on systems without sys/sysctl.h header.
* Enabled QML compiler again, now that QTBUG-70976 has been fixed, which
  broke translations in Qt 5.12 beta releases.
* Changed android.app_extract_android_style in Android manifest from
  none to minimal, which is recommended for Quick Controls 2 apps (see
  QTBUG-69810 and comments in QTBUG-71902)

Version 16.1 (11 Oct 2018)
--------------------------

* Fixed alignment issues of pieces on board if high-DPI scaling is used.

Version 16.0 (10 Oct 2018)
--------------------------

* The desktop version of Pentobi now uses the same QtQuick-based GUI as
  the Android version, which makes the desktop version support all
  features of the Android version like piece animations, dark and light
  themes and more of the state saved between sessions (e.g. position in
  game tree, modifications to loaded file, current analysis)
* The minimum required Qt version is now 5.11 also for the desktop
  version. See INSTALL for the new run-time and compile-time
  dependencies.
* The installation directory /usr/share/pentobi does not exist anymore.
  The translations, opening books and user manual are compiled as
  resources into the binary executable.
* New themes optimized for colorblindness.
* New appearance option in desktop mode that handles the comment
  visibility after a position change.
* New context menu to go to any played move on board or edit its
  annotation.
* Additional warning dialogs to reduce the likelihood that an autosaved
  game is lost, for example because it was changed by another instance
  of Pentobi.
* The computer level is now set in the computer colors dialog. It is no
  longer stored in the settings separately for each game variant.
* The visibility of the move number and variation information in the
  status bar can now be configured in the appearance dialog and is off
  by default.
* New toolbar button to stop computer play or game analysis.
* Play and undo buttons now support autorepeat.
* Reintroduced forward10/backward10 toolbuttons on desktop.
* New shortcut keys for moving the selected piece in larger steps on the
  board.
* New menu item Recent Files/Clear List.
* The locations and file format for the rated game history is not
  compatible with Pentobi 15.0, the rating history will be lost.
* New shortcut Ctrl+Shift+H, which behaves like Find Move (Ctrl+H) but
  iterates backwards through the list of legal moves.
* The game analysis now always contains a value for the position after
  the last move, which is useful for analyzing unfinished games.
* The Android version now shows an error before open/save if permission
  to access storage have not been granted.
* Android: color dot of the color to play is no longer surrounded by a
  border because the color to play is already indicated by having its
  unplayed pieces at the top.
* The translation source strings for menu items and actions no longer
  use an ampersand to mark a mnemonic but a separate translation string
  for the mnemonic.

### Bug Fixes (Both Desktop and Android Version of Pentobi 15.0)

* Fixed bugs in handling AE (add empty) SGF property.

### Bug Fixes (Android Version of Pentobi 15.0)

* Picking up a piece from board in setup mode sometimes switched piece
  instances in game variants with multiple instances per piece.
* Workaround for a bug that made the analysis graph only partially
  visible on Android low-density devices (QTBUG-69102)
* Program could hang or crash if quit during running game analysis.
* Board was not updated if it became empty after opening a file failed.
* Running computer move was not aborted after opening a file failed.
* Disable menu item Analyze Game if game has no moves.
* Show a meaningful error message if startup fails due to low memory.
* Show a warning if current game has unsaved changes before opening
  a file from clipboard.
* Changed text color on purple pieces to white to make it more readable.
* Game info was not updated after loading a file.
* Rating dialog did not show game variant in Callisto (2 players,
  4 colors)
* Don't crash if game analysis stored in settings was not valid.
* Game was not marked as modified after changing move annotation.

Version 15.0 (28 Jun 2018)
--------------------------

### General

* New UI translations: French, Norsk bokmål (thanks to Allan Nordhøy)
* Added a workaround for a compiler issue with GCC 7/8, which slowed
  down the startup time of Pentobi.
* Disable menu item "Keep Only Position" if board is empty.

### Android Version

* The minimum required Qt version is now 5.11.
* Games table in rating dialog did not show the correct level used.
* Saved files should now immediately be visible from computers
  connecting with the Android device via MTP (might not work on all
  devices).
* An error message is now shown when an invalid loaded SGF file causes
  a problem later (e.g. invalid move property value in a side
  variation).

Version 14.1 (03 Jan 2018)
--------------------------

### General

* Fixed a potential race condition during move generation.
* Reduced maximum memory usage to a quarter instead of a third of the
  total system memory.
* Made unit tests work again.

### Android Version

* Migrated QML files from Qt 5.6 to Qt >=5.7.
* The binary translation files are now automatically created by the
  qmake project file.

Version 14.0 (26 Oct 2017)
--------------------------

### General

* Increased playing strength in almost all game variants (except for
  Nexos), especially in Trigon, GembloQ and Callisto.
* Duo now uses the colors purple/orange.
* Junior now uses the colors green/orange.
* File format: accept whitespaces before and after property identifiers.

### Desktop Version

* Minimum required Qt version is now 5.6.
* Bugfix: dot indicating color to play in orientation selector was not
  always updated correctly after loading a file of a different game
  variant.
* Bugfix: added missing include that broke compilation on FreeBSD 11.

Version 13.1 (06 Jun 2017)
--------------------------

### General

* Fixed some crashes that could be triggered by invalid SGF files.

### Desktop Version

* Callisto: selected piece was wrongly rendered as one-piece in some
  situations if partially outside board.
* Fixed Leave Fullscreen button positioning if multiple screens exist.
* Window close button did not work in message dialogs with detailed
  text.
* Ctrl-W now closes application.
* Use reverse-domain file names for appstream and desktop file.
* Removed no longer needed workaround for disabling appstreamtest
  added by KDECMakeSettings.

### Android Version

* Displayed game variant was not changed when loading a file of a
  different game variant with SGF errors.

Version 13.0 (17 Mar 2017)
--------------------------

### General

* New game variant GembloQ.
* New game subvariant Callisto Two-Player Four-Color.
* Slightly increased playing strength in Callisto, Trigon and Nexos.
* New menu item Game/Open From Clipboard.
* The engine now uses up to 8 threads (instead of 4) by default if the
  CPU has enough hardware threads.
* Support for SGF file encodings other than ISO-8859-1 and UTF-8.

### Desktop Version

* Install AppData file to /usr/share/metainfo instead of
  /usr/share/appdata.
* Added AppStream file for the KDE thumbnailer.
* Disabled AppStream tests added by KDECMakeSettings that are broken in
  some versions of KDE and made the project tests fail.
* The compilation now requires CMake >=3.1.0.

### Android Version

* The Android version now supports most features of the desktop version,
  including comments, move annotations, setup positions, game analysis
  (only a very fast mode) and rated games. The playing levels are still
  restricted to 1-7 because the top levels would be too slow on mobile
  devices.
* Current game position, associated file name and file modification
  status are now remembered between sessions.
* Opened games now show the initial instead of the last position if the
  initial position contains either a setup or a comment.
* Game/Find Move now behaves like in the desktop version and will cycle
  through all legal moves if called repeatedly.
* Added a light theme in addition to the default dark theme.
* New menu item View/Fullscreen to make better use of small-screen
  displays.
* Bugfix: game variant Junior erroneously used level set for Classic.
* The minimum required Android version is now 4.1.

Version 12.2 (05 Jan 2017)
--------------------------

### Desktop Version

* Added patterns for Nexos and Callisto SGF files to MIME type
  specification for detecting them independently of the file ending.
* Game info properties were not removed from file if the corresponding
  text in the game info dialog was deleted.
* New Game/Save As was not enabled if no move had been played but game
  was modified by editing the comment in the root node or the game info.
* Fixed a race condition in updating the analysis window that could
  cause a crash while a game analysis was running.
* Game analysis progress dialog was not closed if analysis was canceled.

### Android Version

* Toolbuttons were too small on very high DPI devices.
* Open/Save did not show error message on failure.

Version 12.1 (30 Nov 2016)
--------------------------

### General

* Loading a file with a setup position in Nexos did not always work
  correctly or could cause a crash.
* SGF files for two-player Callisto did not use B/W properties as
  documented but 1/2 as in multi-player variants. Files written by
  Pentobi 12.0 can still be read and will be converted if saved again.

### Desktop Version

* Compilation on Windows is no longer tested or supported.
* Keep Only Position and Keep Only Subtree did not work correctly in
  Nexos and in multi-player Callisto.
* Delete All Variations did not mark the file as modified.
* Missing semicolon in desktop entry file (bug #12).
* Fixed ambiguous shortcut overload.
* Saving a file will now remember the directory and use it as a default
  for file dialogs.

Version 12.0 (10 Apr 2016)
--------------------------

### General

* New game variant Callisto.
* Thinking time of level 7 (the highest level supported on Android) was
  increased in most game variants to better match the CPU speed of
  typical mobile hardware.
* Starting points are no longer shown after color played its first
  piece.

### Desktop Version

* The compilation now requires at least Qt 5.2.
* High-DPI scaling is now automatically used if compiled with Qt 5.6.
* Setting Move Marking to Last now only marks the last move even if the
  computer played several moves in a row.

### Bug Fixes Desktop Version

* Icon for undo did not have a high-DPI version.
* Option --verbose was broken on Windows.

### Android Version

* The compilation now requires Qt 5.6.
* Support for game variant Nexos.
* New menu items Edit/Delete All Variations, Edit/Next Color,
  View/Animate Pieces, Help/About.
* Actions with buttons in action bar are no longer shown in menu.
* Forward/backward buttons now support autorepeat.

### Bug Fixes Android Version

* Fixed crash that could occur when switching game variants while a
  piece was selected.
* Level set for game variant Classic3 was ignored, instead the level set
  for Classic was used.
* Move generation was not properly aborted if some Edit menu items were
  selected while the computer was thinking.

Version 11.0 (29 Dec 2015)
--------------------------

### General

* Slightly increased playing strength, mainly in Trigon.
* The compilation requires now at least Qt 5.1 and GCC 4.9 or MSVC 2015.
* The score display now shows stars at scores that contain bonuses.

### Desktop Version

* New game variant Nexos (2 or 4 players).
* If a piece is removed from the board in setup mode, it will now
  become the selected piece.
* The command line option --memory was replaced by --maxlevel, which
  reduces the needed memory and removes higher levels from the menu.
* The memory requirements are now 1 GB minimum, 4 GB recommended for
  playing level 9.
* Added an application metadata file on Linux according to the AppStream
  specification from freedesktop.org. Added a 64x64 app icon but no
  longer an xpm icon (Debian AppStream Guidelines).

### Bug Fixes Desktop Version

* Message dialog about discarding unsaved current game was not shown if
  a file was loaded by clicking on a game in the rating dialog.
* Last move marking did not work anymore after after interrupting a
  computer move generation and then using Undo Move.
* Autosaving unfinished games did not work if game was finished
  first but then made unfinished again with Undo Move.
* Selecting pieces in setup mode did no longer work if no legal moves
  were left, even if setup mode is also intended to be used for
  setting up illegal positions (e.g. for Blokus art).

### Android Version

* Initial support for loading/saving, variations and game tree
  navigation.
* The piece area now has enough room for all pieces of one color. It
  also removes rows that become empty and orders the colors such that
  the color to play is always on top.
* Action buttons and menu items are now only shown if the action is
  enabled in the current position.

Version 10.1 (15 Oct 2015)
--------------------------

### Desktop Version

* New toolbar button for Undo Move.
* Annotations are now also appended to the move number in the status
  line.
* Don't show move number in status line if no moves have been played.
* Show an error message instead of the crash dialog if the startup
  fails due to low memory.
* The Windows installer is now built with Qt 5 and dynamic libraries.

### Android Version

* New action bar button for Undo Move.
* Reduced memory requirements. A meaningful error message is now shown
  if the startup fails due to low memory.
* Workaround for a bug that made the back button no longer exit the app
  after the computer color dialog was shown (QTBUG-48456).
* Faster startup.
* Changed snapping behavior of the piece area to make it easier to flick
  vertically between colors with multiple movements on small screens.

Version 10.0 (01 Jul 2015)
--------------------------

* Increased playing strength and more opening variety in Trigon.
* The Backward10/Forward10 toolbar buttons were replaced by autorepeat
  functionality of the Backward/Forward buttons.
* The last move is now by default marked with a dot instead of a number.
* The compilation now requires at least GCC 4.8 and CMake 3.0.2.
* On Linux, the manual is now installed in $PREFIX/share/help according
  to the freedesktop.org help specification.
* The KDE thumbnailer plugin can now be compiled with KDE Frameworks 5.
* Better support for high resolution displays if compiled with Qt 5.1
  or newer and environment variable QT_DEVICE_PIXEL_RATIO is used.
* The Pentobi help browser now uses a larger font on Windows
* Regional language subvariants en_GB, en_CA are no longer supported.

### Bug Fixes

* Fixed a build failure when generating the PNG icons from the SVG
  sources if the path contained non-ASCII characters.
* Fixed failure to open a file given as a command line argument to
  pentobi (including the case when Pentobi is used as a handler for
  blksgf files in file browsers) if the path contained non-ASCII
  characters.
* Changed the file dialog filter for "All files" from *.* to * such that
  really all files are shown even if they have no file ending.
  Added an "All files" filter to the Export/ASCII Art file dialog.
* Remembering the playing level separately for each game variant did not
  work if the game variant was implicitly changed by opening a file.
* "View/Move Numbers/Last" did not behave correctly after all colors
  were enabled in the Computer Colors dialog while a move generation was
  running.
* Fixed build failure with MSVC if MinGW was not also installed (because
  windres.exe was used)

Version 9.0 (10 Dec 2014)
-------------------------

* Newly supported game variant Classic for 3 players, in which the
  players take turns playing the fourth color.
* Increased playing strength, mainly in game variant Trigon.
* There are now 9 levels and the playing strength increases more evenly
  with the level. Ratings in rated games are still comparable to
  previous versions of Pentobi apart from Trigon at lower levels because
  Trigon starts now with a higher playing strength at level 1.
* The computer is now better at playing moves that maximize the score
  as long as they do not lead into riskier positions.
* The computer now remembers the playing level separately for each game
  variant and restores it when the game variant is changed.
* Player ratings now change faster if less than 30 rated games have been
  played, and slower afterwards.
* The mouse wheel can no longer be used for game navigation because it
  was too easy to trigger accidentally while playing a game. This also
  fixes the bug that the game navigation with the mouse wheel was not
  disabled in rated games and the game could not be continued after that
  because the play button is disabled in rated games.
* It is no longer possible to select and play a piece while the computer
  is thinking, the thinking must be aborted first with Computer/Stop.
* Bugfix: program crashed if computer colors dialog was opened and
  closed with OK while computer was thinking.
* Experimental support for Android. The Android version supports only a
  subset of the features of the desktop version and only playing levels
  1 to 7. There are still known issues with the user interface due to
  bugs in Qt for Android. The Android version is currently only
  available as an APK file for devices with an ARMv7 CPU from the
  download section of http://pentobi.sourceforge.net


Version 8.2 (05 Sep 2014)
-------------------------

* Fixed remaining link errors on some platforms (Debian bug #759852)

Version 8.1 (31 Aug 2014)
-------------------------

* Fixed link error on some platforms if Pentobi is compiled with
  PENTOBI_BUILD_TESTS (Debian bug #759852)
* Slightly improved some icons and use icons from theme for more menu
  items

Version 8.0 (02 Mar 2014)
-------------------------

* Increased playing strength, especially in game variant Trigon.
* Improved performance on multi-core CPUs: Previously, the move
  generation was faster on multi-core CPUs but there was a small drop
  in playing strength compared to the same playing level on a
  single-core CPU. This effect has been reduced.
* New toolbar button for starting a rated game.
* The interface is now more locked down during rated games, for example
  it is no longer possible to change the computer colors or take back a
  move during a rated game.
* The menu item "Computer Colors" was moved from the Game to the
  Computer menu.
* The source code no longer compiles with MSVC 2012 but requires
  MSVC 2013 because a larger subset of C++11 features is used.
* The source code distribution now uses xz instead of gzip for
  compression.
* The PNG versions of the icons are no longer included in the source
  code but generated at build time from the SVG icons by a small
  Qt-based helper program. This adds a build time dependency on QtSvg.
* A XPM icon is now installed to share/pixmaps.
* The configure option USE_BOOST_THREAD is no longer supported.
  For building with MinGW, a version of MinGW with support for
  std::thread is now required (e.g. from mingwbuilds.sf.net).

Version 7.2 (30 Jan 2014)
-------------------------

* Hyphens used as minus signs in manpage (bug #9)
* Added keywords section to desktop entry to silence lintian
  warning (bug #10)
* Fixed a compilation error with GCC 4.8.2 on PowerPC (and other
  big-endian systems)
* Fixed wrong arguments to update-mime-database/update-desktop-database
  when running "make post-install"
* Improved a blurry menu item icon
* Fixed a compilation warning about a missing translation
* Reduced the sizes of the generated and installed translation files.
* Fixed a compilation error on 64-bit Linux with X32 ABI
* Fixed a compilation error with Cygwin

Version 7.1 (13 Aug 2013)
-------------------------

* Fixed the version string. The released file pentobi-7.0.tar.gz was
  erroneously built from git version c5247c56 just before the version
  tagged with v7.0 and contained the version string 6.UNKNOWN
* The color played by the human in rated games is now randomly assigned
* The mouse wheel is now disabled while the computer is thinking

Version 7.0 (25 Jun 2013)
-------------------------

* Support for compilation with version 5 of the Qt libraries (see
  INSTALL for details)
* Slightly increased playing strength at higher levels (mainly in game
  variant Duo)
* The default settings in game variants with more than two players are
  now that the human plays the first color and the computer all other
  colors
* Fixed a crash that could occur if the window was put in fullscreen
  mode by a method of the window manager (e.g. title bar menu on KDE)
  and then returned to normal mode by a different method (e.g. pressing
  Escape)

Version 6.0 (4 Mar 2013)
------------------------

* Increased playing strength at higher levels. The search algorithm used
  for move generation is now parallelized and can take advantage of
  multi-core CPUs (up to 4 cores). There is a new playing level 8, which
  has a 2 GHz dual-core CPU or faster as the recommended system
  requirement.
* New menu item Toolbar Text to configure the toolbar button appearance
  independent of the system settings
* More SGF game info properties (event, round, time) were added to the
  game info dialog
* The source code now requires at least GCC 4.7 (because a larger subset
  of C++11 features is used)
* The CMake module GNUInstallDirs is now used for setting the
  installation directories on Unix. Note that the defaults for bindir
  and datadir are now CMAKE_INSTALL_PREFIX/bin and
  CMAKE_INSTALL_PREFIX/share instead of CMAKE_INSTALL_PREFIX/games and
  CMAKE_INSTALL_PREFIX/share/games. They can be changed by setting
  CMAKE_INSTALL_BINDIR and CMAKE_INSTALL_DATADIR (bug #7)
* The source code no longer depends on the Boost libraries. However, it
  is still possible to use Boost.Thread instead of std::thread by
  configuring with USE_BOOST_THREAD=ON (e.g. needed on MinGW GCC 4.7,
  which has no functional implementation of std::thread)
* Thumbnailer registration for blksgf files is no longer supported for
  Gnome 2

Version 5.0 (10 Dec 2012)
-------------------------

* Small increase in overall playing strength at higher levels in all
  game variants (especially Trigon)
* The computer now knows about the possibility of rotational-symmetric
  tied games in game variant Trigon Two-Player (like it already knew in
  the variants Duo and Junior) and will prevent the second player from
  enforcing such a tie
* If the move generation takes longer than 10 seconds, the maximum
  remaining time is now shown in the status bar
* Removed less frequently used buttons (Open, Save) from the tool bar
* Re-organized menu bar
* The menu bar and tool bar are no longer shown in fullscreen mode
* Avoided some window flickering at startup

Version 4.3 (2 Nov 2012)
------------------------

* Setting the computer color for Red with the computer colors dialog did
  not work for game variant Trigon Three-Player
* Disable Undo menu item when it is not applicable
* Fixed an assertion at end of move generation in Trigon Three-Player if
  Pentobi was compiled in debug mode

Version 4.2 (7 Oct 2012)
------------------------

* Fixed crash when opening game info dialog in game variants Classic
  Two-Player or Trigon Two-Player

Version 4.1 (5 Oct 2012)
------------------------

* Result of rated game was counted wrongly in four-color/two-player game
  variants if the first player had a higher score than the second player
  but the first color a lower score than the second color.
* Fixed potential crash if Undo, Truncate or Truncate Children is
  selected while the computer is thinking.
* Automatic continuing of computer play did not work in some cases if
  the computer was thinking while the Computer Color dialog was used.

Version 4.0 (4 Oct 2012)
------------------------

* New menu item "Beginning of Branch"
* The rating dialog now also shows the best previous rating and has
  a button to reset the rating
* A thumbnail plugin for KDE can be built by using the CMake option
  -DPENTOBI_BUILD_KDE_THUMBNAILER=ON
* Replaced the icons with less colorful ones. All icons are now licensed
  under the GPLv3+ and include SVG sources. No icons from the Tango icon
  set are used anymore.

Version 3.1 (2 Aug 2012)
------------------------

* Fixed a bug in version 3.0 in the replacement of obsolete move
  properties in old files that corrupted files in game variants with 3
  or 4 colors.

Version 3.0 (1 Aug 2012)
------------------------

* New functionality to compute a player rating for the user by playing
  rated games against the computer
* Different options for speed of game analysis
* New menu item "Play Single Move" to make the computer play a move
  without changing the colors played by the computer
* The mouse wheel can now be used to navigate in the current variation
  if no piece is selected
* Files written by older versions of Pentobi that use a deprecated
  format for move properties are now automatically converted to the
  current format on write

Version 2.1 (1 Jul 2012)
------------------------

* Bugfix: File was erroneously marked as modified if a multiline comment
  was shown and the platform that was used to create the file had
  Windows-style end of line convention and the platform on which the
  file was shown had Unix-style.
* Fixed the corruption of non-ASCII characters in game files on some
  platforms.
* Fixed a case where the program froze instead of showing an error on
  certain syntax errors in the SGF file.
* Fixed duplicate menu shortcut in German translation
* Fixed too high floating point tolerance in unit tests.

Version 2.0 (22 May 2012)
-------------------------

* No more popup messages if a color has no more moves;
  instead, score points of this color are underlined
  (feature request #3431031)
* Newly supported game variant Junior
* Improved playing strength. Number of levels increased to 7.
  Level 7 is about the same speed as the old level 6 but stronger.
* New game analysis function that shows a graph with the estimated
  value of each position in a game (menu item "Computer/Analyze Game")
* Support for setup properties in blksgf files (note that files
  with setup properties cannot be read by older versions of
  Pentobi). A new setup mode can be used to create files that start
  with a setup position including positions that cannot occur in
  real games (e.g. for puzzles or Blokus art)
* New menu items for editing the game tree: "Delete All Variations",
  "Keep Only Position", "Keep Only Subtree", "Move Variation Up/Down",
  "Truncate Children"
* Variations are now displayed by appending a letter to the move number
  instead of underlining
* Added a toolbar button for fast selection of the computer colors
  without having to use the window menu.
* User manual is no longer compiled into the resources of the
  executable but installed in the installation data directory
* Open a console for stderr output on Windows if Pentobi is
  invoked with option --verbose
* New option --memory to make Pentobi run on systems with low
  memory at the cost of reduced playing strength.
* Use standard icons from theme

Version 1.2 (17 Apr 2012)
-------------------------

* Bugfix: program sometimes hung or crashed when generating a
  move in early game Trigon positions especially when there
  were no legal moves with any of the large pieces
* Bugfix: file modified marker was not set on certain changes
  (Make Main Variation, comment changed)
* Bugfix: game info dialog showed wrong player labels in Trigon
  and Trigon Three-Player * Minor other bugfixes in the code
* Reverted the change that used the SVG icon for setting the
  window icon because it created an unwanted dependency on the
  Qt SVG plugin.
* Made Save menu item and tool button active if game is modified
  even if no file name is associated with the current game
* Made the code compile without warnings with GCC -Wunused
* Made "make post-install" continue even if some commands fail.

Version 1.1 (10 Mar 2012)
-------------------------

* File is now immediately visible in Recent Files menu after
  saving under a new name.
* Fixed several cases where the program crashed instead of showing
  an error message if the opened file was invalid. The error
  message now also has a Show Details button to show the reason
  why the file could not be loaded.
* Fixed a bug that distorted the position values reported with
  --verbose if a subtree from a previous search was reused
* Fixed exception in tools/twogtp/analyze.py if option -r was used
* Minor fixes in computer player engine
* Added explaining label to computer color dialog because window
  title is not visible in all L&F's
* Accept pass moves (empty value) in files. Although the current
  Blokus SGF documentation does not specify if they should be
  allowed, they might be used in the future and are used in files
  written by early (unreleased) versions of Pentobi
* Extended the file format documentation by a hint how to put
  blksgf files on web servers
* Smaller icons for piece manipulation buttons
* Fixed computation of the font bounding box in the score display
* Set option -std=c++0x in CMakeLists.txt if compiler is CLang
* Removed duplicate pentobi.png in directories data and src/pentobi;
  The file pentobi.svg was moved from data to src/pentobi and is
  now used for setting the window icon of Pentobi

Version 1.0 (1 Jan 2012)
------------------------

* Support for game variant Trigon Three-Player
* Change directory for autosave file to use AppData
  (on Windows) or XDG_DATA_HOME (on other systems)
* Changed Back to Main Variation to go to the last move
  in the main variation that had a variation, not to the
  last position in the main variation
* Changed variation string in status bar to contain
  information about the move numbers at the branching points
* Fixed small rendering errors
* New menu item Find Next Comment
* Added chapters about the main window and menu items to
  the user manual
* Fix bug: computer color dialog did not set colors correctly
  in game variant Trigon
* Show error message instead of crashing if the SGF file
  contains invalid move properties
* Lowered the required version of the Boost libraries in
  CMakeLists.txt from 1.45 to 1.40 such that Pentobi can
  be compiled on Debian 6.0.
  Note: some versions of Boost cause compilation errors if
  used with certain versions of GCC and option -std=c++0x
  (e.g. the combinations GCC 4.4/Boost 1.40 in Ubuntu 10.04
  and GCC 4.4/Boost 1.42 in Debian 6.0 work but the combination
  GCC 4.5/Boost 1.42 in Ubuntu 11.04 causes errors).
* Changed installation directories according to Filesystem
  Hierarchy Standard (/usr/bin to /usr/games, /usr/share to
  /usr/share/games)
* New CMake option PENTOBI_REGISTER_GNOME2_THUMBNAILER for
  disabling the installation of files for registering the Pentobi
  thumbnailer on Gnome 2
* Install man pages for pentobi and pentobi-thumbnailer on Unix
  systems

Version 0.3 (2 Dec 2011)
------------------------

* Support for the game variants Trigon and Trigon Two-Player
* Fixed saving/opening files if file name contained non-ASCII characters
  and the system used an encoding other than Latin1
* The score numbers now show the total player and color scores instead
  of on-board and bonus points separately (feature request #3431039)
* New menu item "Edit/Select Next Color" that allows to enter moves
  independent of the color to play on the board (feature request
  #3441299)
* Slightly changed file format to use single-valued move properties as
  used in other games supported by SGF. Files written by Pentobi 0.2 can
  still be read.

Version 0.2 (17 Oct 2011)
-------------------------

* German translation
* Display sum score for both player colors in game variant Classic
  Two-Player
* Slightly changed file format to conform to the proposed version 5 of
  SGF that requires digits for move properties in multi-player games.
  Files written by Pentobi 0.1 can still be read.
* Support for move annotation symbols
* Store and edit additional game information (player names, date)
* New menu items Ten Moves Backward/Forward, Go to Move, Undo Move
* Underline move numbers if there are alternative variations
* Show move number, total number of moves and current variation in
  status bar
* Faster play in higher levels, especially of opening moves
* Make thumbnailer for Blokus files work under Gnome 3
* Fix broken compilation with GCC 4.6.1 (bug #3420555)

Version 0.1 (15 Jul 2011)
-------------------------

* Initial release.
