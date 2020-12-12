/*
 *  gttk_Utilities.cpp
 * ----------------------
 *
 * This file is part of the gttk package, a Tk/Tile based theme that uses
 * Gtk/GNOME for drawing.
 *
 * Copyright (C) 2004-2008 by:
 * Georgios Petasis, petasis@iit.demokritos.gr,
 * Software and Knowledge Engineering Laboratory,
 * Institute of Informatics and Telecommunications,
 * National Centre for Scientific Research (NCSR) "Demokritos",
 * Aghia Paraskevi, 153 10, Athens, Greece.
 */

#include "gttk_GtkHeaders.h"
#include "gttk_TkHeaders.h"
extern gboolean   gttk_GtkInitialisedFlag;
extern GtkWidget *gttk_GtkWindow;

int gttk_GtkInitialised(void) {
  return gttk_GtkInitialisedFlag;
}; /* gttk_GtkInitialised */

GtkWidget *gttk_GetGtkWindow(void) {
  return gttk_GtkWindow;
}; /* gttk_GetGtkWindow */

GtkStyle *gttk_GetGtkWindowStyle(GtkWidget *gtkWindow) {
  if (gtkWindow) {
    return gtkWindow->style;
  }
  return NULL;
}; /* gttk_GetGtkWindowStyle */

GtkStyle *gttk_GetGtkStyle(void) {
  return gttk_GetGtkWindowStyle(gttk_GetGtkWindow());
}; /* gttk_GetGtkStyle */

void gttk_InitialiseGtkWidget(gttk_WidgetCache* wc, GtkWidget* widget) {
  // GtkStyle *style;
  if (!wc || !widget) return;
  if (!wc->protoLayout) {
    wc->protoLayout = gttk_gtk_fixed_new();
    gttk_gtk_container_add((GtkContainer*)(wc->gtkWindow), wc->protoLayout);
  }
  if (!wc->protoLayout) return;
  gttk_gtk_container_add((GtkContainer*)(wc->protoLayout), widget);
  gttk_gtk_widget_realize(widget);
  // style = gttk_gtk_rc_get_style(widget);
  // if (style) gttk_gtk_style_attach(style,
}; /* gttk_InitialiseGtkWidget */

#define GTTK_CHECK_WIDGET(widget, allocator_function) \
  if (!wc) return 0; \
  if (!wc->widget) { \
    wc->widget = allocator_function; \
    gttk_InitialiseGtkWidget(wc, wc->widget); \
    return wc->widget; \
  } \
  return wc->widget;

GtkWidget *gttk_GetArrow(gttk_WidgetCache* wc) {
  GTTK_CHECK_WIDGET(gtkArrow,
                       gttk_gtk_arrow_new(GTK_ARROW_DOWN, GTK_SHADOW_NONE));
}; /* gttk_GetButton */

GtkWidget *gttk_GetButton(gttk_WidgetCache* wc) {
  GTTK_CHECK_WIDGET(gtkButton, gttk_gtk_button_new());
}; /* gttk_GetButton */

GtkWidget *gttk_GetCheckButton(gttk_WidgetCache* wc) {
  GTTK_CHECK_WIDGET(gtkCheckButton, gttk_gtk_check_button_new());
}; /* gttk_GetCheckButton */

GtkWidget *gttk_GetRadioButton(gttk_WidgetCache* wc) {
  return gttk_GetCheckButton(wc);
}; /* gttk_GetRadioButton */

GtkWidget *gttk_GetToolBar(gttk_WidgetCache* wc) {
  GTTK_CHECK_WIDGET(gtkToolBar, gttk_gtk_toolbar_new());
}; /* gttk_GetToolBar */

GtkWidget *gttk_GetToolButton(gttk_WidgetCache* wc) {
  GTTK_CHECK_WIDGET(gtkToolButton, gttk_gtk_button_new());
}; /* gttk_GetToolButton */

GtkWidget *gttk_GetFrame(gttk_WidgetCache* wc) {
  GTTK_CHECK_WIDGET(gtkFrame, gttk_gtk_frame_new(NULL));
}; /* gttk_GetFrame */

