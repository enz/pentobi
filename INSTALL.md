Compiling and Installing Pentobi From the Sources
=================================================

Requirements
------------

Building Pentobi requires the following tools and libraries:

* C++ compiler with C++17 support (e.g. GCC >=5)
* [Qt libraries](https://www.qt.io/) (Linux: >=5.15; Android: 6.2)
* [CMake](https://cmake.org/) (>=3.1.0)
* [GNU gettext](https://www.gnu.org/software/gettext/) (>=0.19.6)
* [ITS Tool](http://itstool.org/)
* [xsltproc](http://xmlsoft.org/XSLT/xsltproc.html)
* [DocBooc XSL](http://www.sagehill.net/docbookxsl/)
* [LibRsvg](https://wiki.gnome.org/Projects/LibRsvg)

In Ubuntu 21.04, they can be installed with the command
```
sudo apt install cmake docbook-xsl g++ gettext itstool \
  librsvg2-bin make qml-module-qt-labs-folderlistmodel \
  qml-module-qt-labs-settings qml-module-qtquick2 \
  qml-module-qtquick-controls2 qml-module-qtquick-layouts \
  qml-module-qtquick-window2 qtdeclarative5-dev qtquickcontrols2-5-dev \
  qttools5-dev xsltproc
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
`-DPENTOBI_BUILD_KDE_THUMBNAILER=ON`. In this case, the KDE development
files need to be installed (packages `libkf5kio-dev` and
`extra-cmake-modules` on Debian-based distributions). Note that the
plugin might not be found if the default installation prefix `/usr/local`
is used. You need to add `QT_PLUGIN_PATH=/usr/local/lib/plugins` to
`/etc/environment`. After that, you can enable previews for Blokus game
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
prefix `/usr/local`, this can be done by running
```
sudo update-mime-database /usr/local/share/mime
sudo update-desktop-database /usr/local/share/applications
```
