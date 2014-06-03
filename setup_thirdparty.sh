# environnement
export ROOT_DIR=`pwd`
export THIRD_PARTY=$ROOT_DIR/third_party
export THIRD_PARTY_LIB=$THIRD_PARTY/lib
export THIRD_PARTY_DOWNLOAD=$ROOT_DIR/third_party_download
export LD_LIBRARY_PATH=$THIRD_PARTY_LIB

if [ -z "$CPU" ]; then
  export CPU=`nproc`
fi

printenv

# Creating folders
mkdir -p $THIRD_PARTY $THIRD_PARTY_LIB $THIRD_PARTY_DOWNLOAD

# ilmbase

cd $ROOT_DIR
wget http://download.savannah.nongnu.org/releases/openexr/ilmbase-2.1.0.tar.gz -O $THIRD_PARTY_DOWNLOAD/ilmbase.tar.gz
tar xfz $THIRD_PARTY_DOWNLOAD/ilmbase.tar.gz -C $THIRD_PARTY_DOWNLOAD/
cd $THIRD_PARTY_DOWNLOAD/ilmbase-2.1.0
./configure --prefix=$THIRD_PARTY && make install -j$CPU

# openexr

cd $ROOT_DIR
wget http://download.savannah.nongnu.org/releases/openexr/openexr-2.1.0.tar.gz -O $THIRD_PARTY_DOWNLOAD/openexr.tar.gz
tar xfz $THIRD_PARTY_DOWNLOAD/openexr.tar.gz  -C $THIRD_PARTY_DOWNLOAD/
cd $THIRD_PARTY_DOWNLOAD/openexr-2.1.0
./configure --prefix=$THIRD_PARTY --with-ilmbase-prefix=$THIRD_PARTY && make install -j$CPU

# boost - disabled

#cd $ROOT_DIR
#wget http://sourceforge.net/projects/boost/files/boost/1.55.0/boost_1_55_0.tar.bz2/download -O $THIRD_PARTY_DOWNLOAD/boost_1_55_0.tar.bz2
#tar xfj $THIRD_PARTY_DOWNLOAD/boost_1_55_0.tar.bz2  -C $THIRD_PARTY_DOWNLOAD/
#cd $THIRD_PARTY_DOWNLOAD/boost_1_55_0
#./bootstrap.sh && ./b2 -j$CPU -d1 install --prefix=$THIRD_PARTY

# openimageio | requires libjpeg, libtiff, libpng, boost filesystem regex system thread, openexr, ilmbase

cd $ROOT_DIR
wget https://github.com/OpenImageIO/oiio/archive/RB-1.4.zip -O $THIRD_PARTY_DOWNLOAD/oiio.zip
unzip -q $THIRD_PARTY_DOWNLOAD/oiio.zip -d $THIRD_PARTY_DOWNLOAD/
cd $THIRD_PARTY_DOWNLOAD/oiio-RB-1.4
make -j$CPU OIIO_BUILD_TOOLS=0 OIIO_BUILD_TESTS=0 USE_PYTHON=0 USE_OPENGL=0 USE_OCIO=0 USE_FIELD3D=0 USE_GIF=0 USE_OPENJPEG=0 USE_QT=0 BUILDSTATIC=1 LINKSTATIC=1 ILMBASE_HOME=$THIRD_PARTY OPENEXR_HOME=$THIRD_PARTY BOOST_HOME=$THIRD_PARTY INSTALLDIR=$THIRD_PARTY dist_dir=""

# libav

cd $ROOT_DIR
wget http://libav.org/releases/libav-10.1.tar.gz -O $THIRD_PARTY_DOWNLOAD/libav-10.1.tar.gz
tar xfz $THIRD_PARTY_DOWNLOAD/libav-10.1.tar.gz  -C $THIRD_PARTY_DOWNLOAD/
cd $THIRD_PARTY_DOWNLOAD/libav-10.1
./configure --prefix=$THIRD_PARTY && make install -j$CPU
