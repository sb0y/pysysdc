#/bin/bash

if [[ -z "${BUILD_BUILDNUMBER}" ]];
then
	BUILD_BUILDNUMBER=$(grep "semver:" azure-pipelines.yml | awk '{ print $2 }').0-local
fi

PYT_VERSION=`echo $BUILD_BUILDNUMBER | sed 's/\-.*$//'`
DEB_VERSION=`echo $BUILD_BUILDNUMBER | sed 's/^.*\-//'`

echo "VERSION AZURE PIPELINE:  $BUILD_BUILDNUMBER"
echo "$PYT_VERSION"


# Set version in files
sed -i "s/%VERSION%/${PYT_VERSION}/" pysysdc/__version__.py stdeb.cfg

# https://github.com/pypa/setuptools/issues/3278
#export SETUPTOOLS_USE_DISTUTILS=stdlib
./clean.sh
python3 ./setup.py --command-packages=stdeb.command sdist_dsc --debian-version=${DEB_VERSION}
cp DEBIAN/postinst deb_dist/pysysdc*/debian/
cp DEBIAN/postrm deb_dist/pysysdc*/debian/
cp DEBIAN/preinst deb_dist/pysysdc*/debian/
cd deb_dist/pysysdc*/
#dpkg-buildpackage -k03CD7356 -S -sa
dpkg-buildpackage -rfakeroot -uc -us
