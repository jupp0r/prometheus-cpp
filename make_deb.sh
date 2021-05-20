#/bin/bash -ex
# Create debian package

echo "Usage: $0 <version>"

if [ -z "$1" ]; then
    echo "You must provide a version name";
    exit -1;
fi

# Configure your paths and filenames
DEBPACKAGE="prometheus-cpp"
DEBVERSION=$(grep project CMakeLists.txt | grep VERSION | awk -F ' ' '{print $3}' | awk -F ')' '{print $1}')
DEBAVIDBOTSVERSION="$1"
DEBREVISION=avidbots${DEBAVIDBOTSVERSION}
DEBARCH="amd64"
PACKAGE=${DEBPACKAGE}_${DEBVERSION}-${DEBREVISION}_${DEBARCH}

# Create package folder structure
rm -rf build
mkdir -p build/$PACKAGE

# build
pushd build
#cmake .. -DINSTALL_DIR=$PACKAGE/opt/$DEBPACKAGE
#cmake .. -DBUILD_SHARED_LIBS=ON
cmake .. -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=/usr
make 
mkdir -p $PACKAGE
make DESTDIR=`pwd`/$PACKAGE install

# Create debian package control file based on the package
pushd $PACKAGE
mkdir -p DEBIAN
cat << EOF > DEBIAN/control
Package: $DEBPACKAGE
Architecture: all
Version: $DEBVERSION-$DEVREVISION
Maintainer: Frank Imerson <frank.imerson@avidbots.com>
Homepage: https://github.com/avidbots/prometheus-cpp
Description: This library aims to enable Metrics-Driven Development for C++ services
EOF
popd 

dpkg-deb --build $PACKAGE
