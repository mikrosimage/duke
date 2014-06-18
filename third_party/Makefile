CURRENT_DIR   :=$(shell pwd)
PREFIX_DIR    :=$(CURRENT_DIR)/build
LIB_DIR       :=$(PREFIX_DIR)/lib
PKGCONFIG_DIR :=$(PREFIX_DIR)/lib/pkgconfig

ILMBASE_PC    :=$(PKGCONFIG_DIR)/IlmBase.pc
OPENEXR_PC    :=$(PKGCONFIG_DIR)/OpenEXR.pc
LIBRAW_PC     :=$(PKGCONFIG_DIR)/libraw.pc
LIBAV_PC      :=$(PKGCONFIG_DIR)/libavfilter.pc
OIIO_PC       :=$(PREFIX_DIR)/lib/libOpenImageIO.a

export LD_LIBRARY_PATH=$(LIB_DIR)
export PKG_CONFIG_PATH=$(PKGCONFIG_DIR)

all: $(OIIO_PC) $(LIBAV_PC)

# make

$(ILMBASE_PC): ilmbase-2.1.0/Makefile
	cd ilmbase-2.1.0 && $(MAKE) install
	
$(OPENEXR_PC): openexr-2.1.0/Makefile
	cd openexr-2.1.0 && $(MAKE) install

$(LIBRAW_PC): LibRaw-0.16.0/CMakeLists.txt
	cd `dirname $<` && mkdir -p build && cd build && cmake \
	  -G "Unix Makefiles"                  \
	  -DCMAKE_INSTALL_PREFIX=$(PREFIX_DIR) \
	  -Wno-dev                             \
	  -DENABLE_OPENMP=ON                   \
    -DENABLE_LCMS=OFF                    \
    -DENABLE_EXAMPLES=OFF                \
    -DENABLE_RAWSPEED=OFF                \
    -DENABLE_DEMOSAIC_PACK_GPL2=OFF      \
    -DENABLE_DEMOSAIC_PACK_GPL3=OFF      \
    -DENABLE_DCRAW_DEBUG=OFF             \
	  .. &&	make install
	
$(OIIO_PC): oiio-RB-1.4/configure $(OPENEXR_PC) $(LIBRAW_PC)
	cd oiio-RB-1.4 && $(MAKE)    \
		BOOST_HOME=$(PREFIX_DIR)   \
	  ILMBASE_HOME=$(PREFIX_DIR) \
	  LIBRAW_PATH=$(PREFIX_DIR)  \
	  OPENEXR_HOME=$(PREFIX_DIR) \
	  BUILDSTATIC=1              \
	  LINKSTATIC=1               \
	  INSTALLDIR=$(PREFIX_DIR)   \
	  OIIO_BUILD_TESTS=0         \
	  OIIO_BUILD_TOOLS=0         \
	  USE_PYTHON=0               \
	  USE_OPENGL=0               \
	  USE_OCIO=0                 \
	  USE_FIELD3D=0              \
	  USE_GIF=0                  \
	  USE_OPENJPEG=0             \
	  USE_QT=0                   \
	  dist_dir=""

$(LIBAV_PC): libav-10.1/Makefile
	cd libav-10.1 && $(MAKE) install

# configure
define configure-command
cd `dirname $<` && ./configure --prefix=$(PREFIX_DIR) $*
endef
define cmake-command
cd `dirname $<` && ./configure --prefix=$(PREFIX_DIR) $*
endef

ilmbase-2.1.0/Makefile : ilmbase-2.1.0/configure
	$(configure-command)

openexr-2.1.0/Makefile : openexr-2.1.0/configure $(ILMBASE_PC)
	$(configure-command) --with-ilmbase-prefix=$(PREFIX_DIR)

libav-10.1/Makefile : libav-10.1/configure
	$(configure-command)

# unzipping
all-folders=ilmbase-2.1.0 openexr-2.1.0 boost_1_55_0 oiio-RB-1.4 libav-10.1 LibRaw-0.16.0

ilmbase-2.1.0/configure: ilmbase.tar.gz
	tar xfz $< && touch $@

openexr-2.1.0/configure: openexr.tar.gz ilmbase-2.1.0/configure
	tar xfz $< && touch $@

boost_1_55_0/bootstrap.sh: boost_1_55_0.tar.bz2
	tar xfj $< && touch $@

oiio-RB-1.4/configure: oiio.zip boost_1_55_0/bootstrap.sh openexr-2.1.0/configure
	unzip -q $< && touch $@

libav-10.1/configure: libav-10.1.tar.gz
	tar xfz $< &&	touch $@

LibRaw-0.16.0/CMakeLists.txt: libraw.zip
	unzip -q $< && touch $@

# downloads
define wget-command
wget -N --quiet -O $@ $1
endef

all-archives=ilmbase.tar.gz openexr.tar.gz boost_1_55_0.tar.bz2 oiio.zip libav-10.1.tar.gz libraw.zip

ilmbase.tar.gz:
	$(wget-command) http://download.savannah.nongnu.org/releases/openexr/ilmbase-2.1.0.tar.gz
	touch $@

openexr.tar.gz: ilmbase.tar.gz
	$(wget-command) http://download.savannah.nongnu.org/releases/openexr/openexr-2.1.0.tar.gz
	touch $@

boost_1_55_0.tar.bz2:
	$(wget-command) http://sourceforge.net/projects/boost/files/boost/1.55.0/boost_1_55_0.tar.bz2/download
	touch $@

oiio.zip: boost_1_55_0.tar.bz2 openexr.tar.gz
	$(wget-command) https://github.com/OpenImageIO/oiio/archive/RB-1.4.zip
	touch $@

libav-10.1.tar.gz:
	$(wget-command) http://libav.org/releases/libav-10.1.tar.gz
	touch $@

libraw.zip:
	$(wget-command) https://github.com/LibRaw/LibRaw/archive/0.16.0.zip
	touch $@


# clean

clean: clean-build clean-archives clean-folders

clean-build:
	rm -rf $(PREFIX_DIR)
	
clean-archives:
	rm -rf $(all-archives)	

clean-folders:
	rm -rf $(all-folders)
