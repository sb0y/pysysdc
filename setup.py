#!/usr/bin/env python3

from setuptools import setup, Extension
import re

VERSION = '1.0.0'
with open('pysysdc/__version__.py', 'r') as f:
	VERSION = f.read()

VERSION = re.sub(r'[^0-9.]+', '', VERSION)

setup(
	name="pysysdc",
	version=VERSION,
	description="Python bindings for sd-bus",
	long_description='',
	include_package_data=True,
	author_email="andrey@bagrintsev.me",
    author=[
		"Andrey Bagrintsev <andrey@bagrintsev.me>"
	],
	url="https://github.com/sb0y/",
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
				"pysysdc/connect.c"
			],
			libraries=['systemd'],
			#extra_compile_args=["-O3", "-std=c99"] # match systemd
		)
	],
	scripts=['scripts/dcbus'],
	data_files=[("/etc/dbus-1/",
		["etc/dbus-1/system-local.conf"])]
)
