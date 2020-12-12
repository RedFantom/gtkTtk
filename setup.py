"""
Author: RedFantom
License: GNU GPLv3
Copyright (c) 2020 RedFantom
"""
import shutil
try:
    from skbuild import setup
    from skbuild.command.build import build
except ImportError:
    print("scikit-build is required to build this project")
    print("install with `python -m pip install scikit-build`")
    raise
import sys


def read(file_name):
    with open(file_name) as fi:
        contents = fi.read()
    return contents


class BuildCommand(build):
    """
    Intercept the build command to build the required modules in ./build

    gttk depends on a library built from source. Building this library
    requires the following to be installed, Ubuntu package names:
    - libx11-dev
    - libgtk2.0-dev
    - libgdk-pixbuf2.0-dev
    - tcl-dev
    - tk-dev
    """

    def run(self):
        if "linux" in sys.platform:
            build.run(self)
        elif "win" in sys.platform:
            print("Running setup on Windows: Assuming that libgttk.dll has been built and is in working directory")
            print("If libgttk.dll has not been built: It should be built with MSYS")
            shutil.copy("libgttk.dll", "gttk/libgttk.dll")
            shutil.copy("library/gttk.tcl", "gttk/gttk.tcl")
            shutil.copy("library/pkgIndex.tcl", "gttk/pkgIndex.tcl")


setup(
    name="gttk",
    version="0.1.0",
    packages=["gttk"],
    description="GTK theme for Tkinter/ttk",
    author="The gttk/tile-gtk/gttk authors",
    url="https://github.com/RedFantom/python-gttk",
    download_url="https://github.com/RedFantom/python-gttk/releases",
    license="GNU GPLv3",
    long_description=read("README.md"),
    zip_safe=False,
    install_requires=["scikit-build"],
    cmdclass={"build": BuildCommand}
)
