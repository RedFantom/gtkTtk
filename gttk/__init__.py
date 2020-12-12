"""
Author: RedFantom
License: GNU GPLv3
Copyright (c) 2020 RedFantom
"""
import contextlib
import os
from tkinter import ttk


@contextlib.contextmanager
def chdir(target):
    cwd = os.getcwd()
    try:
        os.chdir(target)
        yield
    finally:
        os.chdir(cwd)

__orig_ttk_Style_init__ = ttk.Style.__init__


def __ttk_Style_init__(self, *args, **kwargs):
    __orig_ttk_Style_init__(self, *args, **kwargs)

    folder = os.path.dirname(os.path.abspath(__file__)).replace("\\", "/")

    with chdir(folder):
        self.tk.eval("set dir {0}; source {0}/pkgIndex.tcl".format(folder))
        self.tk.call("package", "require", "ttk::theme::gttk")


ttk.Style.__init__ = __ttk_Style_init__