GtkWidget *gttk_GetEntry(gttk_WidgetCache* wc) {
  GTTK_CHECK_WIDGET(gtkEntry, gttk_gtk_entry_new());
}; /* gttk_GetEntry */

GtkWidget *gttk_GetCombobox(gttk_WidgetCache* wc) {
  GTTK_CHECK_WIDGET(gtkCombobox, gttk_gtk_combo_box_new());
}; /* gttk_GetComboboxEntry */

GtkWidget *gttk_GetComboboxEntry(gttk_WidgetCache* wc) {
  GTTK_CHECK_WIDGET(gtkComboboxEntry, gttk_gtk_combo_box_entry_new());
}; /* gttk_GetComboboxEntry */

GtkWidget *gttk_GetHScrollBar(gttk_WidgetCache* wc) {
  GtkAdjustment *adjustment = (GtkAdjustment *)
             gttk_gtk_adjustment_new(0.0, 0.0, 1.0, 0, 0, 0);
  GTTK_CHECK_WIDGET(gtkHScrollBar, gttk_gtk_hscrollbar_new(adjustment));
}; /* gttk_GetHScrollBar */

GtkWidget *gttk_GetVScrollBar(gttk_WidgetCache* wc) {
  GtkAdjustment *adjustment = (GtkAdjustment *)
             gttk_gtk_adjustment_new(0.0, 0.0, 1.0, 0, 0, 0);
  GTTK_CHECK_WIDGET(gtkVScrollBar, gttk_gtk_vscrollbar_new(adjustment));
}; /* gttk_GetVScrollBar */

GtkWidget *gttk_GetScrollBar(gttk_WidgetCache* wc) {
  if (wc->orientation == TTK_ORIENT_HORIZONTAL) {
    return gttk_GetHScrollBar(wc);
  }
  return gttk_GetVScrollBar(wc);
}; /* gttk_GetScrollBar */

GtkWidget *gttk_GetHScale(gttk_WidgetCache* wc) {
  GTTK_CHECK_WIDGET(gtkHScale, gttk_gtk_hscale_new_with_range(0,1,0.001));
}; /* gttk_GetHScale */

GtkWidget *gttk_GetVScale(gttk_WidgetCache* wc) {
  GTTK_CHECK_WIDGET(gtkVScale, gttk_gtk_vscale_new_with_range(0,1,0.001));
}; /* gttk_GetVScale */

GtkWidget *gttk_GetScale(gttk_WidgetCache* wc) {
  if (wc->orientation == TTK_ORIENT_HORIZONTAL) {
    return gttk_GetHScale(wc);
  }
  return gttk_GetVScale(wc);
}; /* gttk_GetScale */

GtkWidget *gttk_GetHProgressBar(gttk_WidgetCache* wc) {
  GTTK_CHECK_WIDGET(gtkHProgressBar, gttk_gtk_progress_bar_new());
  gttk_gtk_progress_bar_set_orientation(
     (GtkProgressBar*) wc->gtkHProgressBar, GTK_PROGRESS_LEFT_TO_RIGHT);
  gttk_gtk_progress_bar_set_fraction(
     (GtkProgressBar *) wc->gtkHProgressBar, 1);
}; /* gttk_GetHProgressBar */

GtkWidget *gttk_GetVProgressBar(gttk_WidgetCache* wc) {
  GTTK_CHECK_WIDGET(gtkVProgressBar, gttk_gtk_progress_bar_new());
  gttk_gtk_progress_bar_set_orientation(
     (GtkProgressBar*) wc->gtkVProgressBar, GTK_PROGRESS_BOTTOM_TO_TOP);
  gttk_gtk_progress_bar_set_fraction(
     (GtkProgressBar *) wc->gtkVProgressBar, 1);
}; /* gttk_GetVProgressBar */

GtkWidget *gttk_GetProgressBar(gttk_WidgetCache* wc) {
  if (wc->orientation == TTK_ORIENT_HORIZONTAL) {
    return gttk_GetHProgressBar(wc);
  }
  return gttk_GetVProgressBar(wc);
}; /* gttk_GetProgressBar */

