
EyeOfFly aka EOF 
========

Throughput measurement and testing app, mostly intended to test CCTV & IP-video system designs 
for domestic projects

Building
========

You need following tools/libs:
1. Cmake v2.8
2. FLTK v1.3.*
3. OpengGL. spec v > 2
4. GLU (included in FLFK library)
5. Boost 1.53
6. recommended: libjpeg-turbo

Note: other versions of software may work.

### Linux

Go to dir you want to create copy of project, than
extract application sources from repository

$ git clone  https://github.com/karitra/eye-of-fly.git eye

Make build directory:

$ mkdir build
$ cd build

Run cmake:

$ BOOST_ROOT=$PATH_TO_BOOST_DIST cmake $PATH_TO_SRC

where $PATH_TO_BOOST_DIST is a path where cmake can find boost headers (usually root of boost/* path) and libraries. $PATH_TO_SRC is a path to freshly extracted sources, usually you can just type: 

$ cmake ..

If cmake generates makefile, run:

$ make

If cmake fail, report a bug ;)

### Windows

Should work, as all libraries and tools are crossplatform, but
have never tried build process on Windows.

### Other *NIXes

May work, but there aren't any reason to port app to other systems,
as they are not of any interest of customer.

TODO
====

### Wish list

* Show CPU load
* Add/Remove cameras with GUI
* Change dynamically the columns and rows number of view
* Reformat IO part as separate library
* Add resize of main view
* Context menu for every view cell
* Gprof and optimization of network part

### Distant future (probably - never)

* Save and restore configuration on exit/start
* Work with ONFIV protocol
* Work with our server part
* User accounts
