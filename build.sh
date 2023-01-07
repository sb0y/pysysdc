#/bin/bash -x

if [ -z ${BUILD_BUILDNUMBER} ]
then
	BUILD_BUILDNUMBER=$(grep "semver:" azure-pipelines.yml | awk '{ print $2 }').0-local
fi

PYT_VERSION=$(echo $BUILD_BUILDNUMBER | sed 's/\-.*$//')
DEB_VERSION=$(echo $BUILD_BUILDNUMBER | sed 's/^.*\-//')

# Set  version in files
sed -i "s/%VERSION%/${PYT_VERSION}/" pysysdc/__version__.py stdeb.cfg

./clean.sh
SETUPTOOLS_USE_DISTUTILS=stdlib python3 ./setup.py --command-packages=stdeb.command sdist_dsc --debian-version=${DEB_VERSION}
cp DEBIAN/postinst deb_dist/pysysdc*/debian/
cp DEBIAN/postrm deb_dist/pysysdc*/debian/
cp DEBIAN/preinst deb_dist/pysysdc*/debian/
cd deb_dist/pysysdc*/
dpkg-buildpackage -rfakeroot -uc -us
