#!/usr/bin/env python3

from setuptools import setup, Extension
import re
import subprocess

VERSION = '1.0.0'
with open('pysysdc/__version__.py', 'r') as f:
	VERSION = f.read().strip()

VERSION = re.sub(r'[^0-9.]+', '', VERSION)

long_description = ''
with open('README.md', 'r') as f:
	long_description = f.read()
libsystemd_ver = 'unknown'
try:
	libsystemd_ver = subprocess.check_output(['pkg-config', '--modversion', 'libsystemd'], stderr=subprocess.STDOUT, shell=False, universal_newlines=True).strip()
except Exception as e:
	print("Failed to detect libsystemd version")
	print(str(e))

setup(
	name="pysysdc",
	version=VERSION,
	description="Python C library for SD-Bus communication",
	long_description_content_type='text/markdown',
	long_description=long_description,
	include_package_data=True,
	author_email="andrey@bagrintsev.me, 1timkill@gmail.com, avdieev@gmail.com",
	#long_description_content_type='text/markdown',
	author="Andrey Bagrintsev, Anatolii Cheban, Oleksandr Avdieiev",
	maintainer="Andrey Bagrintsev",
	maintainer_email="andrey@bagrintsev.me",
	url="https://github.com/sb0y/pysysdc",
	license="BSD",
	packages=["pysysdc"],
	classifiers=[
		"Intended Audience :: Developers",
		"Topic :: Software Development :: Libraries :: Python Modules",
		"License :: OSI Approved :: GNU General Public License (GPL)",
		"Development Status :: 5 - Production/Stable",
		"Operating System :: Unix",
		"Programming Language :: C",
	],
	ext_modules=[
		Extension(
			"_pysysdc",
			[
				"pysysdc/pysysdc.c",
				"pysysdc/sdbus.c",
				"pysysdc/python_runner.c",
				"pysysdc/systemd_methods.c",
				"pysysdc/dc_error_handling.c",
				"pysysdc/helpers.c",
				"pysysdc/connect.c",
				"pysysdc/variant.c",
				"pysysdc/signals.c",
				"pysysdc/sd_python_runner.c"
			],
			libraries=['systemd'],
			extra_compile_args=["-std=c99", "-O3", "-Werror=implicit-function-declaration", "-D_SVID_SOURCE", "-D_DEFAULT_SOURCE", "-D_LIBSYSTEMD_VERSION=%s" % libsystemd_ver] # match systemd
		)
	],
	scripts=['scripts/dcbus'],
	data_files=[("/etc/dbus-1/",
		["etc/dbus-1/system-local.conf"])]
)
