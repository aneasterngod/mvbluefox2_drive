
Description
========================================


Procedure for building code using CMake
========================================
You can either create an in-source build, or an out-of-source build. 

An out-of-source build keeps clean separation between source and build, but requires more overhead to switch between source and build directories if necesary.

An in-source build is perhaps more intuitive, but has the disadvantage of polluting your source directories with auto-generated files.

To create an out-of-source build:

	[mvbluefox2_drive]$ mkdir build

	[mvbluefox2_drive]$ cd build

	[mvbluefox2_drive/build]$ cmake ..

	[mvbluefox2_drive/build]$ make

Note that the last two commands are executed inside the build directory.

Afterwards, your tree structure should look like this:

	[mvbluefox2_drive]$ tree -L 2

	.
	|-- CMakeLists.txt
	|-- README.md
	|-- build
	|   |-- CMakeCache.txt
	|   |-- CMakeFiles
	|   |-- Makefile
	|   |-- cmake_install.cmake
	|   |-- src
	|-- src
	  |-- CMakeLists.txt
	  |-- mvbluefox2_drive

and you can execute the program in the mvbluefox2.cpp/build/src directory as follows:

	[mvbluefox2_drive/build/src]$ ./mvbluefox2

To create an in-source build,

	[mvbluefox2_drive]$ cmake .

	[mvbluefox2_drive]$ make

This will create the executables in the mvbluefox2_drive/src directory.

For more information on using CMake, check out 

	http://www.cmake.org/cmake/help/cmake_tutorial.html

Dongshin Kim, 2018-05-07 23:12
	