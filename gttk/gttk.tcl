namespace eval ttk::theme::gttk {
  variable PreviewInterp {}
  variable System
  variable StyleToRc

  proc loadLibraries {args} {
    if {![initialiseLibrary required]} {return}
    package require platform
    set libs {gdk gdk_pixbuf glib gobject gtk}
    switch -glob [::platform::generic] {
      *_64 {
        set paths {
          /usr/lib64 /usr/lib /opt/gnome/lib /usr/openwin/lib /usr/lib/x86_64-linux-gnu
        }
      }
      default {
        set paths {
          /usr/lib /opt/gnome/lib /usr/openwin/lib
        }
      }
    }
    set ext [info sharedlibextension]
    switch $::tcl_platform(platform) {
      windows {
        set sep \;
        set prefixes {{} lib}
        set names {-win32-2.0* -2.0* 20* -win32* * {}}
      }
      unix {
        set sep :
        set prefixes {lib}
        set names {-x11-2.0 -2.0 20 -x11 {}}
        lappend libs gdk_pixbuf_xlib
      }
      default {}
    }
    if {[info exists ::env(LD_LIBRARY_PATH)]} {
      foreach path [split $::env(LD_LIBRARY_PATH) $sep] {
        set path [string trim $path]
        if {[string length $path] && [file isdirectory $path]} {
          lappend paths $path
        }
      }
    }
    if {[info exists ::env(ProgramFiles)]} {
      foreach path [glob -nocomplain -dir $::env(ProgramFiles) *gtk*] {
        set path $path/bin
        if {[file isdirectory $path]} {
          lappend paths [file normalize $path]
        }
      }
    }
      set loaded 0
      foreach lib $libs {
	  set found 0
	  foreach name $names {
	      foreach prefix $prefixes {
		  foreach path $paths {
		      if {$found} {break}
		      if {![file isdirectory $path]} {continue}
		      foreach file [glob -nocomplain -dir $path -type f \
					${prefix}${lib}${name}$ext] {
			  if {$found} {break}
			  if {[file exists $file]} {
			      if {[catch {initialiseLibrary $lib $file} symbol]} {
				  puts " => ERROR while initialising library $lib from $file : $symbol"
			      } else {
				  incr loaded; set found 1
				  break
			      }
			  };# if {[file exists $file]}
		      };# foreach file ...
		  };# foreach prefix $prefixes
	      };# foreach name $names
	  }
      }
      if {$loaded != [llength $libs]} {
      error "Not all symbols loaded!"
    }
  };# loadLibraries

  proc getSystemInfo {} {
    global env
    variable System

    set System(HOME) {}
    foreach var {HOME} {
      if {[info exists env($var)]} {
        set System($var) [file normalize $env($var)]
        break
      }
    }

    set System(GTK_RC_FILES) {}
    if {[info exists env(GTK_RC_FILES)]} {
      set System(GTK_RC_FILES) [file normalize $env(GTK_RC_FILES)]
    } elseif {[info exists env(HOME)]} {
      foreach rc [glob -nocomplain -dir $System(HOME) -type f .gtkrc*] {
        lappend System(GTK_RC_FILES) $rc
      }
      set System(GTK_RC_FILES) [lsort -dictionary $System(GTK_RC_FILES)]
    }

    set System(TEMP) {}
    foreach var {TEMP TMP temp tmp} {
      if {[info exists env($var)]} {
        set System(TEMP) [file normalize $env($var)]
        break
      }
    } 
    if {![string length $System(TEMP)]} {
      foreach dir {/tmp} {
        if {[file isdirectory $dir] && [file writable $dir]} {
          set System(TEMP) $dir; break
        }
      }
    }

    set System(DEFAULT_FILES) [gtkDirectory default_files]
  };# getSystemInfo

  ## availableStyles_AsReturned:
  #  Returns the available styles...
  proc availableStyles_AsReturned {} {
    variable StyleToRc
    variable System
    set styles {}
    foreach dir [list $System(HOME)/.themes [gtkDirectory theme]] {
      foreach theme [glob -nocomplain -type d -directory $dir *] {
        if {[file exists $theme/gtk-2.0/gtkrc]} {
          set style [file tail $theme]
          lappend styles $style
          set StyleToRc($style) [file normalize $theme/gtk-2.0/gtkrc]
        }
      }
    }
    return $styles
  };# availableStyles_AsReturned

  proc updateLayouts {} {
    ## Variable "theme" should be defined by the C part of the extension.
    variable theme
    variable GtkHScrollbar
    variable GtkVScrollbar
    if {![info exists theme]} {return}
    ttk::style theme use gttk
    # puts "============================================================"
    # puts "Current Gtk Theme: [currentThemeName] ($theme)"
    # puts "Tab alignment:    [getStyleHint   -SH_TabBar_Alignment]"
    # puts "Tab base overlap: [getPixelMetric -PM_TabBarBaseOverlap]"
    # puts "Tab overlap:      [getPixelMetric -PM_TabBarTabOverlap]"
    # foreach sc {SC_ScrollBarAddLine SC_ScrollBarSubLine
    #             SC_ScrollBarAddPage SC_ScrollBarSubPage
    #             SC_ScrollBarFirst   SC_ScrollBarLast
    #             SC_ScrollBarSlider  SC_ScrollBarGroove} {
    #   foreach {x y w h} [getSubControlMetrics -$sc] {break}
    #   puts "$sc: x=$x, y=$y, w=$w, h=$h"
    # }
    ##
    ## Update the scrollbars layouts, according to information retrieved from
    ## the GTK style in use...
    ##
    ## has-backward-stepper:           Display the standard backward arrow
    ##                                 button.
    ## has-forward-stepper:            Display the standard forward arrow
    ##                                 button.
    ## has-secondary-backward-stepper: Display a second backward arrow button
    ##                                 on the opposite end of the scrollbar.
    ## has-secondary-forward-stepper:  Display a second forward arrow button
    ##                                 on the opposite end of the scrollbar.
    foreach widget {GtkHScrollbar GtkVScrollbar} {
      foreach p {has-backward-stepper has-forward-stepper
                 has-secondary-backward-stepper has-secondary-forward-stepper
                 trough-under-steppers trough-side-details} {
        set ${widget}($p) [widgetStyleProperty $widget $p boolean]
      }
      foreach p {arrow-displacement-x arrow-displacement-y
                 slider-width stepper-size stepper-spacing trough-border} {
        set ${widget}($p) [widgetStyleProperty $widget $p integer]
      }
    }
    # parray GtkHScrollbar
    # puts "============================================================"

    ##
    ## Dynamically create the scrollbars layouts...
    ##
    set Horizontal.Scrollbar.trough [list]
    if {$GtkHScrollbar(has-backward-stepper)} {
      lappend Horizontal.Scrollbar.trough \
              Horizontal.Scrollbar.leftarrow  -side left
    }
    if {$GtkHScrollbar(has-forward-stepper)} {
      lappend Horizontal.Scrollbar.trough \
              Horizontal.Scrollbar.rightarrow -side right
    }
    if {$GtkHScrollbar(has-secondary-backward-stepper)} {
      lappend Horizontal.Scrollbar.trough \
              Horizontal.Scrollbar.leftarrow -side right
    }
    if {$GtkHScrollbar(has-secondary-forward-stepper)} {
      lappend Horizontal.Scrollbar.trough \
              Horizontal.Scrollbar.rightarrow -side left
    }
    lappend Horizontal.Scrollbar.trough \
            Horizontal.Scrollbar.thumb -side left -expand true -sticky we
    ttk::style layout Horizontal.TScrollbar [list \
         Horizontal.Scrollbar.trough -children ${Horizontal.Scrollbar.trough}]

    set Vertical.Scrollbar.trough   [list]
    if {$GtkVScrollbar(has-backward-stepper)} {
      lappend Vertical.Scrollbar.trough \
              Vertical.Scrollbar.uparrow -side top
    }
    if {$GtkVScrollbar(has-forward-stepper)} {
      lappend Vertical.Scrollbar.trough \
              Vertical.Scrollbar.downarrow -side bottom
    }
    if {$GtkVScrollbar(has-secondary-backward-stepper)} {
      lappend Vertical.Scrollbar.trough \
              Vertical.Scrollbar.uparrow -side bottom
    }
    if {$GtkVScrollbar(has-secondary-forward-stepper)} {
      lappend Vertical.Scrollbar.trough \
              Vertical.Scrollbar.downarrow -side top
    }
    lappend Vertical.Scrollbar.trough \
            Vertical.Scrollbar.thumb -side top -expand true -sticky ns
    ttk::style layout Vertical.TScrollbar \
      [list Vertical.Scrollbar.trough -children ${Vertical.Scrollbar.trough}]
  }; # updateLayouts

  proc updateStyles {} {
    # puts [currentThemeColourKeys]
    ttk::style theme settings gttk {
      ttk::style configure . \
         -background       [currentThemeColour bg(NORMAL)] \
         -foreground       [currentThemeColour fg(NORMAL)] \
         -selectforeground [currentThemeColour fg(SELECTED)] \
         -selectbackground [currentThemeColour bg(SELECTED)] \
         ;
      # ttk::style map . -foreground [list \
      #    active          [currentThemeColour fg(ACTIVE)]      \
      #    disabled        [currentThemeColour fg(INSENSITIVE)] \
      #    focus           [currentThemeColour fg(PRELIGHT)]    \
      #    pressed         [currentThemeColour fg(ACTIVE)]      \
      #    selected        [currentThemeColour fg(SELECTED)]    \
      # ] -background [list \
      #    active          [currentThemeColour bg(ACTIVE)]      \
      #    disabled        [currentThemeColour bg(INSENSITIVE)] \
      #    pressed         [currentThemeColour bg(PRELIGHT)]    \
      #    pressed         [currentThemeColour bg(ACTIVE)]      \
      #    selected        [currentThemeColour bg(SELECTED)]    \
      # ]

      # ttk::style map TButton -foreground [list \
      # ] -background [list \
      # ]
      ttk::style configure TButton -anchor center -width -11 -padding {2}

      # ttk::style map TCheckbutton -foreground [list \
      # ] -background [list \
      # ]
      ttk::style configure TCheckbutton -padding {0 1 0 1}
      
      # ttk::style map TCombobox -foreground [list \
      # ] -background [list \
      # ]
      ttk::style configure TCombobox    -padding {1 2 1 1}
      
      # ttk::style map TEntry -foreground [list \
      # ] -background [list \
      # ] -selectforeground [list \
      # ] -selectbackground [list \
      # ]
      # ttk::style configure TEntry       -padding {3 4 3 3}
      
      ttk::style configure TLabelframe  -background [currentThemeColour \
                            bg(NORMAL)] -labeloutside false -padding 0
      
      # ttk::style map TMenubutton -foreground [list \
      # ] -background [list \
      # ] -selectforeground [list \
      # ] -selectbackground [list \
      # ]
      ttk::style configure TMenubutton  -width -11 -padding {3 2 3 2}

      ##
      ## Notebook widget properties:
      ## tab-pos:       Which side of the notebook holds the tabs
      ## tab-border:    Width of the border around the tab labels
      ## tab-hborder:   Width of the horizontal border of tab labels
      ## tab-vborder:   Width of the vertical border of tab labels
      ## homogeneous:   Whether tabs should have homogeneous sizes
      ## tab-overlap:   Size of tab overlap area
      ## tab-curvature: Size of tab curvature
      ## arrow-spacing: Scroll arrow spacing
      ##
      set tab_overlap   [widgetStyleProperty GtkNotebook tab-overlap   integer]
      set tab_curvature [widgetStyleProperty GtkNotebook tab-curvature integer]
      set homogeneous   [widgetProperty      GtkNotebook homogeneous   boolean]
      set tab_pos       [gtkEnum GtkPositionType \
                          [widgetProperty    GtkNotebook tab-pos       integer]]
      set tab_base_overlap 0
      # incr tab_overlap $tab_curvature
      # puts "tab_overlap   -> $tab_overlap"
      # puts "tab_curvature -> $tab_curvature"
      # puts "homogeneous   -> $homogeneous"
      # puts "tab_pos       -> $tab_pos"
      # # puts "tab_overlap=$tab_overlap, tab_base_overlap=$tab_base_overlap"
      switch -exact $tab_pos {
        GTK_POS_TOP    {set tabposition nw}
        GTK_POS_BOTTOM {set tabposition sw}
        GTK_POS_LEFT   {set tabposition nw}
        GTK_POS_RIGHT  {set tabposition ne}
        default        {set tabposition nw}
      }
      # tabmargins {left top right bottom}
      ttk::style configure TNotebook -tabmargins \
        [list $tab_overlap 0 $tab_overlap $tab_base_overlap] \
        -tabposition $tabposition
      ttk::style map TNotebook.Tab -expand [list selected \
        [list $tab_overlap 0 $tab_overlap $tab_base_overlap]]

      # ttk::style map TRadiobutton -foreground [list \
      # ] -background [list \
      # ]
      ttk::style configure TRadiobutton -padding {0 1 0 1}

      # ttk::style map Toolbutton -foreground [list \
      # ] -background [list \
      # ]
      ttk::style configure Toolbutton -anchor center -padding {2 2 2 2}

      ttk::style configure TPaned -background [currentThemeColour bg(NORMAL)]
      ttk::style configure Horizontal.Sash -background [currentThemeColour \
          bg(NORMAL)]
      ttk::style configure Vertical.Sash -background [currentThemeColour \
          bg(NORMAL)]
    };# ttk::style theme settings gttk

    # puts "\nPixel Metric Information:"
    # foreach pm {PM_TabBarTabOverlap       PM_TabBarTabHSpace
    #             PM_TabBarTabVSpace        PM_TabBarBaseHeight
    #             PM_TabBarBaseOverlap      PM_TabBarTabShiftHorizontal
    #             PM_TabBarTabShiftVertical PM_TabBarScrollButtonWidth
    #             PM_DefaultFrameWidth} {
    #   puts "$pm: [getPixelMetric -$pm]"
    # }
  };# updateStyles

  proc kdeLocate_kdeglobals {} {
    return
    set KDE_dirs {}
    # As a first step, examine the KDE env variables...
    global env
    foreach {var cmd} {KDEHOME {kde-config --localprefix} 
                 KDEDIRS {}
                 KDEDIR  {kde-config --prefix}} {
      if {[info exists env($var)]} {
        set paths [set env($var)]
        if {[string length $paths]} {
          foreach path [split $paths :] {lappend KDE_dirs $path}
        }
      }
      if {[string length $cmd]} {
        if {![catch {eval exec $cmd} dir]} {
          lappend KDE_dirs $dir
        }
      }
    }
    # Now, examine all the paths found to locate the kdeglobals file.
    set PATHS {}
    foreach path $KDE_dirs {
      if {[file exists $path/share/config/kdeglobals]} {
        lappend PATHS $path/share/config/kdeglobals
      }
    }
    return $PATHS
  };# kdeLocate_kdeglobals

  ## updateColourPalette:
  #  This procedure will be called from gttk core each time a message is
  #  received from KDE to change the palette used.
  proc updateColourPalette {} {
    return
    #  puts >>updateColourPalette
    foreach filename [kdeLocate_kdeglobals] {
      if {[file exists $filename]} {
        set file [open $filename]
        while {[gets $file line] != -1} {
          set line [string trim $line]
          switch -glob $line {
            contrast=*         {
              if {![info exists options(-contrast)]} {
                set options(-contrast) [string range $line 9 end]
              }
            }
            background=*       {
              if {![info exists options(-background)]} {
                set options(-background) [kdeGetColourHex $line]
              }
            }
            foreground=*       {
              if {![info exists options(-foreground)]} {
                set options(-foreground) [kdeGetColourHex $line]
              }
            }
            buttonBackground=* {
              if {![info exists options(-buttonBackground)]} {
                set options(-buttonBackground) [kdeGetColourHex $line]
              }
            }
            buttonForeground=* {
              if {![info exists options(-buttonForeground)]} {
                set options(-buttonForeground) [kdeGetColourHex $line]
              }
            }
            selectBackground=* {
              if {![info exists options(-selectBackground)]} {
                set options(-selectBackground) [kdeGetColourHex $line]
              }
            }
            selectForeground=* {
              if {![info exists options(-selectForeground)]} {
                set options(-selectForeground) [kdeGetColourHex $line]
              }
            }
            windowBackground=* {
              if {![info exists options(-windowBackground)]} {
                set options(-windowBackground) [kdeGetColourHex $line]
              }
            }
            windowForeground=* {
              if {![info exists options(-windowForeground)]} {
                set options(-windowForeground) [kdeGetColourHex $line]
              }
            }
            linkColor=*        {
              if {![info exists options(-linkColor)]} {
                set options(-linkColor) [kdeGetColourHex $line]
              }
            }
            visitedLinkColor=* {
              if {![info exists options(-visitedLinkColor)]} {
                set options(-visitedLinkColor) [kdeGetColourHex $line]
              }
            }
          }
        }
        close $file
      }
    }
    if {[info exists options]} {
      eval setPalette [array get options]
    }
  };# updateColourPalette

  ## kdeStyleChangeNotification:
  #  This procedure will be called from gttk core each time a message is
  #  received from KDE to change the style used.
  proc kdeStyleChangeNotification {} {
    return
    #  puts >>kdeStyleChangeNotification
    ## This method will be called each time a ClientMessage is received from
    ## KDE KIPC...
    ## Our Job is:
    ##  a) To get the current style from KDE, and
    ##  b) Apply it.
    foreach filename [kdeLocate_kdeglobals] {
      if {[file exists $filename]} {
        set file [open $filename]
        while {[gets $file line] != -1} {
          set line [string trim $line]
          if {[string match widgetStyle*=* $line]} {
            # We have found the style!
            set index [string first = $line]; incr index
            set style [string range $line $index end]
            if {[string length $style]} {
              close $file
              applyStyle $style
              return
            }
          }
        }
        close $file
      }
    }
  };# kdeStyleChangeNotification

  ## applyStyle:
  #  This procedure can be used to apply any available GTK+ style.
  #  Ths "style" parameter must be a string from the style names returned by
  #  ttk::theme::gttk::availableStyles.
  proc applyStyle {style} {
    variable StyleToRc
    variable System
    if {![info exists StyleToRc($style)]} {
      error "unknown GTK style: \"$style\""
    }
    ##
    ## In order to force a style update, we need to create an rc file, and force
    ## GTK to load it...
    ##
    set rc [file normalize $System(TEMP)/gtkrc.gttk-[pid]]
    set fd [open $rc w]
    puts $fd {# -- THEME AUTO-WRITTEN DO NOT EDIT}
    puts $fd "include \"$StyleToRc($style)\""
    puts $fd "include \"$System(HOME)/.gtkrc.mine\""
    puts $fd {# -- THEME AUTO-WRITTEN DO NOT EDIT}
    close $fd
    ##
    ## Set it as the default file...
    ##
    set default_files [gtkDirectory default_files]
    gtkDirectory default_files [list $rc]
    gtk_method gtk_rc_reparse_all_for_settings
    gtk_method gtk_rc_reset_styles
    #gtkDirectory default_files $default_files
    puts $StyleToRc($style)
    updateColourPalette
    updateStyles
    updateLayouts
    event generate {} <<ThemeChanged>>
    file delete -force $rc
  };# applyStyle

  ## kdePaletteChangeNotification:
  #  This procedure will be called from gttk core each time a message is
  #  received from KDE to change the palette used.
  proc kdePaletteChangeNotification {} {
    return
    #  puts >>kdePaletteChangeNotification
    kdeStyleChangeNotification
  };# kdePaletteChangeNotification

  proc kdeGetColourHex {line} {
    set index [string first = $line]; incr index
    set value [string range $line $index end]
    foreach {r g b} [split $value ,] {break}
    return [format #%02X%02X%02X $r $g $b]
  };# kdeGetColourHex

  ## createThemeConfigurationPanel:
  #  This method will create a configuration panel for the gttk theme in the
  #  provided frame widget.
  proc createThemeConfigurationPanel {dlgFrame} {
    ## The first element in our panel, is a combobox, with all the available
    ## GTK+ styles.
    ttk::labelframe $dlgFrame.style_selection -text "GTK+ Style:"
      ttk::combobox $dlgFrame.style_selection.style -state readonly
      $dlgFrame.style_selection.style set [currentThemeName]
      bind $dlgFrame.style_selection.style <<ThemeChanged>> \
        {%W set [ttk::theme::gttk::currentThemeName]}
      bind $dlgFrame.style_selection.style <Enter> \
        {%W configure -values [ttk::theme::gttk::availableStyles]}
      ttk::button $dlgFrame.style_selection.apply -text Apply -command \
       "ttk::theme::gttk::applyStyle \[$dlgFrame.style_selection.style get\]"
      grid $dlgFrame.style_selection.style $dlgFrame.style_selection.apply \
        -padx 2 -sticky snew
      grid columnconfigure $dlgFrame.style_selection 0 -weight 1
    pack $dlgFrame.style_selection -fill x -expand 0 -padx 2 -pady 2
    ## The second element of our panel, is a preview area. Since tile does not
    ## allow us to use a different theme for some widgets, we start a new wish
    ## session through a pipe, and we embed its window in our dialog. Then, we
    ## instrument this second wish through the pipe...
    ttk::labelframe $dlgFrame.preview -text "Preview:"
      variable PreviewInterp
      if {[string length $PreviewInterp]} {
        frame $dlgFrame.preview.container
          pack [label $dlgFrame.preview.container.lbl \
                 -text {Preview Unavailable!}] -fill both -expand 1
      } else {
        frame $dlgFrame.preview.container -container 1 -height 250 -width 400
        ## Create a slave interpreter, and load gttk. Widgets in this interp
        ## may be of a different widget style!
        set PreviewInterp [interp create]
        interp eval $PreviewInterp {package require Tk}
        interp eval $PreviewInterp "
          wm withdraw .
          set auto_path \{$::auto_path\}
          if {[catch {package require Ttk}]} {
            package require tile
          }
          ttk::setTheme gttk
          # package require ttk::theme::gttk
          ttk::theme::gttk::applyStyle \{[currentThemeName]\}
          toplevel .widgets -height 250 -width 400 \
                            -use [winfo id $dlgFrame.preview.container]
          ttk::theme::gttk::selectStyleDlg_previewWidgets .widgets
        "
        bind $dlgFrame.preview.container <Destroy> \
          "ttk::theme::gttk::destroyThemeConfigurationPanel"
        bind $dlgFrame.style_selection.style <<ComboboxSelected>> \
          {ttk::theme::gttk::updateThemeConfigurationPanel [%W get]}
      }
      pack $dlgFrame.preview.container -padx 0 -pady 0 -fill both -expand 1
    pack $dlgFrame.preview -fill both -expand 1 -padx 2 -pady 2
  };# createThemeConfigurationPanel

  proc destroyThemeConfigurationPanel {} {
    variable PreviewInterp
    interp delete $PreviewInterp
    set PreviewInterp {}
  };# destroyThemeConfigurationPanel

  proc updateThemeConfigurationPanel {style} {
    variable PreviewInterp
    interp eval $PreviewInterp "ttk::theme::gttk::applyStyle \{$style\}"
  };# updateThemeConfigurationPanel

  proc selectStyleDlg_previewWidgets {{win {}}} {
    ## Create a notebook widget...
    ttk::notebook $win.nb -padding 6
    set tab1 [ttk::frame $win.nb.tab1]
    $win.nb add $tab1 -text "Tab 1" -underline 4 -sticky news
    set tab2 [ttk::frame $win.nb.tab2]
    $win.nb add $tab2 -text "Tab 2" -underline 4 -sticky news
    set tab3 [ttk::frame $win.nb.tab3]
    $win.nb add $tab3 -text "Tab 3" -underline 4 -sticky news
    set tab4 [ttk::frame $win.nb.tab4]
    $win.nb add $tab4 -text "Tab 4" -underline 4 -sticky news
    ## Fill tab1...
    #####################
    ttk::panedwindow $tab1.panedwindow -orient horizontal
    ## Add a set of radiobuttons to the left...
    ttk::labelframe $tab1.panedwindow.buttons -text " Button Group "
      ttk::radiobutton $tab1.panedwindow.buttons.b1 -text "Radio button" \
         -variable ttk::theme::gttk::temp(selectionVariable) -value 1
      ttk::radiobutton $tab1.panedwindow.buttons.b2 -text "Radio button" \
         -variable ttk::theme::gttk::temp(selectionVariable) -value 2
      ttk::radiobutton $tab1.panedwindow.buttons.b3 -text "Radio button" \
         -variable ttk::theme::gttk::temp(selectionVariable) -value 3
      ttk::separator $tab1.panedwindow.buttons.sep -orient horizontal
      ttk::checkbutton $tab1.panedwindow.buttons.b4 -text "Checkbox"
      $tab1.panedwindow.buttons.b4 state selected
      set ttk::theme::gttk::temp(selectionVariable) 1
      grid $tab1.panedwindow.buttons.b1 -sticky snew -padx 2 -pady 2
      grid $tab1.panedwindow.buttons.b2 -sticky snew -padx 2 -pady 2
      grid $tab1.panedwindow.buttons.b3 -sticky snew -padx 2 -pady 2
      grid $tab1.panedwindow.buttons.sep -sticky snew -padx 2 -pady 2
      grid $tab1.panedwindow.buttons.b4 -sticky snew -padx 2 -pady 2
      grid columnconfigure $tab1.panedwindow.buttons 0 -weight 1
    $tab1.panedwindow add $tab1.panedwindow.buttons
    ## Add a set of other widgets (like progress, combo, scale, etc).
    ttk::frame $tab1.panedwindow.widgets
      ttk::progressbar $tab1.panedwindow.widgets.progress -orient horizontal \
        -maximum 100 -variable ttk::theme::gttk::temp(progress)
      grid $tab1.panedwindow.widgets.progress -sticky snew -padx 2 -pady 2
      ttk::scale $tab1.panedwindow.widgets.scale -orient horizontal -from 0 \
        -to 100 -variable ttk::theme::gttk::temp(progress)
      set ttk::theme::gttk::temp(progress) 70
      grid $tab1.panedwindow.widgets.scale -sticky snew -padx 2 -pady 2
      ttk::entry $tab1.panedwindow.widgets.entry -textvariable \
        ttk::theme::gttk::temp(entry)
      set ttk::theme::gttk::temp(entry) {Entry Widget}
      grid $tab1.panedwindow.widgets.entry -sticky snew -padx 2 -pady 2
      ttk::button $tab1.panedwindow.widgets.button -text Button
      grid $tab1.panedwindow.widgets.button -sticky snew -padx 2 -pady 2
      ttk::combobox $tab1.panedwindow.widgets.combo -values \
        {{Selection 1} {Selection 2} {Selection 3} {Selection 4}}
      $tab1.panedwindow.widgets.combo set {Selection 1}
      grid $tab1.panedwindow.widgets.combo -sticky snew -padx 2 -pady 2
      grid columnconfigure $tab1.panedwindow.widgets 0 -weight 1
    $tab1.panedwindow add $tab1.panedwindow.widgets

    grid $tab1.panedwindow -padx 2 -pady 2 -sticky snew
    ttk::sizegrip $tab1.sg
    ttk::scrollbar $tab1.hsb -orient horizontal
    grid $tab1.hsb $tab1.sg -padx 2 -pady 2 -sticky snew
    ttk::scrollbar $tab1.vsb -orient vertical
    grid $tab1.vsb -row 0 -column 1 -padx 2 -pady 2 -sticky snew
    grid columnconfigure $tab1 0 -weight 1
    grid rowconfigure $tab1 0 -weight 1

    ## Fill tab2...
    #####################
    ttk::panedwindow $tab2.panedwindow -orient vertical
      ttk::label $tab2.panedwindow.label -text {Label Widget}
    $tab2.panedwindow add $tab2.panedwindow.label
      ttk::treeview $tab2.panedwindow.tree -height 4
    $tab2.panedwindow add $tab2.panedwindow.tree
    grid $tab2.panedwindow -padx 2 -pady 2 -sticky snew
    grid columnconfigure $tab2 0 -weight 1
    grid rowconfigure $tab2 0 -weight 1

    pack $win.nb -fill both -expand 1
  };# selectStyleDlg_previewWidgets

  proc availableStyles {} {
    return [lsort -dictionary [availableStyles_AsReturned]]
  };# availableStyles
  
  ## Test the theme configuration panel...
  if {0 && ![info exists ::testConfigurationPanel]} {
    toplevel .themeConfPanel
    wm withdraw .themeConfPanel
    wm title .themeConfPanel "TileGTK Configuration Panel..."
    frame .themeConfPanel.page
    createThemeConfigurationPanel .themeConfPanel.page
    update
    pack .themeConfPanel.page -fill both -expand 1
    wm deiconify .themeConfPanel
  }

  proc getStyleColourInformation {} {
    foreach colour {fg_color bg_color  base_color text_color
                    selected_bg_color  selected_fg_color
                    tooltip_bg_color   tooltip_fg_color} {
      catch {set C($colour) [currentThemeColour $colour]} e
    }
    foreach prefix {fg bg base text} {
      foreach state {NORMAL PRELIGHT ACTIVE SELECTED INSENSITIVE} {
        set colour ${prefix}($state)
        catch {set C($colour) [currentThemeColour $colour]} e
      }
    }
    return [array get C]
  };# getStyleColourInformation

  ## Update layouts on load...
  proc init {} {
    getSystemInfo
    availableStyles_AsReturned
    updateLayouts
    updateStyles
  };# init
}