GtkWidget *gttk_GetStatusBar(gttk_WidgetCache* wc) {
  GTTK_CHECK_WIDGET(gtkStatusBar, gttk_gtk_statusbar_new());
}; /* gttk_GetStatusBar */

GtkWidget *gttk_GetHPaned(gttk_WidgetCache* wc) {
  GTTK_CHECK_WIDGET(gtkHPaned, gttk_gtk_hpaned_new());
}; /* gttk_GetHPaned */

GtkWidget *gttk_GetVPaned(gttk_WidgetCache* wc) {
  GTTK_CHECK_WIDGET(gtkVPaned, gttk_gtk_vpaned_new());
}; /* gttk_GetVPaned */

GtkWidget *gttk_GetPaned(gttk_WidgetCache* wc) {
  if (wc->orientation == TTK_ORIENT_HORIZONTAL) {
    return gttk_GetHPaned(wc);
  }
  return gttk_GetVPaned(wc);
}; /* gttk_GetPaned */

GtkWidget *gttk_GetNotebook(gttk_WidgetCache* wc) {
  GTTK_CHECK_WIDGET(gtkNotebook, gttk_gtk_notebook_new());
}; /* gttk_GetNotebook */


const char *gttk_GtkStateStr(GtkStateType gtkState) {
  switch ((GtkStateType) gtkState) {
    case GTK_STATE_NORMAL:      return "GTK_STATE_NORMAL";
    case GTK_STATE_ACTIVE:      return "GTK_STATE_ACTIVE";
    case GTK_STATE_PRELIGHT:    return "GTK_STATE_PRELIGHT";
    case GTK_STATE_SELECTED:    return "GTK_STATE_SELECTED";
    case GTK_STATE_INSENSITIVE: return "GTK_STATE_INSENSITIVE";
  }
  return "invalid!";
}; /* gttk_GtkStateStr */
const char *gttk_GtkShadowStr(GtkShadowType gtkShadow) {
  switch ((GtkShadowType) gtkShadow) {
    case GTK_SHADOW_NONE:       return "GTK_SHADOW_NONE";
    case GTK_SHADOW_IN:         return "GTK_SHADOW_IN";
    case GTK_SHADOW_OUT:        return "GTK_SHADOW_OUT";
    case GTK_SHADOW_ETCHED_IN:  return "GTK_SHADOW_ETCHED_IN";
    case GTK_SHADOW_ETCHED_OUT: return "GTK_SHADOW_ETCHED_OUT";
  }
  return "invalid!";
}; /* gttk_GtkShadowStr */

