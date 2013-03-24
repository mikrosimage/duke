Compiling Duke
==============

For the moment Duke can compile on Linux only. The MacOs version is on its way and the Windows version is untested
and will probably not compile as of now. We expect to support those three platforms in a near future.

###OpenImageIO

By default, Duke is shipped with a very basic (almost unusable) TGA reader and a fast DPX reader.
To enjoy playing other format we highly recommend you to use [OpenImageIO](https://github.com/OpenImageIO/oiio).
See below to learn more on how to configure Duke with OpenImageIO for your OS.

###CMake
Duke is using CMake to manage cross platform compilation. Make sure to have CMake 2.8 installed.

Linux
-----

You'll need a C++11 compiler, Duke is known to compile on GCC 4.7.1 and later.

####OpenImageIO
If you want OpenImageIO's support you'll have to compile the library and set the `OPENIMAGEIO_ROOT_DIR`
environment variable to the directory containing the `include`, `lib` and `bin` folders.
> export OPENIMAGEIO_ROOT_DIR=path/to/oiio/build/folder


####Tests
If you want the test suite to be run right after Duke's compilation just retrieve
[gtest-1.6.0.zip](https://code.google.com/p/googletest/) and unzip it into `dependencies/gtest-1.6.0`


####Configuration and build
We provide a `./configure` script to configure CMake for Linux users to feel right at home.

To build the release version, just type
> ./configure -DCMAKE_BUILD_TYPE=Release  
> make

Consider adding a -jX option to `make` where X is the number of core of your machine to speed up the compilation.

The executable will be available in `./build/src/duke`
