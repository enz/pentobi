Compiling and Installing Pentobi From the Sources
=================================================

Requirements
------------

Building Pentobi requires the following tools and libraries:

* C++ compiler with C++17 support (e.g. GCC >=5)
* [Qt libraries](https://www.qt.io/) (>=5.15)
* [CMake](https://cmake.org/) (>=3.19)
* [GNU gettext](https://www.gnu.org/software/gettext/) (>=0.19.6)
* [ITS Tool](http://itstool.org/)
* [LibRsvg](https://wiki.gnome.org/Projects/LibRsvg)
* [xsltproc](http://xmlsoft.org/XSLT/xsltproc.html) (only on Unix)
* [DocBooc XSL](http://www.sagehill.net/docbookxsl/) (only on Unix)

In Ubuntu 22.04, they can be installed with the command
```
sudo apt install cmake docbook-xsl g++ gettext itstool \
  librsvg2-bin make qml-module-qt-labs-folderlistmodel \
  qml-module-qt-labs-platform qml-module-qt-labs-settings \
  qml-module-qtquick2 qml-module-qtquick-controls2 \
  qml-module-qtquick-layouts qml-module-qtquick-window2 \
  qtdeclarative5-dev qtquickcontrols2-5-dev qttools5-dev xsltproc
```

Building
--------

Pentobi can be compiled from the source directory with the commands
```
cmake -DCMAKE_BUILD_TYPE=Release .
make
```
To use an installation directory different from the cmake default, use
the cmake option `CMAKE_INSTALL_PREFIX`.

Installing
----------

Pentobi can be installed after compilation with the command
```
sudo make install
```
After installation, the system-wide databases should be updated to
make Pentobi appear in the desktop menu and register it as handler for
Blokus files (*.blksgf). On Ubuntu with install prefix `/usr/local`,
this can be done by running
```
sudo update-mime-database /usr/local/share/mime
sudo update-desktop-database /usr/local/share/applications
```
Depending on your distribution and desktop environment, it might be
necessary to run more commands, for example to update the GTK icon
cache.