void gttk_StateInfo(int state, GtkStateType gtkState,
             GtkShadowType gtkShadow, Tk_Window tkwin, GtkWidget *widget)
{
    printf("Widget: %s\n  ", Tk_PathName(tkwin));
    if (state & TTK_STATE_ACTIVE) {
      printf("TTK_STATE_ACTIVE %d ", TTK_STATE_ACTIVE);
    } 
    if (state & TTK_STATE_DISABLED) {
      printf("TTK_STATE_DISABLED %d ", TTK_STATE_DISABLED);
    } 
    if (state & TTK_STATE_FOCUS) {
      printf("TTK_STATE_FOCUS %d ", TTK_STATE_FOCUS);
    } 
    if (state & TTK_STATE_PRESSED) {
      printf("TTK_STATE_PRESSED %d ", TTK_STATE_PRESSED);
    } 
    if (state & TTK_STATE_SELECTED) {
      printf("TTK_STATE_SELECTED %d ", TTK_STATE_SELECTED);
    } 
    if (state & TTK_STATE_BACKGROUND) {
      printf("TTK_STATE_BACKGROUND %d ", TTK_STATE_BACKGROUND);
    } 
    if (state & TTK_STATE_ALTERNATE) {
      printf("TTK_STATE_ALTERNATE %d ", TTK_STATE_ALTERNATE);
    } 
    if (state & TTK_STATE_INVALID) {
      printf("TTK_STATE_INVALID %d ", TTK_STATE_INVALID);
    } 
    if (state & TTK_STATE_READONLY) {
      printf("TTK_STATE_READONLY %d ", TTK_STATE_READONLY);
    } 
    if (state & TTK_STATE_USER1) {
      printf("TTK_STATE_USER1 %d ", TTK_STATE_USER1);
    } 
    if (state & TTK_STATE_USER2) {
      printf("TTK_STATE_USER2 %d ", TTK_STATE_USER2);
    } 
    if (state & TTK_STATE_USER3) {
      printf("TTK_STATE_USER3 %d ", TTK_STATE_USER3);
    } 
    if (state & TTK_STATE_USER4) {
      printf("TTK_STATE_USER4 %d ", TTK_STATE_USER4);
    } 
    if (state & TTK_STATE_USER5) {
      printf("TTK_STATE_USER5 %d ", TTK_STATE_USER5);
    } 
    if (state & TTK_STATE_USER6) {
      printf("TTK_STATE_USER6 %d ", TTK_STATE_USER6);
    } 
#ifdef TTK_STATE_USER7
    if (state & TTK_STATE_USER7) {
      printf("TTK_STATE_USER7 %d ", TTK_STATE_USER7);
    }
#endif /* TTK_STATE_USER7 */
    printf(" state=%d\n", state);
    printf("  GTK state: %s, shadow: %s",
      gttk_GtkStateStr(gtkState), gttk_GtkShadowStr(gtkShadow));
#ifndef GTTK_LOAD_GTK_DYNAMICALLY
    if (widget) {
      printf(", focus: %d, default: %d", GTK_WIDGET_HAS_FOCUS(widget),
                                         GTK_WIDGET_HAS_DEFAULT(widget));
    }
#endif /* GTTK_LOAD_GTK_DYNAMICALLY */
    printf("\n");
    fflush(0);
}; /* gttk_StateInfo */

void gttk_CopyGtkPixmapOnToDrawable(GdkPixmap *gdkDrawable, Drawable d,
            Tk_Window tkwin, int x, int y, int w, int h, int x1, int x2)
{
#ifdef __WIN32__
#define gc_usage ((GdkGCValuesMask) \
    (GDK_GC_FOREGROUND | GDK_GC_BACKGROUND | GDK_GC_EXPOSURES))
    XGCValues gcValues;
    gcValues.graphics_exposures = False;
    GC gc = Tk_GetGC(tkwin, GCForeground | GCBackground | GCGraphicsExposures,
                     &gcValues);
    GdkGC *gdkGC = gttk_gdk_gc_new(gdkDrawable);
    HDC hdcSrc = gdk_win32_hdc_get(gdkDrawable, gdkGC, gc_usage);
    /* Create a Tk Drawable from the HDC... */
    TkWinDrawable gtkD;
    gtkD.type = TWD_WINDC;
    gtkD.winDC.hdc = hdcSrc;
    XCopyArea(Tk_Display(tkwin), (Drawable) &gtkD, d, gc, x, y, w, h, x1, x2);
    gdk_win32_hdc_release(gdkDrawable, gdkGC, gc_usage);
    if (gdkGC) gttk_g_object_unref(gdkGC);
    Tk_FreeGC(Tk_Display(tkwin), gc);
#else

#ifdef GTTK_USE_XCOPY
    XGCValues gcValues;
    gcValues.graphics_exposures = False;
    GC gc = Tk_GetGC(tkwin, GCForeground | GCBackground | GCGraphicsExposures,
                     &gcValues);
    XCopyArea(Tk_Display(tkwin), GDK_DRAWABLE_XID(pixmap), d, gc,
              x, y, w, h, x1, x2);
    Tk_FreeGC(Tk_Display(tkwin), gc);
    XGCValues gcValues;
    GC gc;

    gcValues.function = GXcopy;
    gcValues.graphics_exposures = False;
    gc = Tk_GetGC(tkwin, GCFunction|GCGraphicsExposures, &gcValues);

    XCopyArea(Tk_Display(tkwin), d, Tk_WindowId(tkwin), gc,
            0, 0, (unsigned) Tk_Width(tkwin), (unsigned) Tk_Height(tkwin),
            0, 0);

    Tk_FreePixmap(Tk_Display(tkwin), d);
    Tk_FreeGC(Tk_Display(tkwin), gc);
