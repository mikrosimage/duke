Duke - Install HOWTO
====================

Why Boost.Build ?
-----------------

Duke relies heavily on the Boost library, so instead of using one of
the many multiplatform build system available like Scons, CMake, Autotools
we're simply using Boost.Build, the build system embedded within Boost.

Boost.Build is powerful yet very expressive and terse. It's a weapon of
choice when building large pieces of code with a lot of dependencies.

An excellent introduction to Boost.Build can be found [here](http://www.highscore.de/cpp/boostbuild/)

And the official documentation is available [online](http://www.boost.org/boost-build2/doc/html/) 
or in your boost copy at `$(BOOST_ROOT)/tools/build/index.html` 


Setting up the build process
----------------------------

### Building the building tool (the build-ception...)  

* First grab Boost from the website ( http://www.boost.org/ )
* Uncompress the archive somewhere.
* Open a terminal and go to the boost root folder, then type 
    
    - Linux / MacOsX
    
            ./bootstrap.sh
    
    - Windows
    
            bootstrap.bat

    This will generate a 'b2' executable.



### Setting up environment variables  

In order to use Boost.Build you have to set the `BOOST_ROOT` environment variable.

- Linux / MacOSX

        export BOOST_ROOT=/your/path/to/boost/root


- Windows

        set BOOST_ROOT=C:\your\path\to\boost\root
    
*Note : You can add it to your ~/.bashrc, ~/.profile
or to your Windows environment to make it persistent.*



### External libraries  

Duke currently relies on the following libraries

#### mandatory  

* [`boost 1.49.0`](http://www.boost.org/users/history/version_1_49_0.html)
* [`Cg-3.1_February2012_x86`](http://http.developer.nvidia.com/Cg/cg_3_1_0010.html)
* [`protobuf-2.4.1`](http://code.google.com/p/protobuf/downloads/list)
* [`Qt 4.8`](http://qt-project.org/downloads)
* `SFML` from Github [commit 92f14fa8538fd8c1db93e4dcfb130a60dbafd651](https://github.com/LaurentGomila/SFML/commit/92f14fa8538fd8c1db93e4dcfb130a60dbafd651) 
 * `OpenAl`
 * `libsndfile`

**Please note that we are currently trying to prune this list so to keep the dependencies at a minimum.**  
**We do agree this list is way too long and we plan to get rid of SFML and al. in a near future.**

#### optional

* `OIIO RB-1.0` from Github [commit 513d0dd5af6568111e88e65c15e77470e2ae6d82](https://github.com/OpenImageIO/oiio/commit/513d0dd5af6568111e88e65c15e77470e2ae6d82)
* [`libjpeg-turbo-1.2.0`](http://sourceforge.net/projects/libjpeg-turbo/files/1.2.0/)

Have a look at the `config/README` file to learn how to tell the build system where to find them.
     


Building
--------

* Choose the appropriate script depending on your platform  

    - Linux

            ./build-linux-gcc.sh

    - MacOSX

            ./build-osx-darwin.sh

    - Windows

            build-windows-mingw.bat


* Run it with one or more of the following command line arguments  

    --user-config=XXX  
        __Specifies the configuration file__  
        
    --help  
        __Display help message__  
        
    --clean  
        __Remove targets instead of building__  

    --no-XXX  
       __With XXX being one of plugin-jpg, plugin-dpx, plugin-oiio__
 
    dist  
        __Full build & installation__  
        
    tests  
        __Build & run unit test suite__  
        
    debug -or- release  
        __Select the build variant ( debug is default )__  