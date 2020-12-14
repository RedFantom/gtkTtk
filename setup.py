"""
Author: RedFantom
License: GNU GPLv3
Copyright (c) 2020 RedFantom
"""
import sys


def read(file_name):
    with open(file_name) as fi:
        contents = fi.read()
    return contents


if "linux" in sys.platform:
    try:
        from skbuild import setup
        from skbuild.command.build import build
    except ImportError:
        print("scikit-build is required to build this project")
        print("install with `python -m pip install scikit-build`")
        raise


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
            build.run(self)

    kwargs = {"install_requires": ["scikit-build"], "cmdClass": {"build": BuildCommand}}

elif "win" in sys.platform:
    import os
    import shutil
    from setuptools import setup
    import subprocess as sp
    from typing import List, Optional

    print("Running setup on Windows: Assuming that libgttk.dll has been built and is in working directory")
    print("If libgttk.dll has not been built: It should be built with MSYS")
    shutil.copy("libgttk.dll", "gttk\\libgttk.dll")
    shutil.copy("library\\gttk.tcl", "gttk\\gttk.tcl")
    shutil.copy("library\\pkgIndex.tcl", "gttk\\pkgIndex.tcl")


    class DependencyWalker(object):
        """
        Walk the dependencies of a DLL file and find all DLL files

        DLL files are searched for in all the directories specified by
        - The PATH environment variable
        - The DLL_SEARCH_PATHS environment variable
        """

        def __init__(self, dll_file: str, dependencies_exe="deps\\dependencies.exe"):
            if not os.path.exists(dependencies_exe):
                print("dependencies.exe is required to find all dependency DLLs")
                raise FileNotFoundError("Invalid path specified for dependencies.exe")
            self._exe = dependencies_exe
            if not os.path.exists(dll_file):
                raise FileNotFoundError("'{}' does not specify a valid path to first file".format(dll_file))
            self._dll_file = dll_file
            self._dll_cache = {}

        @property
        def dependency_dll_files(self) -> List[str]:
            """Return a list of abspaths to the dependency DLL files"""
            print("Walking dependencies of {}".format(self._dll_file))
            dlls = [self._dll_file]
            done = []
            while set(dlls) != set(done):  # As long as not all dlls are done, keep searching
                for dll in set(dlls) - set(done):  # Go only over not-yet done DLLs
                    print("Looking for dependencies of {}".format(dll))
                    p = sp.Popen([self._exe, "-imports", dll], stdout=sp.PIPE)
                    stdout, stderr = p.communicate()
                    new_dlls = self._parse_dependencies_output(stdout)
                    for new_dll in new_dlls:
                        p = self._find_dll_abs_path(new_dll)
                        if p is None:
                            continue
                        elif "system32" in p:
                            continue
                        elif p not in dlls:
                            dlls.append(p)
                    done.append(dll)
            return list(set(dlls))

        @staticmethod
        def _parse_dependencies_output(output: bytes) -> List[str]:
            """Parse the output of the dependencies.exe command"""
            dlls: List[str] = list()
            for line in map(str.strip, output.decode().split("\n")):
                if not line.startswith("Import from module"):
                    continue
                line = line[len("Import from module"):].strip(":").strip()
                dlls.append(line)
            return dlls

        def _find_dll_abs_path(self, dll_name: str) -> Optional[str]:
            """Find the absolute path of a specific DLL file specified"""
            if dll_name in self._dll_cache:
                return self._dll_cache[dll_name]
            print("Looking for path of {}...".format(dll_name), end="")
            for var in ("PATH", "DLL_SEARCH_DIRECTORIES"):
                print(".", end="")
                val = os.environ.get(var, "")
                for dir in val.split(";"):
                    if not os.path.exists(dir) and os.path.isdir(dir):
                        continue
                    for dirpath, subdirs, files in os.walk(dir):
                        if dll_name in files:
                            p = os.path.join(dirpath, dll_name)
                            print(" Found: {}".format(p))
                            self._dll_cache[dll_name] = p
                            return p
            print("Not found.")
            self._dll_cache[dll_name] = None
            return None

    for p in DependencyWalker("libgttk.dll").dependency_dll_files:
        print("Copying {}".format(p))
        shutil.copyfile(p, os.path.join("gttk", os.path.basename(p)))

    kwargs = {"package_data": {"gttk": ["*.dll", "pkgIndex.tcl", "gttk.tcl"]}}

else:
    print("Only Linux and Windows are currently supported by the build system")
    print("If you wish to help design a build method for your OS, please")
    print("contact the project author.")
    raise RuntimeError("Unsupported platform")

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
    **kwargs
)