#else
    GdkPixbuf *imgb;
    XGCValues gcValues;
    gcValues.graphics_exposures = False;
    GC gc = Tk_GetGC(tkwin, GCForeground | GCBackground | GCGraphicsExposures,
                     &gcValues);
    imgb = gttk_gdk_pixbuf_new(GDK_COLORSPACE_RGB, true, 8, w, h);
    if (!imgb) {
      Tk_FreeGC(Tk_Display(tkwin), gc);
      return;
    }
    imgb = gttk_gdk_pixbuf_get_from_drawable(imgb, gdkDrawable,
                                                NULL, x, y, 0, 0, w, h);
    gttk_gdk_pixbuf_xlib_render_to_drawable(imgb, d, gc,
         0, 0, x1, x2, w, h, XLIB_RGB_DITHER_MAX, 0, 0);
    gttk_gdk_pixbuf_unref(imgb);
    Tk_FreeGC(Tk_Display(tkwin), gc);
#endif

/*
 * How to convert an X drawable to a GtkDrawable!
 *
  GdkPixmap    *gdkDrawable = NULL; \
  GtkStyle     *style       = NULL; \
  GdkScreen    *screen      = NULL; \
  GdkColormap  *cmap        = NULL; \
  screen = gttk_gdk_display_get_screen(wc->gdkDisplay, \
                                          Tk_ScreenNumber(tkwin)); \
  printf("Widget: %s, d=%d, p=%p\n", Tk_PathName(tkwin), d, \
          gdk_xid_table_lookup((XID) d));fflush(0); \
  gdkDrawable = gttk_gdk_pixmap_foreign_new_for_screen(screen, d, \
          Tk_Width(tkwin), Tk_Height(tkwin), \
          DefaultDepthOfScreen(Tk_Screen(tkwin))); \
  if (!gdkDrawable) return; \
  GdkVisual *visual = gttk_gdk_x11_screen_lookup_visual(screen,
                              XVisualIDFromVisual(Tk_Visual(tkwin))); \
  cmap = gttk_gdk_x11_colormap_foreign_new(visual, Tk_Colormap(tkwin)); \
  gttk_gdk_drawable_set_colormap(gdkDrawable, cmap);\
  printf("d=%d, gdkDrawable=%p, XID=%d, p=%p\n", d, gdkDrawable, \
          gdk_x11_drawable_get_xid(gdkDrawable), \
          gdk_xid_table_lookup((XID) d));fflush(0);
 */
#endif
}; /* gttk_CopyGtkPixmapOnToDrawable */

void gttk_StoreStyleNameLowers(gttk_WidgetCache *wc) {
}; /* gttk_StoreStyleName */

bool gttk_ThemeIs(gttk_WidgetCache *wc, const char* name) {
  return false;
}; /* gttk_ThemeIs */

/*
 * gttk_StateTableLookup --
 * Look up an index from a statically allocated state table.
 */
unsigned int gttk_StateTableLookup(Ttk_StateTable *map, unsigned int state) {
    return Ttk_StateTableLookup(map, state);
    int value = 0;
    while (map->onBits || map->offBits) {
      if (state & map->onBits) value |= map->index;
      ++map;
    }
    return value;
}; /* gttk_StateTableLookup */

