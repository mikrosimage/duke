CURRENT_DIR   :=$(shell pwd)

THIRD_PARTY_INSTALL_DIR ?= $(abspath ../third_party_dist)

PREFIX_DIR    :=$(THIRD_PARTY_INSTALL_DIR)
INCLUDE_DIR   :=$(PREFIX_DIR)/include
LIB_DIR       :=$(PREFIX_DIR)/lib
PKGCONFIG_DIR :=$(PREFIX_DIR)/lib/pkgconfig

ILMBASE_PC       :=$(PKGCONFIG_DIR)/IlmBase.pc
OPENEXR_PC       :=$(PKGCONFIG_DIR)/OpenEXR.pc
LIBRAW_PC        :=$(PKGCONFIG_DIR)/libraw.pc
LIBAV_PC         :=$(PKGCONFIG_DIR)/libavfilter.pc
OIIO_PC          :=$(PKGCONFIG_DIR)/OpenImageIO.pc

BOOST_NAME       :=boost_1_55_0
BOOST_THREAD_LIB :=$(LIB_DIR)/libboost_thread.a

export LD_LIBRARY_PATH=$(LIB_DIR)
export PKG_CONFIG_PATH=$(PKGCONFIG_DIR)

all: $(OIIO_PC) $(LIBAV_PC) gtest-1.7.0/CMakeLists.txt
.PHONY: all

# make
CMAKE:=cmake -DCMAKE_BUILD_TYPE=Release

$(ILMBASE_PC): ilmbase-2.1.0/Makefile
	$(MAKE) -C $(dir $<) install

$(OPENEXR_PC): openexr-2.1.0/Makefile
	$(MAKE) -C $(dir $<) install

$(LIBRAW_PC): LibRaw-0.16.0/CMakeLists.txt
	cd $(dir $<) && mkdir -p build && cd build && $(CMAKE) \
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
		.. && $(MAKE) install

$(OIIO_PC): oiio-master/CMakeLists.txt $(OPENEXR_PC) $(LIBRAW_PC) $(BOOST_THREAD_LIB)
	cd $(dir $<) && $(MAKE) \
		OIIO_INSTALL_PKGCONFIG=1   \
		BOOST_HOME=$(PREFIX_DIR)   \
		ILMBASE_HOME=$(PREFIX_DIR) \
		LIBRAW_PATH=$(PREFIX_DIR)  \
		OPENEXR_HOME=$(PREFIX_DIR) \
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
	$(MAKE) -C $(dir $<) install

# configure

ilmbase-2.1.0/Makefile : ilmbase-2.1.0/configure
	cd $(dir $<) && ./configure --prefix=$(PREFIX_DIR)
	touch $@

openexr-2.1.0/Makefile : openexr-2.1.0/configure $(ILMBASE_PC)
	cd $(dir $<) && ./configure --prefix=$(PREFIX_DIR) --with-ilmbase-prefix=$(PREFIX_DIR)
	touch $@

libav-10.1/Makefile : libav-10.1/configure
	cd $(dir $<) && ./configure --prefix=$(PREFIX_DIR)
	touch $@

#boost bootstrap

$(BOOST_THREAD_LIB): $(BOOST_NAME)/b2
	cd $(dir $<) && ./b2 --with-thread --with-filesystem --with-regex --with-system stage
	cp -R -f $(dir $<)/boost $(INCLUDE_DIR)
	cp -R -f $(dir $<)/stage/lib/* $(LIB_DIR)
	touch $@

$(BOOST_NAME)/b2: $(BOOST_NAME)/bootstrap.sh
	cd $(dir $<) && ./bootstrap.sh
	touch $@

# unzipping
UNZIP:=unzip -o -q
TARZ:=tar xfz
all-folders=$(BOOST_NAME) ilmbase-2.1.0 openexr-2.1.0 oiio-master libav-10.1 LibRaw-0.16.0 gtest-1.7.0

$(BOOST_NAME)/bootstrap.sh: $(BOOST_NAME).tar.gz
	$(TARZ) $<
	touch $@

ilmbase-2.1.0/configure: ilmbase.tar.gz
	$(TARZ) $<
	touch $@

openexr-2.1.0/configure: openexr.tar.gz ilmbase-2.1.0/configure
	$(TARZ) $<
	touch $@

oiio-master/CMakeLists.txt: oiio.zip openexr-2.1.0/configure
	$(UNZIP) $<
	touch $@

libav-10.1/configure: libav-10.1.tar.gz
	$(TARZ) $<
	touch $@

LibRaw-0.16.0/CMakeLists.txt: libraw.zip
	$(UNZIP) $<
	patch $@ < LibRaw-0.16.0.patch
	touch $@

gtest-1.7.0/CMakeLists.txt: gtest-1.7.0.zip
	$(UNZIP) $<
	touch $@

# downloads
WGET:=wget -N --quiet
all-archives=$(BOOST_NAME).tar.gz ilmbase.tar.gz openexr.tar.gz oiio.zip libav-10.1.tar.gz libraw.zip gtest-1.7.0.zip 

$(BOOST_NAME).tar.gz:
	$(WGET) -O $@ http://sourceforge.net/projects/boost/files/latest/download?source=files
	touch $@

ilmbase.tar.gz:
	$(WGET) -O $@ http://download.savannah.nongnu.org/releases/openexr/ilmbase-2.1.0.tar.gz
	touch $@

openexr.tar.gz: ilmbase.tar.gz
	$(WGET) -O $@ http://download.savannah.nongnu.org/releases/openexr/openexr-2.1.0.tar.gz
	touch $@

oiio.zip: openexr.tar.gz
	$(WGET) -O $@ https://github.com/gchatelet/oiio/archive/master.zip
	touch $@

libav-10.1.tar.gz:
	$(WGET) -O $@ http://libav.org/releases/libav-10.1.tar.gz
	touch $@

libraw.zip:
	$(WGET) -O $@ https://github.com/LibRaw/LibRaw/archive/0.16.0.zip
	touch $@

gtest-1.7.0.zip:
	$(WGET) -O $@ https://googletest.googlecode.com/files/gtest-1.7.0.zip
	touch $@

# clean
RM    := rm -rf

clean: clean-build clean-folders # clean-archives
.PHONY: clean

clean-build:
	$(RM) $(PREFIX_DIR)
.PHONY: clean-build

clean-archives:
	$(RM) $(all-archives)	
.PHONY: clean-archives

clean-folders:
	$(RM) $(all-folders)
.PHONY: clean-folders
