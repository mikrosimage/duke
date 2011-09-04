@echo off

if "%BOOST_ROOT%"=="" (
	echo ""
	echo "In order to use bjam and build $PROJECTNAME$, the BOOST_ROOT environment variable have to be set"
	echo "Edit and type the following command: set BOOST_ROOT=C:\your\path\to\boost\root\directory"
	echo "See README file for more informations."
	echo "Quit." 
	exit 
)

set BOOST_BUILD_PATH=%BOOST_ROOT%\tools\build\v2
%BOOST_ROOT%\bjam.exe --toolset=gcc %*