/*
 * GTK_STATE_NORMAL:      State during normal operation.
 * GTK_STATE_ACTIVE:      State of a currently active widget, such as a
 *                        depressed button.
 * GTK_STATE_PRELIGHT:    State indicating that the mouse pointer is over the
 *                        widget and the widget will respond to mouse clicks.
 * GTK_STATE_SELECTED:    State of a selected item, such the selected row in
 *                        a list.
 * GTK_STATE_INSENSITIVE: State indicating that the widget is unresponsive
 *                        to user actions.
 *
 * GTK_SHADOW_NONE:       No outline.
 * GTK_SHADOW_IN:         The outline is bevelled inwards.
 * GTK_SHADOW_OUT:        The outline is bevelled outwards like a button.
 * GTK_SHADOW_ETCHED_IN:  The outline has a sunken 3d appearance.
 * GTK_SHADOW_ETCHED_OUT: The outline has a raised 3d appearance
 *
 * TTK_STATE_ACTIVE
 * TTK_STATE_DISABLED
 * TTK_STATE_FOCUS
 * TTK_STATE_PRESSED
 * TTK_STATE_SELECTED
 * TTK_STATE_BACKGROUND
 * TTK_STATE_ALTERNATE
 * TTK_STATE_INVALID
 * TTK_STATE_READONLY
 */
