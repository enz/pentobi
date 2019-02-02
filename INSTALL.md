Compiling and Installing Pentobi From the Sources
=================================================

Requirements
------------

Pentobi requires the Qt libraries (>=5.11). The C++ compiler needs
C++17 support (GCC >=5). The build system uses CMake (>=3.1.0).

In Debian-based distributions, the necessary tools and libraries can be
installed with the command
```
sudo apt-get install cmake g++ libqt5svg5-dev libqt5webview5-dev make \
  qml-module-qt-labs-folderlistmodel qml-module-qt-labs-settings \  
  qml-module-qtquick2 qml-module-qtquick-controls2 \  
  qml-module-qtquick-layouts qml-module-qtquick-window2 \  
  qml-module-qtwebview qtquickcontrols2-5-dev qttools5-dev
```

Building
--------

Pentobi can be compiled from the source directory with the commands
```
cmake -DCMAKE_BUILD_TYPE=Release .
make
```

Building the KDE Thumbnailer Plugin
-----------------------------------

A thumbnailer plugin for KDE can be built by using the cmake option
-DPENTOBI_BUILD_KDE_THUMBNAILER=ON. In this case, the KDE development
files need to be installed (packages libkf5kio-dev and
extra-cmake-modules on Debian-based distributions). Note that the
plugin might not be found if the default installation prefix /usr/local
is used. You need to add QT_PLUGIN_PATH=/usr/local/lib/plugins to
/etc/environment. After that, you can enable previews for Blokus game
file in the Dolphin file manager in Configure Dolphin/General/Previews.

Installing
----------

On Linux, Pentobi can be installed after compilation with the command
```
sudo make install
```
After installation, the system-wide databases should be updated to
make Pentobi appear in the desktop menu and register it as handler for
Blokus files (*.blksgf). On Debian-based distributions with install
prefix /usr/local, this can be done by running
```
sudo update-mime-database /usr/local/share/mime
sudo update-desktop-database /usr/local/share/applications
```

Building the Android App
------------------------

Because building, deploying and debugging for Android is not yet
functional for CMake projects in QtCreator, there exists a project file
in pentobi/Pentobi.pro for building the Android app.
