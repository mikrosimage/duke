#!/bin/bash

if [ ! $BOOST_ROOT ]
then 
	echo 
	echo "In order to use bjam and build Duke, the BOOST_ROOT environment variable have to be set"
	echo "Edit and type the following command: export BOOST_ROOT=/your/path/to/boost/root/directory"
	echo "See README file for more informations."
	echo "Quit." 
	exit 1;
fi

# boost build configuration
export BOOST_BUILD_PATH=$BOOST_ROOT/tools/build/v2
$BOOST_ROOT/bjam --toolset=darwin cflags=-fPIC cxxflags=-fPIC $*
