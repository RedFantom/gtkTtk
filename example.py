"""
Author: RedFantom
License: GNU GPLv3
Copyright (c) 2018-2020 RedFantom
"""
import os
import sys
import tkinter as tk
from tkinter import ttk


class Example(tk.Tk):
    """Example that is used to create screenshots for new themes"""
    def __init__(self):
        tk.Tk.__init__(self)
        # Create widgets
        self.menu = tk.Menu(self, tearoff=False)
        self.sub_menu = tk.Menu(self.menu, tearoff=False)
        self.sub_menu.add_command(label="Exit", command=self.destroy)
        self.menu.add_cascade(menu=self.sub_menu, label="General")
        self.config(menu=self.menu)
        self.label = ttk.Label(self, text="This is an example label.")
        self.dropdown = ttk.OptionMenu(self, tk.StringVar(), "First value", "Second Value")
        self.entry = ttk.Entry(self, textvariable=tk.StringVar(value="Default entry value."))
        self.button = ttk.Button(self, text="Button")
        self.radio_one = ttk.Radiobutton(self, text="Radio one", value=True)
        self.radio_two = ttk.Radiobutton(self, text="Radio two", value=False)
        self.scroll = ttk.Scrollbar(self, orient=tk.VERTICAL)
        self.checked = ttk.Checkbutton(self, text="Checked", variable=tk.BooleanVar(value=True))
        self.unchecked = ttk.Checkbutton(self, text="Unchecked")
        self.tree = ttk.Treeview(self, height=4, show=("tree", "headings"))
        self.setup_tree()
        self.progress = ttk.Progressbar(self, maximum=100, value=50)
        self.bind("<F10>", self.screenshot)
        # Grid widgets
        self.grid_widgets()

    def setup_tree(self):
        """Setup an example Treeview"""
        self.tree.insert("", tk.END, text="Example 1", iid="1")
        self.tree.insert("", tk.END, text="Example 2", iid="2")
        self.tree.insert("2", tk.END, text="Example Child")
        self.tree.heading("#0", text="Example heading")

    def grid_widgets(self):
        """Put widgets in the grid"""
        sticky = {"sticky": "nswe"}
        self.label.grid(row=1, column=1, columnspan=2, **sticky)
        self.dropdown.grid(row=2, column=1, **sticky)
        self.entry.grid(row=2, column=2, **sticky)
        self.button.grid(row=3, column=1, columnspan=2, **sticky)
        self.radio_one.grid(row=4, column=1, **sticky)
        self.radio_two.grid(row=4, column=2, **sticky)
        self.checked.grid(row=5, column=1, **sticky)
        self.unchecked.grid(row=5, column=2, **sticky)
        self.scroll.grid(row=1, column=3, rowspan=8, padx=5, **sticky)
        self.tree.grid(row=6, column=1, columnspan=2, **sticky)
        self.progress.grid(row=9, column=1, columnspan=2, padx=5, pady=5, **sticky)

    def screenshot(self, *args):
        """Take a screenshot, crop and save"""
        try:
            from PIL import Image
            from mss import mss
        except ImportError:
            print("Taking a screenshot requires additional packages: 'pillow' and 'mss'")
            raise
        if not os.path.exists("screenshots"):
            os.makedirs("screenshots")
        box = {
            "top": self.winfo_y(),
            "left": self.winfo_x(),
            "width": self.winfo_width(),
            "height": self.winfo_height()
        }
        screenshot = mss().grab(box)
        screenshot = Image.frombytes("RGB", screenshot.size, screenshot.rgb)
        screenshot.save("screenshots/{}.png".format(ttk.Style(self).theme_use()))


if __name__ == '__main__':
    sys.path = sys.path[2:]
    from gttk import GTTK

    window = Example()
    gttk = GTTK(window)
    style = ttk.Style(window)

    style.theme_use("gttk")

    print(gttk.get_theme_colour_keys())

    window.mainloop()
