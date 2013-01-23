Duke version 2.0 (alpha)
========================

Duke is an **opensource high resolution image and sequence viewer** dedicated to visual effect and post production companies.

Duke has been initiated by HD3D² R&D project, sustained by Cap Digital, the French business cluster for digital content and services.

The project is maintained as an open source project by [Mikros Image](http://www.mikrosimage.eu).

Disclaimer
----------
**This version is a complete rewrite so to minimize the dependencies and make it more attractive to adventurers.**  
**It is alpha stage so don't expect too much for now.**  
**Stay tuned and check this file from time to time to see our progress.**  

Current features
----------------
Duke is mostly an image viewer for now, it provides two very basic image readers ( Dpx, Tga ) and supports many more formats by the addition of [OpenImageIO](https://sites.google.com/site/openimageio/).

Pan and zoom is supported as well as channel selection (R,G,B,A) and exposure manipulation.  
ColorSpace is detected from metadata or file extension and adjusted to display on a sRGB monitor.  
Moving previous / next image or playing the loaded images


We aim at
---------

* 3D support
* timeline
* fluid playback
* movie playback as well as file sequences
* basic color grading
* timeline
* metadata display
* recording to file sequences or movies
* sound support



Compilation
-----------

* Getting the source

> git clone --recursive https://github.com/mikrosimage/duke.git  

* Compilation

  see INSTALL file


Tested compilers
----------------

* G++ 4.7.1 on OpenSUSE 12.2
* G++ 4.8 version macports (MacOs)

More informations 
-----------------

* Duke public repository
http://github.com/mikrosimage/duke

* The github issues module for bug report
https://github.com/mikrosimage/duke/issues


License
-------

    see COPYING file