unsigned int gttk_StateShadowTableLookup(gttk_StateTable *map,
             unsigned int state, GtkStateType& gtkState,
             GtkShadowType& gtkShadow, unsigned int section) {
  static gttk_StateTable default_map[] = {
    /* Section for: Buttons */
    /* Section common to all widgets */
    {GTK_STATE_ACTIVE,      GTK_SHADOW_NONE, 0,
     TTK_STATE_ACTIVE,      0,               GTTK_SECTION_ALL},
    {GTK_STATE_INSENSITIVE, GTK_SHADOW_NONE, 0,
     TTK_STATE_DISABLED,    0,               GTTK_SECTION_ALL},
    {GTK_STATE_PRELIGHT,    GTK_SHADOW_NONE, 0,
     TTK_STATE_FOCUS,       0,               GTTK_SECTION_ALL},
    {GTK_STATE_SELECTED,    GTK_SHADOW_NONE, 0,
     TTK_STATE_PRESSED,     0,               GTTK_SECTION_ALL},
    {GTK_STATE_SELECTED,    GTK_SHADOW_NONE, 0,
     TTK_STATE_SELECTED,    0,               GTTK_SECTION_ALL},
    {GTK_STATE_NORMAL,      GTK_SHADOW_NONE, 0,
     TTK_STATE_BACKGROUND,  0,               GTTK_SECTION_ALL},
    {GTK_STATE_NORMAL,      GTK_SHADOW_NONE, 0,
     TTK_STATE_ALTERNATE,   0,               GTTK_SECTION_ALL},
    {GTK_STATE_INSENSITIVE, GTK_SHADOW_NONE, 0,
     TTK_STATE_INVALID,     0,               GTTK_SECTION_ALL},
    {GTK_STATE_INSENSITIVE, GTK_SHADOW_NONE, 0,
     TTK_STATE_READONLY,    0,               GTTK_SECTION_ALL},
    {GTK_STATE_NORMAL,      GTK_SHADOW_NONE, 0,
     0,                     0,               GTTK_SECTION_ALL}
  };
  unsigned int value = 0;
  gtkState  = GTK_STATE_NORMAL; 
  gtkShadow = GTK_SHADOW_NONE;
  if (!map) {
    map = default_map;
    /* Instead of writting huge tables, do some checks here... */
    if (section & GTTK_SECTION_STEPPERS) {
      gtkShadow = GTK_SHADOW_OUT;
      if (state & TTK_STATE_DISABLED) {
        gtkState = GTK_STATE_INSENSITIVE;
      } else if (state & TTK_STATE_PRESSED) {
        gtkState = GTK_STATE_ACTIVE;
        gtkShadow = GTK_SHADOW_IN;
      } else if (state & TTK_STATE_ACTIVE) {
        gtkState = GTK_STATE_PRELIGHT;
      }
      map = NULL; /* Do not search the table */
    } else if (section & GTTK_SECTION_BUTTONS ||
               section & GTTK_SECTION_ENTRY ||
               section & GTTK_SECTION_SCROLLBAR) {
      /* Whether the button is drawn pressed or not, is defined by shadow. */
      if (state & TTK_STATE_PRESSED || state & TTK_STATE_SELECTED) {
        gtkShadow = GTK_SHADOW_IN;
      } else {
        gtkShadow = GTK_SHADOW_OUT;
        if (state & TTK_STATE_DISABLED || state & TTK_STATE_READONLY)
            gtkState  = GTK_STATE_INSENSITIVE;
        // else if (state & TTK_STATE_ACTIVE) gtkState  = GTK_STATE_PRELIGHT;
        // else if (state & TTK_STATE_FOCUS)  gtkState  = GTK_STATE_ACTIVE;
      }
      if ((section & GTTK_SECTION_ENTRY) && (state & TTK_STATE_FOCUS))
        gtkState = GTK_STATE_ACTIVE;
      if ((state & TTK_STATE_ACTIVE) &&
             (!(state & TTK_STATE_PRESSED) && !(state & TTK_STATE_SELECTED))) {
        gtkState = GTK_STATE_PRELIGHT;
      } else {
        if (!(section & GTTK_SECTION_SCROLLBAR) &&
	    state & TTK_STATE_DISABLED || state & TTK_STATE_READONLY)
          gtkState  = GTK_STATE_INSENSITIVE;
        else if (state & TTK_STATE_PRESSED) gtkState  = GTK_STATE_ACTIVE;
        else if (state & TTK_STATE_DISABLED)
          gtkState  = GTK_STATE_INSENSITIVE;
	}
      map = NULL; /* Do not search the table */
    } else if (section & GTTK_SECTION_TROUGH) {
      if (state & TTK_STATE_PRESSED) {
        gtkState = GTK_STATE_ACTIVE;
      } else {
        gtkState = GTK_STATE_INSENSITIVE;
      }
      gtkShadow = GTK_SHADOW_IN;
      map = NULL; /* Do not search the table */
    } else if (section & GTTK_SECTION_SASH) {
      if (state & TTK_STATE_DISABLED || state & TTK_STATE_READONLY)
                                         gtkState  = GTK_STATE_INSENSITIVE;
      else if (state & TTK_STATE_FOCUS)  gtkState  = GTK_STATE_SELECTED;
      else if (state & TTK_STATE_ACTIVE) gtkState  = GTK_STATE_PRELIGHT;
      map = NULL; /* Do not search the table */
    } else if (section & GTTK_SECTION_TABS) {
      gtkState  = GTK_STATE_ACTIVE;
      gtkShadow = GTK_SHADOW_OUT;
      if (state & TTK_STATE_DISABLED || state & TTK_STATE_READONLY)
                                            gtkState  = GTK_STATE_INSENSITIVE;
      else if (state & TTK_STATE_SELECTED)  gtkState  = GTK_STATE_NORMAL;
      else if (state & TTK_STATE_ACTIVE)    gtkState  = GTK_STATE_PRELIGHT;
      else if (state & TTK_STATE_FOCUS)     gtkState  = GTK_STATE_ACTIVE;
      map = NULL; /* Do not search the table */
    }
  }
  if (map) {
    while ( (state & map->onBits)  != map->onBits  ||
           (~state & map->offBits) != map->offBits ||
           !(section & map->section)) {
      ++map; ++value;
    }
    gtkState  = map->state;
    gtkShadow = map->shadow;
  }
  return (map)? map->value : value;
}; /* gttk_StateShadowTableLookup */

double gttk_ValueFromSlider(gttk_WidgetCache *wc, Tk_Window tkwin,
                               Ttk_Box b) {
  double value = 0.0;
  double ww = Tk_Width(tkwin), wh = Tk_Height(tkwin), dx = 0.0, x;
  if (wc->orientation == TTK_ORIENT_HORIZONTAL) {
    dx = b.width / 2.0;
    x = b.x + dx;
    value = x / ww;
  } else {
    dx = b.height / 2.0;
    x = b.y + dx;
    value = x / wh;
  }
  if (value < 0.0) value = 0.0;
  if (value > 1.0) value = 1.0;
  // printf("ww:%f, b.x:%d, x:%f, dx:%f, v:%f\n", ww, b.x, x, dx, value);
  return value;
}; /* gttk_ValueFromSlider */
