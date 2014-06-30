Compiling Duke
==============

For the moment Duke compiles on Linux and OSX. The Windows version is untested and will probably not compile.

First, have a look at the [Travis-CI configuration](.travis.yml) to know what packages you'll need to successfully build Duke.

Duke is known to compile on GCC 4.8 / clang 3.4 or later.

Linux and OSX
-------------

On Linux/OSX the build is fully automatic : dependencies will be fetched and compiled as needed. This may take a long time so consider using make -j option to speed up the build.

To compile duke 
> make

To publish duke and its dependencies in a 'dist' folder
> make dist

To create a ready to use archive
> make package

If you need to build, distribute or package a debug version just set the DEBUG environment variable beforehand. eg :
> DEBUG=1 make dist
