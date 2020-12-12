/*
 *  gttk_GtkApp.c
 * ---------------
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

#ifdef GTTK_ENABLE_GNOME
#include <gnome.h>
GnomeProgram *my_app = NULL;
#endif /* GTTK_ENABLE_GNOME */

#include "gttk_Utilities.h"
#include "gttk_TkHeaders.h"
#include <string.h>
gboolean   gttk_GtkInitialisedFlag = FALSE;
static int gttk_xlib_rgb_initialised = 0;
GtkWidget *gttk_GtkWindow = NULL;



/* In the following variable we store the XErrorHandler, before we install our
 * own, which filters out some XErrors... */
static int (*gttk_TkXErrorHandler)(Display *displayPtr,
                                     XErrorEvent *errorPtr);
static int (*gttk_GtkXErrorHandler)(Display *displayPtr,
                                     XErrorEvent *errorPtr);
static int gttk_XErrorHandler(Display *displayPtr, XErrorEvent *errorPtr);

static int  gttk_XEventHandler(ClientData clientdata, XEvent *eventPtr);

/*
 * gttk_InterpDeleteProc:
 * This function will be called when the interpreter gets deleted. It must free
 * all allocated interp-specific memory segments.
 */
static void gttk_InterpDeleteProc(ClientData clientData, Tcl_Interp *interp) {
  gttk_WidgetCache **wc_array = (gttk_WidgetCache **) clientData;
  gttk_WidgetCache *wc = wc_array[0];
  if (wc && wc->gtkWindow) {
    /*This will destroy also ALL children!*/
    gttk_gtk_widget_destroy(wc->gtkWindow);
  }
  // printf("Tk_DeleteGenericHandler: %p\n", interp); fflush(NULL);
  Tk_DeleteGenericHandler(&gttk_XEventHandler, (ClientData) interp);
  Tcl_Free((char *) wc_array[0]);
  Tcl_Free((char *) wc_array[1]);
  Tcl_Free((char *) wc_array);
}; /* gttk_InterpDeleteProc */

gttk_WidgetCache **gttk_CreateGtkApp(Tcl_Interp *interp) {
  /*
   * The first step is to initialise the gtk library. This must be done once
   * in the application lifetime.
   */
  Tcl_MutexLock(&gttkMutex);
  if (!gttk_GtkInitialisedFlag) {
#ifdef GTTK_ENABLE_GNOME
    gchar **remaining_args = NULL;
    GOptionEntry option_entries[] = {
      {G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_FILENAME_ARRAY, &remaining_args,
       "Special option that collects any remaining arguments for us"},
      { NULL }
    };
    GOptionContext *option_context;
#endif /* GTTK_ENABLE_GNOME */
    int argc = 1;
    char **argv = gttk_g_new0(char*, 2);
    argv[0] = (char *) Tcl_GetNameOfExecutable();

#ifdef GTTK_INSTALL_XERROR_HANDLER
    gttk_TkXErrorHandler = XSetErrorHandler(gttk_XErrorHandler);
#endif /* GTTK_INSTALL_XERROR_HANDLER */

#ifdef GTTK_ENABLE_GNOME
    option_context = gttk_g_option_context_new("tile-gtk");
    gttk_g_option_context_add_main_entries(option_context,
                                              option_entries, NULL);
    /* We assume PACKAGE and VERSION are set to the program name and version
     * number respectively. Also, assume that 'option_entries' is a global
     * array of GOptionEntry structures.
     */
    my_app = gnome_program_init(PACKAGE_NAME, PACKAGE_VERSION,
                                LIBGNOMEUI_MODULE, argc, argv,
                                GNOME_PARAM_GOPTION_CONTEXT, option_context,
                                GNOME_PARAM_NONE);
    if (my_app) gttk_GtkInitialisedFlag = TRUE;
    if (remaining_args != NULL) {
      gttk_g_strfreev(remaining_args);
      remaining_args = NULL;
    }
#else  /* GTTK_ENABLE_GNOME */
    gttk_gtk_disable_setlocale();
    gttk_GtkInitialisedFlag = gttk_gtk_init_check(&argc, &argv);
#endif /* GTTK_ENABLE_GNOME */
    gttk_g_free(argv);
    if (!gttk_GtkInitialisedFlag) {
      Tcl_MutexUnlock(&gttkMutex);
      return NULL;
    }
    /* Initialise gttk_GtkWindow... */
    gttk_GtkWindow = gttk_gtk_window_new(GTK_WINDOW_POPUP);
    gttk_gtk_widget_realize(gttk_GtkWindow);
#ifdef GTTK_INSTALL_XERROR_HANDLER
    /*
     * GTK+ xerror handler will terminate the application.
     * Just get rid of that...
     */
    gttk_GtkXErrorHandler = XSetErrorHandler(gttk_XErrorHandler);
#endif /* GTTK_INSTALL_XERROR_HANDLER */

#ifdef GTTK_SYNCHRONIZE
    XSynchronize(Tk_Display(Tk_MainWindow(interp)), true);
#endif /* GTTK_SYNCHRONIZE */
  }
  Tcl_MutexUnlock(&gttkMutex);

  /*
   * Allocate the widget cache. We keep a widget cache per interpreter.
   * Each cache is an array of two elements, one for each orientation.
   */

  gttk_WidgetCache **wc_array = (gttk_WidgetCache **)
                           Tcl_Alloc(sizeof(gttk_WidgetCache*)*2);
  wc_array[0] = (gttk_WidgetCache *)
                           Tcl_Alloc(sizeof(gttk_WidgetCache));
  wc_array[1] = (gttk_WidgetCache *)
                           Tcl_Alloc(sizeof(gttk_WidgetCache));
  Tcl_SetAssocData(interp, "gttkTtk_gtk_widget_cache",
                   &gttk_InterpDeleteProc, (ClientData) wc_array);
  gttk_WidgetCache *wc = wc_array[0];
  memset(wc, 0, sizeof(gttk_WidgetCache));
  /*
   * Initialise the widget cache.
   */
  wc->gttk_MainInterp  = interp;
  wc->gttk_tkwin       = Tk_MainWindow(interp);
  if (wc->gttk_tkwin != NULL && wc->gttk_MainDisplay == NULL) {
    Tk_MakeWindowExist(wc->gttk_tkwin);
    wc->gttk_MainDisplay = Tk_Display(wc->gttk_tkwin);
  }
  if (wc->gttk_MainDisplay == NULL) {
    Tcl_MutexUnlock(&gttkMutex);
    Tcl_Free((char *) wc_array[0]);
    Tcl_Free((char *) wc_array[1]);
    Tcl_Free((char *) wc_array);
    return NULL;
  }
#ifndef __WIN32__
  wc->gdkDisplay = gttk_gdk_x11_lookup_xdisplay(wc->gttk_MainDisplay);
#endif
  if (!wc->gdkDisplay) {
    wc->gdkDisplay = gttk_gdk_display_get_default();
  }
  wc->gtkWindow = gttk_gtk_window_new(GTK_WINDOW_POPUP);
  if (wc->gtkWindow) gttk_gtk_widget_realize(wc->gtkWindow);
  wc->protoLayout = gttk_gtk_fixed_new();
  gttk_gtk_container_add((GtkContainer*)(wc->gtkWindow), wc->protoLayout);
  memcpy(wc_array[1], wc_array[0], sizeof(gttk_WidgetCache));
  wc_array[0]->orientation    = TTK_ORIENT_HORIZONTAL;
  wc_array[1]->orientation    = TTK_ORIENT_VERTICAL;
  wc_array[0]->gtkOrientation = GTK_ORIENTATION_HORIZONTAL;
  wc_array[1]->gtkOrientation = GTK_ORIENTATION_VERTICAL;

#ifndef __WIN32__
  Tcl_MutexLock(&gttkMutex);
  if (!gttk_xlib_rgb_initialised) {
    gttk_xlib_rgb_init(wc->gttk_MainDisplay,Tk_Screen(wc->gttk_tkwin));
    gttk_xlib_rgb_initialised = 1;
  }
  Tcl_MutexUnlock(&gttkMutex);
#endif
  return wc_array;
}; /* gttk_CreateGtkApp */

