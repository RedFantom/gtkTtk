"""
Author: RedFantom
License: GNU GPLv3
Copyright (c) 2020 RedFantom
"""
import contextlib
import os
import tkinter as tk
from tkinter import ttk
from typing import List, Tuple


@contextlib.contextmanager
def chdir(target):
    cwd = os.getcwd()
    try:
        os.chdir(target)
        yield
    finally:
        os.chdir(cwd)


class GTTK(object):
    """
    Class representing the GTTK extension
    """

    def __init__(self, window: tk.Tk):
        """
        Initialize gttk and load it into a window

        :param window: Window with Tk/Tcl interpreter to load gttk for
        """
        self.tk = window.tk
        folder = os.path.dirname(os.path.abspath(__file__)).replace("\\", "/")
        with chdir(folder):
            self.tk.eval("set dir {0}; source {0}/pkgIndex.tcl".format(folder))
            self.tk.call("package", "require", "ttk::theme::gttk")
        print(self.get_current_theme())

    def get_themes_directory(self) -> str:
        """Return the directory in which GTK looks for installed themes"""
        return self.tk.call("ttk::theme::gttk::gtkDirectory", "theme")

    def get_default_files(self) -> Tuple[str]:
        """Return the files that GTK parses by default at start-up"""
        return self.tk.call("ttk::theme::gttk::gtkDirectory", "default_files")

    def get_current_theme(self) -> str:
        """Return the name of the currently active GTK theme"""
        return self.tk.call("ttk::theme::gttk::currentThemeName")

    def get_module_path(self) -> str:
        """Return the name of the module path (theme engines)"""
        return self.tk.call("ttk::theme::gttk::gtkDirectory", "module")

    def get_gtk_enum_value(self, value: int) -> str:
        """
        Return a value of a the GtkPositionType enum

        TODO: Extend gttk_GtkEnum in gttk_Init.cpp for more enums
        """
        return self.tk.call("ttk::theme::gttk::gtkEnum", "GtkPositionType", 0)

    def get_theme_colour(self, name: str) -> str:
        return self.tk.call("ttk::theme::gttk::currentThemeColour", name)

    def get_theme_colour_keys(self) -> Tuple[str]:
        return self.tk.call("ttk::theme::gttk::currentThemeColourKeys")
