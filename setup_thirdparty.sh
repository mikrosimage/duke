# environnement
export ROOT_DIR=`pwd`
export THIRD_PARTY=$ROOT_DIR/third_party
export THIRD_PARTY_LIB=$THIRD_PARTY/lib
export THIRD_PARTY_DOWNLOAD=$ROOT_DIR/third_party_download
export LD_LIBRARY_PATH=$THIRD_PARTY_LIB
export NPROC=`nproc`

printenv

# Creating folders
mkdir -p $THIRD_PARTY $THIRD_PARTY_LIB $THIRD_PARTY_DOWNLOAD

# ilmbase

cd $ROOT_DIR
wget http://download.savannah.nongnu.org/releases/openexr/ilmbase-2.1.0.tar.gz -O $THIRD_PARTY_DOWNLOAD/ilmbase.tar.gz
tar xfz $THIRD_PARTY_DOWNLOAD/ilmbase.tar.gz -C $THIRD_PARTY_DOWNLOAD/
cd $THIRD_PARTY_DOWNLOAD/ilmbase-2.1.0
./configure --prefix=$THIRD_PARTY
make install -j$NPROC

# openexr

cd $ROOT_DIR
wget http://download.savannah.nongnu.org/releases/openexr/openexr-2.1.0.tar.gz -O $THIRD_PARTY_DOWNLOAD/openexr.tar.gz
tar xfz $THIRD_PARTY_DOWNLOAD/openexr.tar.gz  -C $THIRD_PARTY_DOWNLOAD/
cd $THIRD_PARTY_DOWNLOAD/openexr-2.1.0
./configure --prefix=$THIRD_PARTY --with-ilmbase-prefix=$THIRD_PARTY
make install -j$NPROC

# boost - disabled

#cd $ROOT_DIR
#wget http://sourceforge.net/projects/boost/files/boost/1.55.0/boost_1_55_0.tar.bz2/download -O $THIRD_PARTY_DOWNLOAD/boost_1_55_0.tar.bz2
#tar xfj $THIRD_PARTY_DOWNLOAD/boost_1_55_0.tar.bz2  -C $THIRD_PARTY_DOWNLOAD/
#cd $THIRD_PARTY_DOWNLOAD/boost_1_55_0
#./bootstrap.sh
#./b2 -j$NPROC -d1 install --prefix=$THIRD_PARTY

# openimageio | requires libjpeg, libtiff, libpng, boost filesystem regex system thread, openexr, ilmbase

cd $ROOT_DIR
wget https://github.com/OpenImageIO/oiio/archive/Release-1.4.8.zip -O $THIRD_PARTY_DOWNLOAD/oiio.zip
unzip $THIRD_PARTY_DOWNLOAD/oiio.zip -d $THIRD_PARTY_DOWNLOAD/
cd $THIRD_PARTY_DOWNLOAD/oiio-Release-1.4.8
make -j$NPROC OIIO_BUILD_TOOLS=0 OIIO_BUILD_TESTS=0 USE_PYTHON=0 USE_OPENGL=0 USE_OCIO=0 USE_FIELD3D=0 USE_GIF=0 USE_OPENJPEG=0 USE_QT=0 BUILDSTATIC=1 LINKSTATIC=1 ILMBASE_HOME=$THIRD_PARTY OPENEXR_HOME=$THIRD_PARTY BOOST_HOME=$THIRD_PARTY INSTALLDIR=$THIRD_PARTY dist_dir=""