void gttk_DestroyGtkApp(void) {
  Tcl_MutexLock(&gttkMutex);
  if (gttk_GtkInitialisedFlag) {
    // XSetErrorHandler(gttk_TkXErrorHandler);
    gttk_GtkInitialisedFlag = FALSE;
  }
  Tcl_MutexUnlock(&gttkMutex);
}; /* gttk_DestroyGtkApp */

/*
 * gttk_XErrorHandler:
 * This XError handler just prints some debug information and then calls
 * Tk's XError handler...
 */
static int gttk_XErrorHandler(Display *displayPtr, XErrorEvent *errorPtr) {
#ifdef GTTK_VERBOSE_XERROR_HANDLER
  char buf[64];
  XGetErrorText (displayPtr, errorPtr->error_code, buf, 63);
  printf("===============================================================\n");
  printf("  gttk_XErrorHandler:\n");
  printf("    error_code   = %s (%d)\n", buf, errorPtr->error_code);
  printf("    request_code = %d\n", errorPtr->request_code);
  printf("    minor_code   = %d\n", errorPtr->minor_code);
  printf("===============================================================\n");
#endif /* GTTK_VERBOSE_XERROR_HANDLER */
  return gttk_TkXErrorHandler(displayPtr, errorPtr);
}; /* gttk_XErrorHandler */

static int gttk_XEventHandler(ClientData clientData, XEvent *eventPtr) {
  const char *tcl_callback;
  int status;
  if (eventPtr->type != ClientMessage) return 0;
  // Atom gttk_KIPC_COMM_ATOM = XInternAtom(eventPtr->xclient.display,
  //                                          "KIPC_COMM_ATOM" , false);
  Atom gttk_KIPC_COMM_ATOM = None;
  if (eventPtr->xclient.message_type != gttk_KIPC_COMM_ATOM) return 0;
  /* The following data variable contains the type of the KIPC message,
   * As defined in gnomelibs/gnomecore/kipc.h:
   * PaletteChanged      = 0
   * StyleChanged        = 2
   * ToolbarStyleChanged = 6
   */
  switch (eventPtr->xclient.data.l[0]) {
    case 0:   /* PaletteChanged      */
      tcl_callback = "tile::theme::gttk::gnomePaletteChangeNotification";
      break;
    case 2:   /* StyleChanged        */
    case 6: { /* ToolbarStyleChanged */
      tcl_callback = "tile::theme::gttk::gnomeStyleChangeNotification";
      break;
    }
    default: {
      return 0;
    }
  }
  Tcl_Interp *interp = (Tcl_Interp *) clientData;
  if (interp == NULL) return 0;
  // printf("gttk_XEventHandler: %p\n", interp); fflush(NULL);
  /* Notify the tile engine about the change... */
  status = Tcl_Eval(interp, tcl_callback);
  if (status != TCL_OK) Tcl_BackgroundError(interp);
  /* Do not remove True: As many interpreters may have registered this event
   * handler, allow Tk to call all of them! */
  return True;
} /* gttk_XEventHandler */
