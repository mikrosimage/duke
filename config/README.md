
"I wanna build, what's this user-config thing ?"
-----------------------------------------------

The Boost.Build system allows you to decouple the project itself from
the needed libraries. That's great because libraries installation can vary
so much depending on your OS, Linux flavor, rights, global, local or
hacked install...

So the user-config.jam file is read by Boost.Build at the very beginning of
the compilation process and will define how your libraries are installed.

There's a lot of options and as a starter you need to understand how
Boost.Build declares libraries. Check out the 
[doc](http://www.boost.org/boost-build2/doc/html/bbv2/tasks/libraries.html)  
Or have a look at the predefined user-config.jam files in this directory.



"Ok... user-config configured, what's next ?"
--------------------------------------------

First, be sure to uncomment the last line of your user-config.jam.
You must have exactly the following line in your file or you won't be able
to compile:

    constant duke_user_configuration : true ;

Then Boost.Build needs to be aware of this file, you have several options to
do so. Check  the 'Configuration' section of the 
[documentation](http://www.boost.org/boost-build2/doc/html/bbv2/overview/configuration.html)

To sum up, you can :

* move you're user-config.jam file in your `$HOME` or `$BOOST_BUILD_PATH` (1)
* specify the file on the command line, eg :

        ./build-linux-gcc.sh --user-config=/path/to/user-config.jam

You're done !



"Erh!? compilation complains about syntax error ?!"
--------------------------------------------------

Syntax in Boost.Build is very peaky. All the token must be separated by at least 
one whitespace : tab, space or line feed.

For instance, the following is invalid (twice):

    lib mylib: <file>/mylib/lib.a;
       ------^------       ------^------
       missing space       missing space

*Note for Windows users, you must not use the \ sign in your paths  
C:\PATH\TO\FILE is written C:/PATH/TO/FILE instead.*


--
--


> 1 - BOOST_BUILD_PATH is a mandatory environment variable needed by the
 Boost.Build system. It's pointing to the $(BOOST_ROOT)/tools/build/v2
 directory. The provided build scripts automatically sets that variable
 for you.