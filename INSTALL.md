Compiling and Installing Pentobi From the Sources
=================================================

Requirements
------------

Building Pentobi requires the following tools and libraries:

* C++ compiler with C++20 support
* [Qt libraries](https://www.qt.io/) (>=6.5)
* [CMake](https://cmake.org/) (>=3.19)
* [GNU gettext](https://www.gnu.org/software/gettext/) (>=0.19.6)
* [LibRsvg](https://wiki.gnome.org/Projects/LibRsvg)

In Debian 13, they can be installed with the command
```
sudo apt install cmake g++ gettext librsvg2-bin make qml6-module-qtcore \
  qml6-module-qtqml-workerscript qml6-module-qtquick-controls \
  qml6-module-qtquick-dialogs qml6-module-qtquick-layouts \
  qml6-module-qtquick-templates qml6-module-qtquick-window qt6-base-dev \
  qt6-declarative-dev qt6-tools-dev
```

Building
--------

Pentobi can be compiled from the source directory with the commands
```
cmake .
make
```

Installing
----------

Pentobi can be installed after compilation with the command
```
sudo make install
```
