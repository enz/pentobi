Compiling and Installing Pentobi From the Sources
=================================================

Requirements
------------

Building Pentobi requires the following tools and libraries:

* C++ compiler with C++20 support
* [Qt libraries](https://www.qt.io/) (>=6.4)
* [CMake](https://cmake.org/) (>=3.19)
* [GNU gettext](https://www.gnu.org/software/gettext/) (>=0.19.6)
* [ITS Tool](http://itstool.org/)
* [LibRsvg](https://wiki.gnome.org/Projects/LibRsvg)
* [xsltproc](http://xmlsoft.org/XSLT/xsltproc.html) (only on Unix)
* [DocBooc XSL](http://www.sagehill.net/docbookxsl/) (only on Unix)

In Debian 12, they can be installed with the command
```
sudo apt install cmake docbook-xsl g++ gettext itstool librsvg2-bin \
  make qml6-module-qt-labs-platform qml6-module-qt-labs-settings \
  qml6-module-qtqml-workerscript qml6-module-qtquick-controls \
  qml6-module-qtquick-dialogs qml6-module-qtquick-layouts \
  qml6-module-qtquick-templates qml6-module-qtquick-window \
  qt6-base-dev qt6-declarative-dev qt6-tools-dev xsltproc
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
Blokus files (*.blksgf). On Debian with install prefix `/usr/local`,
this can be done by running
```
sudo update-mime-database /usr/local/share/mime
sudo update-desktop-database /usr/local/share/applications
```
Depending on your distribution and desktop environment, it might be
necessary to run more commands, for example to update the GTK icon
cache.
