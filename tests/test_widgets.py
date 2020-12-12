"""
Author: RedFantom
License: GNU GPLv3
Copyright (c) 2017-2018 RedFantom
"""
import sys
from unittest import TestCase
import tkinter as tk
from tkinter import ttk
import gttk


def printf(string, end="\n"):
    sys.__stdout__.write(string + end)
    sys.__stdout__.flush()


class TestThemedWidgets(TestCase):
    """
    Tkinter may crash if widgets are not configured properly in a theme.
    Therefore, in addition to checking if all files for a theme exist
    by loading it, this Test also tests every core ttk widget to see
    if the widget can be successfully created with the theme data.

    When Tkinter crashes, it keeps the Global Interpreter Lock in place,
    so the program actually has to be terminated with SIGTERM.
    Therefore, this test only executes on UNIX.
    """
    WIDGETS = [
        "Label",
        "Treeview",
        "Button",
        "Frame",
        "Notebook",
        "Progressbar",
        "Scrollbar",
        "Scale",
        "Entry",
        "Combobox"
    ]

    def setUp(self):
        self.window = tk.Tk()
        self.style = ttk.Style()

    def test_widget_creation(self):
        try:
            import signal
        except ImportError:
            pass
        signal_available = "signal" in locals() and hasattr(locals()["signal"], "alarm")
        theme = "gttk"
        self.style.theme_use(theme)
        for widget in self.WIDGETS:
            if signal_available:
                signal.alarm(5)
            printf("Testing {}: {}".format(theme, widget), end=" - ")
            getattr(ttk, widget)(self.window).pack()
            self.window.update()
            if signal_available:
                signal.alarm(0)
            printf("SUCCESS")

    def tearDown(self):
        self.window.destroy()
