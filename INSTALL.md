Compiling and Installing Pentobi From the Sources
=================================================

Requirements
------------

Building Pentobi requires the following tools and libraries:

* C++ compiler with C++20 support
* [Qt libraries](https://www.qt.io/) (>=6.9)
* [CMake](https://cmake.org/) (>=3.22)
* [GNU gettext](https://www.gnu.org/software/gettext/) (>=0.19.6)
* [ITS Tool](http://itstool.org/)
* [LibRsvg](https://wiki.gnome.org/Projects/LibRsvg)

In Debian (upcoming version 14), they can be installed with the command
```
sudo apt install cmake g++ gettext itstool librsvg2-bin make \
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
