#########  Sue2 - an updated version of the SUE schematic editor #######
############         SUE was written by Lee Tavrow        ##############
## updated to Sue2 by Michael Perrott - http://www-mtl.mit.edu/~perrott


########################################################################
# Copyright (c) 2004 Michael H Perrott
# Portions Copyright (c) 1994 Sun Microsystems, Inc.
# All rights reserved.

# Permission is hereby granted, without written agreement and without
# license or royalty fees, to use, copy, modify, and distribute this
# software and its documentation for any purpose, provided that the
# above copyright notice and the following two paragraphs appear in
# all copies of this software.

# IN NO EVENT SHALL SUN MICROSYSTEMS, INC. OR MICHAEL H PERROTT BE
# LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR
# CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS
# DOCUMENTATION, EVEN IF SUN MICROSYSTEMS, INC. AND/OR MICHAEL H PERROTT
# HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# SUN MICROSYSTEMS, INC. AND MICHAEL H PERROTT SPECIFICALLY DISCLAIM ANY
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
# NON-INFRINGEMENT.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
# BASIS, AND SUN MICROSYSTEMS, INC. AND MICHAEL H PERROTT HAVE NO
# OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
# MODIFICATIONS.
########################################################################

########################################################################
# SUE window global variables
#
# WIN		current window.  For now always .win1
#
# WIN_DATA($WIN,)    array with the following fields
#  display_msg  Linked to the text window on the top right of 
#		of the window which contains (hopefully) useful information
#		about the mode, the selection, etc.
#  abort_cmd    tcl procedure to be called to abort out of a mode.  This
#		should be called if we have exited a mode illegally, e.g.
#		with an error.
########################################################################

# make a toplevel window.  We will put a canvas in it later

proc make_window {win} {

  global SPICE_BIN_PATH SNAP_XY WIN_DATA GEOMETRY MODE KEYS ICON_MENU

  # setup global variables associated with this window
  set WIN_DATA($win,display_msg) ""
  set WIN_DATA($win,abort_cmd) ""

  catch {destroy $win}

  toplevel $win

  wm geometry $win $GEOMETRY(window)
  wm minsize $win 400 300
#  wm iconbitmap $win @$CPPSIMSHARED/Sue2/sue_icon.xbm

  bind $win <Unmap> {map_others %W "wm iconify"}
  bind $win <Map> {map_others %W "wm deiconify"}

  # Menu bar
  frame $win.mbar -relief raised -bd 2
  pack $win.mbar -side top -fill x 
  menubutton $win.mbar.file -padx 10 -text File -underline 0 -menu \
	  $win.mbar.file.menu
  menubutton $win.mbar.edit -padx 10 -text Edit -underline 0 -menu \
	  $win.mbar.edit.menu
  menubutton $win.mbar.window -padx 10 -text Window -underline 0 -menu \
	  $win.mbar.window.menu
  menubutton $win.mbar.sim -padx 10 -text Tools -underline 0 -menu $win.mbar.sim.menu
  menubutton $win.mbar.doc -padx 10 -text Doc -underline 0 -menu \
	  $win.mbar.doc.menu
  
  # "File" submenu
  menu $win.mbar.file.menu
#  $win.mbar.file.menu add separator
  $win.mbar.file.menu add command -label "New Schematic" -command "make_new_schematic" \
      -accelerator [abbrev $KEYS(new_schematic)]
  $win.mbar.file.menu add command -label "New Icon" \
      -command {
	if {[make_new_schematic {} I] != ""} {
	  add_properties_to_icon
	  zoom_to_fit
	}
      }
  $win.mbar.file.menu add command -label Load -command "load_schematic" \
      -accelerator [abbrev $KEYS(load)]
  $win.mbar.file.menu add command -label Save -command {write_file $cur_s} \
      -accelerator [abbrev $KEYS(save)]
  $win.mbar.file.menu add command -label "Save as" -command "copy_schematic"
#  $win.mbar.file.menu add command -label "Modified Save and Leaves  " \
#      -command {modified_save_and_leaves $cur_s} \
#      -accelerator [abbrev $KEYS(modified_save_and_leaves)]
#  $win.mbar.file.menu add command -label "Change Path of Cell" \
#      -command "change_path"
  $win.mbar.file.menu add separator
  $win.mbar.file.menu add command -label "Raise Windows  " \
      -command {map_others "" raise} -accelerator [abbrev $KEYS(raise_windows)]
  $win.mbar.file.menu add separator
  $win.mbar.file.menu add command -label "Create eps file" -command "make_ps_simplified" \
      -accelerator [abbrev $KEYS(print)]
#  $win.mbar.file.menu add cascade -label "print setup" \
#      -menu $win.mbar.file.menu.print_setup
  $win.mbar.file.menu add separator
  # $win.mbar.file.menu add command -label Close -command "destroy $win"
  $win.mbar.file.menu add command -label Exit -command "modify_exit" \
      -accelerator [abbrev $KEYS(exit)]

  # "Pages per schematics" cascading submenu
  menu $win.mbar.file.menu.print_setup
  $win.mbar.file.menu.print_setup add radiobutton \
      -label "print to default printer" \
      -variable PRINT(MODE) -value print
  $win.mbar.file.menu.print_setup add radiobutton -label "print to file" \
      -variable PRINT(MODE) -value file
  $win.mbar.file.menu.print_setup add separator
  $win.mbar.file.menu.print_setup add radiobutton -label "landscape" \
       -variable PRINT(ORIENT) -value 1
  $win.mbar.file.menu.print_setup add radiobutton -label "portrait" \
       -variable PRINT(ORIENT) -value 0
  $win.mbar.file.menu.print_setup add separator
  $win.mbar.file.menu.print_setup add radiobutton \
      -label "1 page per schematic" \
      -variable PRINT(PAGES) -value 1 
  $win.mbar.file.menu.print_setup add radiobutton \
      -label "2 pages per schematic" \
      -variable PRINT(PAGES) -value 2
  $win.mbar.file.menu.print_setup add radiobutton \
      -label "4 pages per schematic" \
      -variable PRINT(PAGES) -value 4 

  # "Window" submenu
  menu $win.mbar.window.menu
  $win.mbar.window.menu add command -label "push into" \
      -command "push_into_schematic" -accelerator [abbrev $KEYS(push)]
  $win.mbar.window.menu add command -label "pop out of" \
      -command "pop_out_of_schematic" -accelerator [abbrev $KEYS(pop)]
  $win.mbar.window.menu add command -label "swap views" \
      -command {change_views} -accelerator [abbrev $KEYS(swap_views)]
  $win.mbar.window.menu add command -label "make icon" \
      -command {make_icon} -accelerator [abbrev $KEYS(make_icon)]
  $win.mbar.window.menu add separator
#  $win.mbar.window.menu add command -label "zoom box" \
#      -command "setup_zoom_box" -accelerator [abbrev $KEYS(zoom_box)]
  $win.mbar.window.menu add command -label "zoom in" \
      -command {zoom 1.3} -accelerator [abbrev $KEYS(zoom_in)]
  $win.mbar.window.menu add command -label "zoom out" \
      -command {zoom .8} -accelerator [abbrev $KEYS(zoom_out)]
  $win.mbar.window.menu add command -label "zoom to fit" \
      -command "zoom_to_fit" -accelerator [abbrev $KEYS(zoom_fit)]
#  $win.mbar.window.menu add command -label "center on cursor  " \
#      -command "setup_center_cursor" -accelerator [abbrev $KEYS(center)]
  $win.mbar.window.menu add separator
  $win.mbar.window.menu add command -label "toggle grid" \
      -command "toggle_grid" -accelerator [abbrev $KEYS(grid)]
#  $win.mbar.window.menu add command -label "change grid spacing  " \
#      -command "change_grid" -accelerator [abbrev $KEYS(change_grid)]

  # "Doc" submenu
  menu $win.mbar.doc.menu
  #$win.mbar.doc.menu add command -label "CppSim/VppSim Primer" \
  #    -command "pdf_read Doc/cppsim_vppsim_primer5.pdf"
  #$win.mbar.doc.menu add command -label "Talk on System Level Simulation Using CppSim/VppSim" \
  #    -command "pdf_read Doc/cppsim_talk_perrott_2016.pdf"
  #$win.mbar.doc.menu add command -label "CppSim Reference Manual" \
  #    -command "pdf_read Doc/cppsimdoc.pdf"
  $win.mbar.doc.menu add command -label "Sue2 Manual" \
      -command "pdf_read Doc/sue2_manual.pdf"
  #$win.mbar.doc.menu add command -label "Paper on Double-Interp Method" \
  #    -command "pdf_read Doc/paper.pdf"
  $win.mbar.doc.menu add separator
  $win.mbar.doc.menu add command -label " Intro to SPICE " \
      -command "pdf_read Doc/Hodges-BriefIntroToSPICE-AppA.pdf"
  $win.mbar.doc.menu add command -label "NGspice within CppSim Primer" \
      -command "pdf_read Doc/ngspice_cppsim_primer5.pdf"
  $win.mbar.doc.menu add command -label "Ngspice Manual" \
      -command "pdf_read Doc/ngspice-manual.pdf"
  $win.mbar.doc.menu add command -label "CppSim and Ngspice Data Modules for Python" \
      -command "pdf_read Doc/cppsimdata_for_python.pdf"
#  $win.mbar.doc.menu add command -label "Hspice Toolbox for Matlab/Octave" \
#      -command "pdf_read Doc/hspice_toolbox.pdf"
  $win.mbar.doc.menu add command -label "HSPC Manual" \
      -command "pdf_read Doc/hspc.pdf"
  $win.mbar.doc.menu add command -label "BSIM3v3 Manual" \
      -command "pdf_read Doc/BSIM3v3.pdf"
#  $win.mbar.doc.menu add command -label "BSIM4 Manual" \
#      -command "pdf_read Doc/BSIM464_Manual.pdf"
  $win.mbar.doc.menu add command -label "SCMOS Manual" \
      -command "pdf_read Doc/scmos.pdf"
  $win.mbar.doc.menu add separator
  #$win.mbar.doc.menu add command -label "PLL Design Assistant Manual" \
  #    -command "pdf_read Doc/pll_manual.pdf"

  # "Edit" submenu
  menu $win.mbar.edit.menu
  $win.mbar.edit.menu add command -label "Undo" \
      -command "undo_last" -accelerator [abbrev $KEYS(undo)]
  $win.mbar.edit.menu add separator
  $win.mbar.edit.menu add command -label "add wire" \
      -command "setup_draw_wire" -accelerator [abbrev $KEYS(add_wire)]
  $win.mbar.edit.menu add command -label "add text" \
      -command "setup_text_mode" -accelerator [abbrev $KEYS(add_text)]
  $win.mbar.edit.menu add command -label "add line" \
      -command "setup_line_mode" -accelerator [abbrev $KEYS(add_line)]
  $win.mbar.edit.menu add command -label "add arc" \
      -command "setup_arc_mode" -accelerator [abbrev $KEYS(add_arc)]
  $win.mbar.edit.menu add separator
  $win.mbar.edit.menu add command -label "move selected" \
      -command "setup_move_mode" -accelerator [abbrev $KEYS(move_selected)]
  $win.mbar.edit.menu add command -label "rotate" \
      -command "transform_selected ROTATE" \
      -accelerator [abbrev $KEYS(rotate)]
  $win.mbar.edit.menu add command -label "flip horizontal (<-->)" \
      -command "transform_selected MX" \
      -accelerator [abbrev $KEYS(flip_horizontal)]
  $win.mbar.edit.menu add command -label "flip vertical" \
      -command "transform_selected MY" \
      -accelerator [abbrev $KEYS(flip_vertical)]
  $win.mbar.edit.menu add command -label "duplicate selected  " \
      -command "setup_duplicate_selected" \
      -accelerator [abbrev $KEYS(duplicate_selected)]
  $win.mbar.edit.menu add command -label "replace instance" \
      -command "setup_replace_instance" \
      -accelerator [abbrev $KEYS(replace_instance)]
  $win.mbar.edit.menu add command -label "duplicate selected text  " \
      -command "setup_duplicate_selected text" \
      -accelerator [abbrev $KEYS(duplicate_text)]
  $win.mbar.edit.menu add command -label "show text anchors" \
      -command "show_anchors" -accelerator [abbrev $KEYS(show_anchors)]
  $win.mbar.edit.menu add command -label "show sel. term names" \
      -command "show_icon_term_names" \
      -accelerator [abbrev $KEYS(show_term_names)]
  $win.mbar.edit.menu add command -label "lower selected" \
      -command {$cur_c lower selected} \
      -accelerator [abbrev $KEYS(lower_selected)]
  $win.mbar.edit.menu add separator
  $win.mbar.edit.menu add command -label "delete selected" \
      -command "delete_selected" -accelerator [abbrev $KEYS(delete_selected)]
  $win.mbar.edit.menu add command -label "copy to clipboard" \
      -command "delete_selected_undo copy_to_clipboard" \
      -accelerator [abbrev $KEYS(copy_to_clipboard)]
  $win.mbar.edit.menu add command -label "paste" \
      -command "setup_paste_mode" -accelerator [abbrev $KEYS(paste)]
#  $win.mbar.edit.menu add separator
#  $win.mbar.edit.menu add command -label "delete cell" \
#      -command {delete_schematic $cur_s}

  # "Tools" submenu (filled in depending on mode)
  menu $win.mbar.sim.menu
  make_tools_menu $win

  # pack the menu headings
  pack $win.mbar.file $win.mbar.window $win.mbar.edit $win.mbar.sim \
       $win.mbar.doc -side left

  tk_menuBar $win.mbar $win.mbar.file $win.mbar.window \
       $win.mbar.edit $win.mbar.sim $win.mbar.doc

  # Feedback text line is to the right of the menu bar
  label $win.mbar.msg -relief sunken -bd 2 -anchor w \
      -padx 7 -textvariable WIN_DATA($win,display_msg)
  pack $win.mbar.msg -side left -fill x -expand yes

 if {$ICON_MENU != "flat" && $ICON_MENU != "hier"} { 
    # make the listbox frame and pack it in
    frame $win.lb -width 300
    pack $win.lb -side right -fill y

    # make a special frame for resizing the listboxes
    frame $win.resizelb -width 2 
    pack $win.resizelb -side right -fill y
    bind $win.resizelb <Enter> "listbox_resize enter"
    bind $win.resizelb <Leave> "listbox_resize leave"
    bind $win.resizelb <Button-1> "listbox_resize start %X"
    bind $win.resizelb <Button1-Motion> "listbox_resize move %X"
    bind $win.resizelb <B1-ButtonRelease> "listbox_resize release"
  }

  # create the happy scrollbars
  scrollbar $win.vscroll -relief sunken -command {$cur_c yview}
  scrollbar $win.hscroll -orient horiz -relief sunken -command {$cur_c xview}
  pack $win.hscroll -side bottom -fill x
  pack $win.vscroll -side right -fill y

  # this binding changes focus to current canvas when entered
  bind $win <Any-Enter> {enter_canvas $cur_s}
  bind $win <Any-Leave> {leave_canvas}

  return $win
}

proc listbox_clear_selection {win_listbox} {
  $win_listbox selection clear 0 end
}

# called when mouse in resize listbox frame to change cursor and
# resize listbox with button1

proc listbox_resize {type {x ""}} {

  global WIN _LISTBOX_RESIZE_

  switch $type {
    enter {
      if {[info exists _LISTBOX_RESIZE_(cursor)]} {
	# already here
	return
      }
      set _LISTBOX_RESIZE_(cursor) [$WIN cget -cursor]

      # horiz double arrow
      $WIN configure -cursor sb_h_double_arrow
    }

    leave {
      if {![info exists _LISTBOX_RESIZE_(button)] && \
	      [info exists _LISTBOX_RESIZE_(cursor)]} {
	$WIN configure -cursor $_LISTBOX_RESIZE_(cursor)
	catch {unset _LISTBOX_RESIZE_(cursor)}
      }
    }

    start {
      set _LISTBOX_RESIZE_(button) 1
    }

    move {
      # move by one character width
      set actualx [winfo rootx $WIN.lb.schematics]

#      resize_listboxes [expr ($actualx - $x) / 16]

      if {$x > [expr $actualx + 7]} {
	resize_listboxes -1
      } elseif {$x < [expr $actualx - 7]} {
	resize_listboxes 1
      }
    }

    release {
      catch {unset _LISTBOX_RESIZE_(button)}

      # probably not needed
      listbox_resize leave
    }
  }
}



proc pdf_read {filename} {
global CPPSIMSHARED_HOME tcl_platform

    if {$tcl_platform(platform) == "windows"} {
        open "|$CPPSIMSHARED_HOME/SumatraPDF/SumatraPDF -restrict \
               $CPPSIMSHARED_HOME/$filename"
    } else {
       if {$tcl_platform(os) == "Darwin"} {
           open "|open \
                  $CPPSIMSHARED_HOME/$filename"
       } else {
           open "|evince \
                  $CPPSIMSHARED_HOME/$filename"
       }
    }
}

proc SueFileDialog {operation title} {
    global PREV_DIRECTORY CPPSIM_HOME cur_s

    #   Type names		Extension(s)	Mac File Type(s)
    #
    #---------------------------------------------------------
    set types {
	{"Sue Files"		{.sue}	}
	{"All files"		*}
    }

    set old_dir [pwd]

    upvar #0 SUE_$cur_s data
    set cell_filename [use_first data(filename)]
    set i [string last "/" $cell_filename]
    set PREV_DIRECTORY [string range $cell_filename 0 [expr ${i}-1]]

    cd $PREV_DIRECTORY

    if {$operation == "open"} {
	set file [tk_getOpenFile -title $title -filetypes $types -parent . \
            -initialdir $PREV_DIRECTORY]
    } else {
        set i [string last "SueLib" $PREV_DIRECTORY]
        set suelib_check [string range $PREV_DIRECTORY $i end]
        if {[string compare $suelib_check "SueLib"] == 0} {
            tk_dialog_new .cppsim_message "Error: need to first open an existing schematic cell to specify library" \
	       "Error:  the library for a new module wil be the same as the library of the current schematic cell. You must open a proper schematic cell within the desired library first before creating a new module\n\nNOTE: If you want to create a new libary, use the 'Create Library' command in the Library Manager (under the Tools menu)" "" 0 {OK}
            return ""
        }
	set file [tk_getSaveFile -filetypes $types -parent . \
	    -initialdir $PREV_DIRECTORY -initialfile Untitled -title $title]
        set i [string last "/" $file]
	set cellname [string range $file [expr ${i} + 1] end]
        set cellname [string map {" " "_" ":" "_" "." "_" "," "_" ";" "_" "-" "_" "*" "_"} $cellname]
	set file [string replace $file [expr ${i} + 1] end $cellname]
    }

    if {$file != ""} {
       set i [string last "/" $file]
	set PREV_DIRECTORY [string range $file 0 [expr ${i}-1]]
    }
    cd $old_dir

    return $file
}

proc SueNetFileDialog {operation title} {

    global cur_s CPPSIM_HOME

    #   Type names		Extension(s)	Mac File Type(s)
    #
    #---------------------------------------------------------
    set types {
	{"Netlist Files"     {.sp}	}
	{"All files"		*}
    }

    set old_dir [pwd]
    cd $CPPSIM_HOME/Netlist
    if {$operation == "open"} {
	set file [tk_getOpenFile -title $title -filetypes $types -parent .]
    } else {
	set file [tk_getSaveFile -filetypes $types -parent . \
	    -initialfile $cur_s -title $title]
    }
    cd $old_dir
    return $file
}

proc SuePSFileDialog {operation title} {

    global cur_s CPPSIM_HOME

    #   Type names		Extension(s)	Mac File Type(s)
    #
    #---------------------------------------------------------
    set types {
	{"EPS Files"     {.eps}	}
	{"All files"		*}
    }

    set old_dir [pwd]
    cd $CPPSIM_HOME/Sue2/EPSfiles
    if {$operation == "open"} {
	set file [tk_getOpenFile -title $title -filetypes $types -parent .]
    } else {
	set file [tk_getSaveFile -filetypes $types -parent . \
	    -initialfile $cur_s -title $title]
    }
    cd $old_dir

    return $file
}


proc create_spice_netlist {option_string} {
global cur_s CPPSIMSHARED_HOME CPPSIM_HOME tcl_platform

     if {$tcl_platform(platform) == "windows"} {
        set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin/win32"
     } else {
         set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin"
     }

     set netlist_file [SueNetFileDialog "save" "Netlist Filename"]
     if {$netlist_file == ""} {
        return
     } else {
	 if {$option_string == "no_top_sub"} {
             catch "exec $sue2_bin_directory/sue_spice_netlister $cur_s \
                    $CPPSIM_HOME/Sue2/sue.lib $netlist_file.sp no_top_sub" message
	 } else {
             catch "exec $sue2_bin_directory/sue_spice_netlister $cur_s \
                    $CPPSIM_HOME/Sue2/sue.lib $netlist_file.sp" message
	 }
          set button [tk_dialog_new .netlist_message "Netlisting Messages" \
		    "$message" \
		    "" 0 {OK}]
     }

}

proc start_cppsimview {} {
global cur_s CPPSIMSHARED_HOME

#catch {exec $CPPSIMSHARED_HOME/MatlabGui/bin/cppsimview}
catch {exec $CPPSIMSHARED_HOME/MatlabGui/bin/cppsimview >& NUL: &}

}

# The sim menu has been broken out so it can replaced if the
# netlist type changes.

proc make_tools_menu {win} {

  global KEYS MODE NETLIST_TYPE PROBE_TYPE cur_s CPPSIM_RUN_FLAG

  set CPPSIM_RUN_FLAG 0

#  $win.mbar.sim.menu add command -label "Start CppSimView" \
#      -command {start_cppsimview} 
  $win.mbar.sim.menu add command -label "Library Manager" \
      -command {create_run_lib_manager_form} 
  $win.mbar.sim.menu add separator
  #$win.mbar.sim.menu add command -label "CppSim Simulation" \
  #    -command {create_run_cppsim_form CppSim}
  #$win.mbar.sim.menu add separator 
  #$win.mbar.sim.menu add command -label "VppSim Simulation" \
  #    -command {create_run_cppsim_form VppSim}
  #$win.mbar.sim.menu add separator 
  $win.mbar.sim.menu add command -label "NGspice Simulation" \
      -command {create_run_ngspice_form} 
#  $win.mbar.sim.menu add command -label "Hspice Simulation" \
#      -command {create_run_hspice_form} 
  $win.mbar.sim.menu add separator 
  $win.mbar.sim.menu add command -label "Create $NETLIST_TYPE netlist" \
      -command {create_spice_netlist no_top_sub} \
      -accelerator [abbrev $KEYS(netlist)]
  $win.mbar.sim.menu add command -label "Create $NETLIST_TYPE netlist (with top sub)" \
      -command {create_spice_netlist with_top_sub}
#  $win.mbar.sim.menu add command \
#      -label "$NETLIST_TYPE it" -command {${NETLIST_TYPE}_it} \
#      -accelerator [abbrev $KEYS(spice_it)]
#  $win.mbar.sim.menu add command -label "init probe" \
#      -command {${PROBE_TYPE}_init_probe} \
#      -accelerator [abbrev $KEYS(init_probe)]
#  $win.mbar.sim.menu add command -label "close probe" \
#      -command {${PROBE_TYPE}_close_probe}
#  $win.mbar.sim.menu add separator
#  $win.mbar.sim.menu add command -label "change simulation mode" \
#      -command "change_netlist_props" 
#  $win.mbar.sim.menu add separator

#  if {$NETLIST_TYPE == "spice"} {
#    $win.mbar.sim.menu add command -label "kill spice job" \
#	-command "kill_spice_job" 
#    $win.mbar.sim.menu add separator
#  }
   
#  $win.mbar.sim.menu add command -label "plot net" \
#      -command {${PROBE_TYPE}_plot_net} \
#      -accelerator [abbrev $KEYS(plot_net)]

#  if {$PROBE_TYPE == "interactive"} {
#    $win.mbar.sim.menu add command -label "plot net (binary)" \
#	-command {${PROBE_TYPE}_plot_net %b} \
#	-accelerator [abbrev $KEYS(plot_net_remember)]
#    $win.mbar.sim.menu add command -label "plot net (hex)" \
#	-command {${PROBE_TYPE}_plot_net %h} \
#	-accelerator [abbrev $KEYS(plot_old_net)]
#    $win.mbar.sim.menu add command -label "update flags" \
#	-command {verilog_update_flags} \
#	-accelerator [abbrev $KEYS(unplot_net)]
#    $win.mbar.sim.menu add command -label "display term values" \
#	-command {verilog_display_term_values} \
#	-accelerator [abbrev $KEYS(unplot_old_net)]

#  } elseif {$PROBE_TYPE == "analyzer"} {
#    $win.mbar.sim.menu add command -label "plot net & remember" \
#	-command {${PROBE_TYPE}_plot_net_and_remember} \
#	-accelerator [abbrev $KEYS(plot_net_remember)]
#    $win.mbar.sim.menu add command -label "forget net" \
#	-command {${PROBE_TYPE}_unplot_net_and_forget} \
#	-accelerator [abbrev $KEYS(unplot_net_forget)]
#    $win.mbar.sim.menu add command -label "update flags" \
#	-command {irsim_update_flags} \
#	-accelerator [abbrev $KEYS(unplot_net)]
#    $win.mbar.sim.menu add command -label "display term values" \
#	-command {irsim_display_term_values} \
#	-accelerator [abbrev $KEYS(unplot_old_net)]
#    $win.mbar.sim.menu add separator
#    $win.mbar.sim.menu add command -label "irsim step" \
#	-command {irsim_step} -accelerator [abbrev $KEYS(irsim_step)]
#    $win.mbar.sim.menu add command -label "irsim step & update" \
#	-command {irsim_step_update} \
#	-accelerator [abbrev $KEYS(irsim_step_update)]
#    $win.mbar.sim.menu add command -label "irsim set hi" \
#	-command {irsim_set h} -accelerator [abbrev $KEYS(irsim_set_hi)]
#    $win.mbar.sim.menu add command -label "irsim set low" \
#	-command {irsim_set l} -accelerator [abbrev $KEYS(irsim_set_low)]
#    $win.mbar.sim.menu add command -label "irsim set x" \
#	-command {irsim_set u} -accelerator [abbrev $KEYS(irsim_set_x)]
#    $win.mbar.sim.menu add command -label "irsim query net" \
#	-command {irsim_set ?} -accelerator [abbrev $KEYS(irsim_query_net)]
#    $win.mbar.sim.menu add command -label "enter irsim mode" \
#	-command {enter_irsim_mode} \
#	-accelerator [abbrev $KEYS(enter_irsim_mode)]

#  } else {
#    $win.mbar.sim.menu add command -label "unplot net" \
#	-command {${PROBE_TYPE}_unplot_net} \
#	-accelerator [abbrev $KEYS(unplot_net)]
#    $win.mbar.sim.menu add command -label "plot net & remember" \
#	-command {${PROBE_TYPE}_plot_net_and_remember} \
#	-accelerator [abbrev $KEYS(plot_net_remember)]
#    $win.mbar.sim.menu add command -label "unplot net & forget" \
#	-command {${PROBE_TYPE}_unplot_net_and_forget} \
#	-accelerator [abbrev $KEYS(unplot_net_forget)]
#  }

#  if {$PROBE_TYPE == "NST"} {
#    $win.mbar.sim.menu add command -label "plot old net  " \
#	-command {${PROBE_TYPE}_plot_net ~} \
#	-accelerator [abbrev $KEYS(plot_old_net)]
#    $win.mbar.sim.menu add command -label "unplot old net" \
#	-command {${PROBE_TYPE}_unplot_net ~} \
#	-accelerator [abbrev $KEYS(unplot_old_net)]
#  }

#  if {$PROBE_TYPE != "interactive"} {
#    $win.mbar.sim.menu add separator
#    $win.mbar.sim.menu add command -label "erase and plot memory" \
#	-command {${PROBE_TYPE}_erase_and_plot_memory}
#    $win.mbar.sim.menu add command -label "cancel memory" \
#	-command "cancel_memory" 
#  }

#  if {$NETLIST_TYPE == "verilog"} {
#    $win.mbar.sim.menu add separator
#    $win.mbar.sim.menu add command -label "create verilog property  " \
#	-command "setup_duplicate_selected verilog"
#  }

}

proc clean_cpp_lib_name {lib_name} {

  set base_lib_index [string first ":Private" $lib_name]
  if {$base_lib_index != -1 } {
     set base_lib [string range $lib_name 0 [expr ${base_lib_index} - 1]]
  } else {
     set base_lib $lib_name
  } 
  return $base_lib
}

# Pops up a window to create/edit CppSim code for a given icon

proc cppsim_code_menu {mod_name} {

  global command

  set command 0

  # Just in case there is an old one around
  catch {destroy .cppsim_menu}

  toplevel .cppsim_menu 
  set w .cppsim_menu

  set sc_width [winfo screenwidth $w]
  set sc_height [winfo screenheight $w]
  set w_req_width [winfo reqwidth $w]
  set w_req_height [winfo reqheight $w]
  set parx [winfo vrootx [winfo parent $w]]
  set pary [winfo vrooty [winfo parent $w]]
  set x [expr ${sc_width}/2 - ${w_req_width}/2 - ${parx}]
  set y [expr ${sc_height}/2 - ${w_req_height}/2 - ${pary}]

  wm geometry .cppsim_menu "+$x+$y"
  wm title .cppsim_menu $mod_name
    
  bind .cppsim_menu <Escape> {catch {destroy .cppsim_menu} ; return}

  frame .cppsim_menu.default -relief sunken -bd 1
  button .cppsim_menu.cancel -padx 5 -text "Cancel" \
      -command {set command "" ; catch {destroy .cppsim_menu}; return}
  pack .cppsim_menu.cancel -in .cppsim_menu.default -padx 4m -pady 1m -ipadx 2m -expand 1

  label .cppsim_menu.note -text [cppCreateModuleLabel $mod_name] -justify left 
  pack .cppsim_menu.note -side top
  pack .cppsim_menu.default -side bottom

  cppCreateCodeButtons $mod_name .cppsim_menu

  # Border pixels for X windows.  Can't seem to figure these out.
  set XBORDER 3
  set YBORDER 25

  # If the prop_menu floats off the screen, move it back on.
  set win_width [winfo screenwidth .cppsim_menu]
  set win_height [winfo screenheight .cppsim_menu]
  set menu_width [winfo width .cppsim_menu]
  set menu_height [winfo height .cppsim_menu]
  set dx [min [expr ${win_width}-${menu_width}- \
		   ${x}-${XBORDER}] 0]
  set dy [min [expr ${win_height}-${menu_height}- \
		   ${y}-${YBORDER}] 0]
  if {$dx < 0 || $dy < 0} {
      wm geometry .cppsim_menu "+[expr ${x}+${dx}]+[expr ${y}+${dy}]"    
  }
   
  grab set .cppsim_menu

  vwait command
  return $command
}

############## create CppSim code buttons ################
### Note:  it is assumed that cppCreateModuleLabel was run before
###        this routine in order to properly set up SUE2_CELL_FILENAME
###        and CPPSIM_LIBRARY
proc cppCreateCodeButtons {cur_mod win_id} {

  global CPPSIM_CELL CPPSIM_LIBRARY SUE2_CELL_FILENAME CPPSIMSHARED_HOME \
         CPPSIM_HOME CANCEL_BUTTON cppsim_code_file verilog_code_file \
         cur_code_file_choice cppCodeChoice


  set CANCEL_BUTTON $win_id.cancel
  set CPPSIM_CELL $cur_mod

###### determine if CppSim code exists and then create associated buttons

  set base_dir_index [string first "/SueLib/" $SUE2_CELL_FILENAME]
  set base_dir [string range $SUE2_CELL_FILENAME 0 [expr ${base_dir_index} - 1]]
  set cpp_lib [clean_cpp_lib_name $CPPSIM_LIBRARY]
  if {$cpp_lib == "devices"} {
     return
  }
  set cppsim_code_file "$base_dir/CadenceLib/$cpp_lib/$CPPSIM_CELL/cppsim/text.txt"
  set verilog_code_file "$base_dir/CadenceLib/$cpp_lib/$CPPSIM_CELL/verilog/verilog.v"
  set cur_code_file_choice $cppsim_code_file
  set cppCodeChoice CppSim

  #### Create buttons
  frame $win_id.cppsim_code_button
  frame $win_id.cppsim_code_button_group


  if {[file isfile $cppsim_code_file]} {
     button $win_id.cppsim_create -state disabled -padx 5 -text "Create" \
	 -command {if {[cppCreateCppSimCode $cppCodeChoice]} { \
	     cppEditFile $cur_code_file_choice }; catch {$CANCEL_BUTTON invoke}}
  } else {
     button $win_id.cppsim_create -padx 5 -text "Create" \
	 -command {if {[cppCreateCppSimCode $cppCodeChoice]} { \
	     cppEditFile $cur_code_file_choice }; catch {$CANCEL_BUTTON invoke}}
  }
  if {[file isfile $cppsim_code_file]} {
     button $win_id.cppsim_code_edit -padx 5 -text "Edit" -command {\
	 cppEditFile $cur_code_file_choice ; catch {$CANCEL_BUTTON invoke}} 
     button $win_id.cppsim_code_delete -padx 5 -text "Delete" -command {\
	 cppDeleteFile $cur_code_file_choice}
  } else {
     button $win_id.cppsim_code_edit -state disabled -padx 5 -text "Edit" -command {\
	 cppEditFile $cur_code_file_choice ; catch {$CANCEL_BUTTON invoke}} 
     button $win_id.cppsim_code_delete -state disabled -padx 5 -text "Delete" -command {\
	 cppDeleteFile $cur_code_file_choice}
  }


  set CPP_CODE_MENU [tk_optionMenu $win_id.cppsim_code_options cppCodeChoice CppSim Verilog]
  $CPP_CODE_MENU entryconfigure 0 -command "set cur_code_file_choice $cppsim_code_file; \
         if {[file isfile $cppsim_code_file]} {$win_id.cppsim_code_delete configure -state normal; \
          $win_id.cppsim_create configure -state disabled;\
          $win_id.cppsim_code_edit configure -state normal}\
          else {$win_id.cppsim_code_delete configure -state disabled;\
          $win_id.cppsim_create configure -state normal; \
          $win_id.cppsim_code_edit configure -state disabled}"
  $CPP_CODE_MENU entryconfigure 1 -command "set cur_code_file_choice $verilog_code_file; \
         if {[file isfile $verilog_code_file]} {$win_id.cppsim_code_delete configure -state normal; \
         $win_id.cppsim_create configure -state disabled;\
         $win_id.cppsim_code_edit configure -state normal}\
         else {$win_id.cppsim_code_delete configure -state disabled; \
         $win_id.cppsim_create configure -state normal; \
         $win_id.cppsim_code_edit configure -state disabled}"



  pack $win_id.cppsim_code_options -side left -in \
        $win_id.cppsim_code_button -padx 1m -ipadx 0m -pady 2m -expand 1
  pack $win_id.cppsim_code_edit -side left -in $win_id.cppsim_code_button_group \
        -padx 0m -ipadx 0m -pady 2m -expand 1
  pack $win_id.cppsim_create -side left -in $win_id.cppsim_code_button_group \
         -padx 0m -ipadx 0m -pady 2m -expand 1
  pack $win_id.cppsim_code_delete -side left -in \
        $win_id.cppsim_code_button_group -padx 0m -ipadx 0m -pady 2m -expand 1
  pack $win_id.cppsim_code_button_group -side right -in \
        $win_id.cppsim_code_button -padx 1m -ipadx 1m -pady 2m -expand 1


  pack $win_id.cppsim_code_button -side bottom

}    

proc cppDeleteFile {file_name} {
global CANCEL_BUTTON

set verify_flag [tk_dialog_new .cppsim_message "Verification of Delete Operation" \
		  "Are you sure that you want to delete file\n   $file_name?" "" 1 {Yes} {Cancel}]

if {$verify_flag == 0} {
   ## verify that file indeed exists
    if {[file exists $file_name] != 1} {
      tk_dialog_new .cppsim_message "Error In Delete Operation" \
	     "Error:  cannot find file\n   $file_name" "" 0 {OK}
   } else {
      if {[catch "file rename -force $file_name $file_name.old" err]} {
	  tk_dialog_new .cppsim_message "Error in Delete Operation" \
	       "$err" "" 0 {OK} 
      } else {
          tk_dialog_new .cppsim_message "Completed Delete Operation" \
	        "Deleted file, but kept a copy of its contents as file:\n  $file_name.old" "" 0 {OK} 
      }
   }
catch {$CANCEL_BUTTON invoke}
}
}


proc cppEditFile {file_name} {

global tcl_platform EDITOR RUN_EDITOR CPPSIMSHARED_HOME
  
set RUN_EDITOR $EDITOR

if {$tcl_platform(platform) == "windows"} {
   if {$EDITOR == "emacs"} {
        set RUN_EDITOR "$CPPSIMSHARED_HOME/Emacs/win32/emacs-21.3/bin/emacs"
   }
} else {
   if {$tcl_platform(os) == "Darwin"} {
         set RUN_EDITOR "open -n -a $CPPSIMSHARED_HOME/Emacs/macosx/Emacs.app --args"
   }
}

if {$EDITOR == "emacs"} {
   if {$tcl_platform(os) == "Darwin"} {
        catch "exec $RUN_EDITOR --name $file_name -Q -no-splash $file_name &" emacs_message
   } else {
        catch "exec $RUN_EDITOR -bg black -fg yellow -T $file_name $file_name &" emacs_message
   }
} else { 
    catch "exec $RUN_EDITOR $file_name &" emacs_message
}
}

proc cppCreateCppSimCode {cppCodeChoice} {

global CPPSIM_CELL CPPSIM_LIBRARY EDITOR RUN_EDITOR CPPSIMSHARED_HOME \
       tcl_platform CANCEL_BUTTON SUE2_CELL_FILENAME sue2_bin_directory

if {$tcl_platform(platform) == "windows"} {
     set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin/win32"
} else {
     set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin"
}

## search sue2 module file to see if instances exist
if {[catch "set fid \[open $SUE2_CELL_FILENAME r\]" err]} {
   tk_dialog_new .cppsim_message "Error in examining Sue2 module file" \
	     "$err" "" 0 {OK} 
   return 0
}

set found_instance_flag 0
while {[gets $fid line] >= 0} { 
   if {[string last "make " $line] >= 0} {
       if {[string last "make input" $line] < 0 && \
           [string last "make output" $line] < 0 && \
           [string last "make inout" $line] < 0 && \
           [string last "make global" $line] < 0 && \
           [string last "make name_net" $line] < 0 && \
	   [string last "make name_net_s" $line] < 0} {
           set found_instance_flag 1
           break
       }
   }
}

if {[catch {close $fid} err]} {
    tk_dialog_new .cppsim_message "Error in examining Sue2 module file" \
	     "$err" "" 0 {OK} 
    return 0
}

if {$cppCodeChoice == "CppSim"} {
   if {$found_instance_flag == 1} {
      set code_index [tk_dialog_new .cppsim_message "Select CppSim Module Type" \
		       "Select the Type of CppSim Module\nYou Would Like to Create" "" 1 {Module Code} {Sim_Order Code} {Cancel}]
   } else {
      set code_index 0
   }

   if {$code_index == 0} {
      set cpp_lib [clean_cpp_lib_name $CPPSIM_LIBRARY]
      catch "exec $sue2_bin_directory/create_cppsim_code_template \
         $CPPSIM_CELL $cpp_lib CppSim" create_message;
      if {[string first "Error:" $create_message] != -1} {
          tk_dialog_new .cppsim_message "Error in Creating CppSim Code" \
		    "$create_message" "" 0 {OK}
          return 0
      }
   } elseif {$code_index == 1} {
      return [cppCreateSimOrderModule]
   } else {
      return 0
   }
} else {
   if {$found_instance_flag == 1} {
      set code_index [tk_dialog_new .cppsim_message "Verify Verilog Code Creation" \
		       "Note that this cell contains instances below it.\nAre you sure that you want to create\nVerilog code for it?" "" 1 {Proceed}  {Cancel}]
   } else {
      set code_index 0
   }

   if {$code_index == 0} {
      set cpp_lib [clean_cpp_lib_name $CPPSIM_LIBRARY]
      catch "exec $sue2_bin_directory/create_cppsim_code_template \
         $CPPSIM_CELL $cpp_lib Verilog" create_message;
      if {[string first "Error:" $create_message] != -1} {
          tk_dialog_new .cppsim_message "Error in Creating Verilog Code" \
		    "$create_message" "" 0 {OK}
          return 0
      }
   } else {
      return 0
   }
}

return 1
}


proc cppCreateSimOrderModule {} {

  global tcl_platform CPPSIM_HOME CPPSIMSHARED_HOME \
         CPPSIM_CELL CPPSIM_LIBRARY SUE2_CELL_FILENAME

  if {$tcl_platform(platform) == "windows"} {
     set cppsimshared_bin_directory "$CPPSIMSHARED_HOME/bin/win32" 
     set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin/win32"
  } else {
     set cppsimshared_bin_directory "$CPPSIMSHARED_HOME/bin"
     set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin"
  }

  ###### Netlist the current cell to determine sim_order

  set netlist_dir "$CPPSIM_HOME/Netlist"
  if {[file isdirectory $netlist_dir] == 0} {
      tk_dialog_new .cppsim_message "Error in checking sim_order of cell '$CPPSIM_CELL'" \
               "Error:  CppSim does not have a Netlist directory!\n      (i.e., can't open directory '$netlist_dir')\n      ->  create this directory to correct this problem\n\n------------------ Netlisting Exited with Errors --------------------\n" \
            "" 0 {OK} 
      return 0
  }

  if {[file isfile "$netlist_dir/sim_order_netlist.cppsim"] != 0} {
      if {[catch {file delete "$netlist_dir/sim_order_netlist.cppsim"} mssg]} {
           tk_dialog_new .cppsim_message "Error in checking sim_order of cell '$CPPSIM_CELL'" \
                  "Error:  cannot delete netlist file!\n     (i.e., cannot delete '$netlist_dir/sim_order_netlist.cppsim')\n      ->  remove this file to correct this problem\n\n------------------ Netlisting Exited with Errors --------------------\n"\
            "" 0 {OK} 
      return 0
      } 
  }

  catch {open "|$sue2_bin_directory/sue_cppsim_netlister $CPPSIM_CELL \
             $CPPSIM_HOME/Sue2/sue.lib $netlist_dir/sim_order_netlist.cppsim |& cat"} fid
  set message_input [read $fid]
  if {[catch {close $fid} err] != 0 || [file isfile "$netlist_dir/sim_order_netlist.cppsim"] == 0} {
       tk_dialog_new .cppsim_message "Error in checking sim_order of cell $CPPSIM_CELL using sue_cppsim_netlister" \
	     "$message_input" "" 0 {OK} 
       return 0
  }

  set cur_dir [pwd]
  cd $netlist_dir

  catch {open "|$cppsimshared_bin_directory/net2code -sim_order none $netlist_dir/sim_order_netlist.cppsim |& cat"} fid
  set message_input [read $fid]

  cd $cur_dir

  if {[catch {close $fid} err] != 0} {
      tk_dialog_new .cppsim_message "Error in checking sim_order of cell $CPPSIM_CELL using 'net2code -sim_order'" \
       "$message_input" "" 0 {OK} 
      return 0
  } 
  set cur_index [string last "sim_order:" $message_input]
  if {$cur_index < 0} {
        tk_dialog_new .cppsim_message "Error in checking sim_order of cell $CPPSIM_CELL using 'net2code -sim_order'" \
	     "There is no 'sim_order' string in the 'net2code -sim_order' output:\n\n $message_input" \
             "" 0 {OK} 
        return 0
  }
  set sim_order_string [string range $message_input [expr ${cur_index} + 10] end]

###########################################################

set base_dir_index [string first "/SueLib/" $SUE2_CELL_FILENAME]
set base_dir [string range $SUE2_CELL_FILENAME 0 [expr ${base_dir_index} - 1]]

set cur_dir "$base_dir/CadenceLib"

if {[file isdirectory $cur_dir] == 0} {
     tk_dialog_new .cppsim_message "Error in Creating CppSim Code" \
	   "CadenceLib directory is missing!\n   -> you need to create directory:\n     $cur_dir" "" 0 {OK}
     return 0
}

set cpp_lib [clean_cpp_lib_name $CPPSIM_LIBRARY]

set cur_dir "$base_dir/CadenceLib/$cpp_lib"
if {[file isdirectory $cur_dir] == 0} {
    if {[catch {file mkdir $cur_dir} message]} {
      tk_dialog_new .cppsim_message "Error in creating CppSim Code" \
	  "$message" "" 0 {OK} 
      return 0
    }
}

set cur_dir "$base_dir/CadenceLib/$cpp_lib/$CPPSIM_CELL"
if {[file isdirectory $cur_dir] == 0} {
    if {[catch {file mkdir $cur_dir} message]} {
      tk_dialog_new .cppsim_message "Error in creating CppSim Code" \
	  "$message" "" 0 {OK} 
      return 0
    }
}

set cur_dir "$base_dir/CadenceLib/$cpp_lib/$CPPSIM_CELL/cppsim"
if {[file isdirectory $cur_dir] == 0} {
    if {[catch {file mkdir $cur_dir} message]} {
      tk_dialog_new .cppsim_message "Error in creating CppSim Code" \
	  "$message" "" 0 {OK} 
      return 0
    }
}

set cppsim_code_file "$base_dir/CadenceLib/$cpp_lib/$CPPSIM_CELL/cppsim/text.txt"
if {[catch "set fid \[open $cppsim_code_file w\]" err]} {
   tk_dialog_new .cppsim_message "Error in creating CppSim Code" \
	  "$err" "" 0 {OK} 
   return 0
}

puts $fid "module: $CPPSIM_CELL"
puts $fid "sim_order: $sim_order_string"

if {[catch {close $fid} err]} {
   tk_dialog_new .cppsim_message "Error in creating CppSim Code" \
	  "$err" "" 0 {OK} 
   return 0
}

return 1

}


############## Create Module Label ################
proc cppCreateModuleLabel {cur_mod} {

  global CPPSIM_CELL CPPSIM_LIBRARY CPPSIMSHARED_HOME \
         CPPSIM_HOME tcl_platform SUE2_CELL_FILENAME

  set CPPSIM_CELL $cur_mod

  if {$tcl_platform(platform) == "windows"} {
     set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin/win32"
  } else {
     set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin"
  }

  if {[catch "exec $sue2_bin_directory/find_sue_module_library $CPPSIM_CELL \
                 $CPPSIM_HOME/Sue2/sue.lib" library_name_message] != 0} {
     tk_dialog_new .cppsim_message "Error in $CPPSIM_HOME/Sue2/sue.lib file" \
	     "$library_name_message" "" 0 {OK} 
  }

  set last_index [string last "file:" $library_name_message]
  set CPPSIM_LIBRARY [string range $library_name_message 0 [expr ${last_index} - 2]]
  set SUE2_CELL_FILENAME [string range $library_name_message [expr ${last_index} + 6] end]
  if {[string first "Error:" $CPPSIM_LIBRARY] != -1} {
      tk_dialog_new .cppsim_message "Error in finding CppSim module" \
	  "$CPPSIM_LIBRARY" "" 0 {OK} 
  }
  
  if {[string first "Error:" $CPPSIM_LIBRARY] == -1} {
     set out_text "Cell: $CPPSIM_CELL\nLibrary: $CPPSIM_LIBRARY\n"
  } else {
     set out_text "$CPPSIM_LIBRARY\n"
  }
  return $out_text
}    


# Set up event bindings for the window

proc setup_bindings {} {

  global cur_c scale WIN tcl_platform
  global SNAP_XY NOSNAP_XY MODE KEYS

  # Bindings for background window (i.e. empty piece of canvas)
  bind $cur_c <Button-1> "icon_select_and_setup_select_region $NOSNAP_XY $SNAP_XY"
  bind $cur_c <Shift-Button-1> "icon_select_and_setup_select_region $NOSNAP_XY $SNAP_XY add"

  # Bindings for mouse buttons and mouse drags on objects
  foreach type {icon wire open dot draw_item} {
#      $cur_c bind $type <Button-1> "icon_select $SNAP_XY"
      $cur_c bind $type <B1-Motion> "setup_drag_move_mode $SNAP_XY"
      $cur_c bind $type <$KEYS(pan)> "setup_pan_canvas $SNAP_XY"
#      $cur_c bind $type <Shift-Button-1> "icon_select $SNAP_XY add"
#      $cur_c bind $type <Button-2> "icon_select $SNAP_XY special"
#      $cur_c bind $type <Shift-Button-2> "icon_select $SNAP_XY add"
  }
  
  # Button-3 is hard coded to move
#  bind $cur_c <Any-Button-3> "setup_move_mode $SNAP_XY"
  
  # move edit markers and auto-scroll with button-1
  $cur_c bind edit_marker <Button-1> "marker_press $SNAP_XY"
  $cur_c bind edit_marker <B1-Motion> \
      "marker_drag $SNAP_XY; set SCROLL(status) on; auto_scroll \[incrX SCROLL(mem)\] $SNAP_XY"
  $cur_c bind edit_marker <B1-ButtonRelease> \
      "marker_release; set SCROLL(status) off"
  
  # bindings all canvas items to highlight when you are over them
  foreach type {icon wire open dot draw_item edit_marker} {
    $cur_c bind $type <Any-Enter> {item_enter_or_leave "enter"}
#    $cur_c bind $type <Any-Leave> {item_enter_or_leave "leave"}
  }
  
  # hot key bindings

  bind $cur_c <Left> "$cur_c xview scroll -1 units"
  bind $cur_c <Right> "$cur_c xview scroll 1 units"
  bind $cur_c <Up> "$cur_c yview scroll -1 units"
  bind $cur_c <Down> "$cur_c yview scroll 1 units"
#  bind $cur_c <KeyPress> "puts %K"

  bind $cur_c <$KEYS(delete_selected)> "delete_selected"
  bind $cur_c <Delete> "delete_selected"
  # L10 is the "Cut" key
#  bind $cur_c <L10> "delete_selected"

  bind $cur_c <$KEYS(paste)> "setup_paste_mode"
  # L8 is the "Paste" key
  bind $cur_c <L8> "setup_paste_mode"

  bind $cur_c <$KEYS(duplicate_selected)> "setup_duplicate_selected"
  bind $cur_c <$KEYS(show_anchors)> "show_anchors"
  bind $cur_c <$KEYS(show_term_names)> "show_icon_term_names"
  bind $cur_c <$KEYS(lower_selected)> {$cur_c lower selected}
  # L6 is the "Copy" key
  bind $cur_c <L6> "setup_duplicate_selected"
  bind $cur_c <$KEYS(replace_instance)> "setup_replace_instance"

  bind $cur_c <$KEYS(duplicate_text)> "setup_duplicate_selected text"

  bind $cur_c <$KEYS(load)> "load_schematic"
  # L7 is the "open" key
  bind $cur_c <L7> "load_schematic"

  bind $cur_c <$KEYS(raise_windows)> {map_others "" raise} 

  bind $cur_c <$KEYS(print)> "make_ps_simplified"

  bind $cur_c "$KEYS(undo)" "undo_last"
  # L4 is the "undo" key
  bind $cur_c <L4> "undo_last"

  bind $cur_c <$KEYS(copy_to_clipboard)> \
      "delete_selected_undo copy_to_clipboard"
  bind $cur_c <$KEYS(move_selected)> {setup_move_mode}
  bind $cur_c <$KEYS(flip_horizontal)> {transform_selected MX}
  bind $cur_c <$KEYS(flip_vertical)> {transform_selected MY}
  bind $cur_c <$KEYS(rotate)> {transform_selected ROTATE}
  bind $cur_c <$KEYS(push)> {push_into_schematic}
  bind $cur_c <$KEYS(pop)> {pop_out_of_schematic}
  bind $cur_c <$KEYS(exit)> {modify_exit}
  bind $cur_c <$KEYS(swap_views)> {change_views}
  bind $cur_c <$KEYS(make_icon)> {make_icon}
  bind $cur_c <$KEYS(new_schematic)> "make_new_schematic"
  bind $cur_c <$KEYS(save)> {write_file $cur_s}
  bind $cur_c <$KEYS(modified_save_and_leaves)> \
      {modified_save_and_leaves $cur_s}
  bind $cur_c <Any-KeyPress-$KEYS(zoom_in)> {zoom 1.3}
  bind $cur_c <Any-KeyPress-$KEYS(zoom_out)> {zoom 0.8}
  bind $cur_c <Any-KeyPress-6> "zoom_to_fit"
  bind $cur_c <$KEYS(zoom_fit)> "zoom_to_fit"
  bind $cur_c <$KEYS(pan)> "setup_pan_canvas $SNAP_XY"
#  bind $cur_c <$KEYS(zoom_box)> "setup_zoom_box"
  if {$tcl_platform(os) == "Darwin"} {
     bind $cur_c <Button-2> "begin_zoom_box $SNAP_XY"
  } else {
     bind $cur_c <Button-3> "begin_zoom_box $SNAP_XY"
  }
  bind $cur_c <$KEYS(add_line)> "setup_line_mode"
  bind $cur_c <$KEYS(add_text)> "setup_text_mode"
  bind $cur_c <$KEYS(add_arc)> "setup_arc_mode"
  bind $cur_c <$KEYS(add_wire)> "setup_draw_wire"
  bind $cur_c <$KEYS(grid)> "toggle_grid"
  bind $cur_c <$KEYS(change_grid)> "change_grid"

  bind $cur_c <$KEYS(netlist)> {create_spice_netlist no_top_sub}
#  bind $cur_c <$KEYS(spice_it)> {${NETLIST_TYPE}_it}
#  bind $cur_c <$KEYS(init_probe)> {${PROBE_TYPE}_init_probe}
#  bind $cur_c <$KEYS(plot_net)> {${PROBE_TYPE}_plot_net}
#  bind $cur_c <$KEYS(unplot_net)> {${PROBE_TYPE}_unplot_net}
#  bind $cur_c <$KEYS(plot_net_remember)> {${PROBE_TYPE}_plot_net_and_remember}
#  bind $cur_c <$KEYS(unplot_net_forget)> {${PROBE_TYPE}_unplot_net_and_forget}
#  bind $cur_c <$KEYS(plot_old_net)> {${PROBE_TYPE}_plot_net ~}
#  bind $cur_c <$KEYS(unplot_old_net)> {${PROBE_TYPE}_unplot_net ~}

#  bind $cur_c $KEYS(irsim_step) "irsim_step"
#  bind $cur_c $KEYS(irsim_step_update) "irsim_step_update"
#  bind $cur_c $KEYS(irsim_set_hi) "irsim_set h"
#  bind $cur_c $KEYS(irsim_set_low) "irsim_set l"
#  bind $cur_c $KEYS(irsim_set_x) "irsim_set u"
#  bind $cur_c $KEYS(irsim_query_net) "irsim_set ?"
#  bind $cur_c $KEYS(enter_irsim_mode) "enter_irsim_mode"

  # double clicking the button-1 on an inst will let us edit the properties
  # Input x and y should NOT go through canvasx,canvasy commands

  $cur_c bind icon <Double-Button-1> {prop_edit_menu %x %y}

  $cur_c bind icon <Shift-Double-Button-1> {generator_edit_menu %x %y}
  $cur_c bind draw_item <Double-Button-1> {edit_draw_item %x %y}

  # double click will select entire wire, no just current segment
  foreach type {wire dot open} {
    $cur_c bind $type <Double-Button-1> {select_entire_wire branch}
    $cur_c bind $type <Shift-Double-Button-1> {select_entire_wire}
  }
}


# Displays/changes the title to include the current cell and flags.

proc display_title {{win ""}} {

  global cur_s WIN CPPSIM_HOME

  if {$win == ""} {
    set win $WIN
  }

  upvar #0 SUE_$cur_s data

  if {[string compare $data(modified) "M"] == 0} {
     set modified "** MODIFIED ** "
  } else {
     set modified ""
  }
  if {[string compare $data(type) "S"] == 0} {
     set type "(schematic)"
  } else {
     set type "(icon)"
  }
  set written $data(written)
  set filename [use_first data(filename)]
  set schematic [get_rootname $cur_s]

  set filename_suffix [string last "/" $filename]
  set cell_name [string range $filename [expr ${filename_suffix} + 1] end]
  if {[string compare $cell_name "no_name.sue"] != 0} {
     wm title $win "SUE2:  $schematic  $type $modified ---  $filename"

     set cur_cell_filename "$CPPSIM_HOME/Sue2/current_cell.cppsimview"

     if {[catch "set CUR_CELL_FILE_ID \[open $cur_cell_filename w\]"] != 0} {
        puts "Aborting, could not create file $cur_cell_filename"
        ready
        return
     }

     set suffix_index [string last ".sue" $filename]
     set filename_nosuffix [string range $filename 0 [expr ${suffix_index} - 1]]
     puts $CUR_CELL_FILE_ID $filename_nosuffix

     ###### Close file
     close $CUR_CELL_FILE_ID

  } else {
     wm title $win "SUE2:  $schematic  $type $modified ---  this is a scratch cell:  rename to desired cell name using 'save as' command"
  }
  wm iconname $win "SUE2:${schematic}_$type"
}


# makes a listbox of all the icons that are around.

if {$ICON_MENU == "flat"} {

proc make_icon_listbox {{bogus ""}} {

  global GEOMETRY WIN tcl_platform

  set win .icons

  if {[winfo exists $win] == 1} {
    # clean out old icon listbox
    $win.nodes delete 0 end

  } else {

    # build a toplevel window
    toplevel $win 
    wm geometry $win $GEOMETRY(icons)
    wm title $win "icons"
    wm min $win 0 0
    wm group $win $WIN

    bind $win <Unmap> {map_others %W "wm iconify"}
    bind $win <Map> {map_others %W "wm deiconify"}

    scrollbar $win.scroll -command "$win.nodes yview"
    pack $win.scroll -side right -fill y
    listbox $win.nodes -yscrollcommand "$win.scroll set"
    pack $win.nodes -side left -fill both -expand 1

    # need to use a fixed width font here
    global LISTBOX_FONT
    $win.nodes configure -font $LISTBOX_FONT

#    listbox $win.nodes

    set selected \
	"\[string range \[$win.nodes get \[$win.nodes index active\]\] 2 end\]"

    bind $win.nodes <Motion> {%W activate [%W nearest %y]} 
    # single click on button-1 drops into current schematic
    bind $win.nodes <Button-1> "setup_drop_icon $selected %W"
    # single click on button-2 goes to as if a schematic
    if {$tcl_platform(os) == "Darwin"} {
        bind $win.nodes <Button-2> "goto_schematic ICON_$selected %W"
    } else {
        bind $win.nodes <Button-3> "goto_schematic ICON_$selected %W"
    }
    # double click button-3 deletes
#    bind $win.nodes <Double-Button-3> "delete_schematic ICON_$selected"

  }

  # Now put the icon list into it
  foreach icon [lsort [info commands ICON_*]] {
    global SUE_$icon
    if {[info exists SUE_${icon}(modified)]} {
      set prefix [string range "[set SUE_${icon}(modified)]  " 0 1]
    } else {
      set prefix "  "
    }

    $win.nodes insert end $prefix[string range $icon 5 end]
  }
}

} elseif {$ICON_MENU == "hier"} {

# makes a hierarchical listbox of all the icons based on directory from
# which they come.

proc make_icon_listbox {{dir ""} {win ""}} {
  
  global GEOMETRY WIN auto_index ICON_WINDOWS tcl_platform

  set win [use_first win '.[lindex $ICON_WINDOWS 0]]
  set win_name [string range $win 1 end]

  if {[winfo exists $win] == 1} {
    # clean out old icon listbox of icons
    $win.nodes delete 0 end
    # clean out old icon listbox of directories.  Wish end worked here too.
    $win.dir.other delete 0 1000

  } else {
    # build a toplevel window
    toplevel $win 
    # use the geometry if it is available.  Otherwise wish picks it.
    if {[info exists GEOMETRY($win_name)]} {
      wm geometry $win $GEOMETRY($win_name)
    }
    wm title $win $win_name
    wm min $win 0 0
    wm group $win $WIN

    global LISTBOX_FONT

    bind $win <Unmap> {map_others %W "wm iconify"}
    bind $win <Map> {map_others %W "wm deiconify"}

    set dir [clean_dir [pwd]]
    # if not the first icon listbox, find a directory other than pwd
    if {[lindex $ICON_WINDOWS 0] != $win_name} {
      set win1 .[lindex $ICON_WINDOWS 0]
      set win_search_val [lsearch $ICON_WINDOWS $win_name]
      set index [expr 2 + ${win_search_val}]
#   fix this later
#      set dir [lindex [$win1.dir.other entryconfigure $index -label] 4]
    }

    menubutton $win.dir -text $dir -menu $win.dir.other \
	-font $LISTBOX_FONT -relief raised -bd 2 -anchor center
    pack $win.dir -side top -fill x

    menu $win.dir.other

    scrollbar $win.scroll -command "$win.nodes yview"
    pack $win.scroll -side right -fill y
    listbox $win.nodes -yscrollcommand "$win.scroll set"
    pack $win.nodes -side left -fill both -expand 1

    # need to use a fixed width font here
    $win.nodes configure -font $LISTBOX_FONT

#    listbox $win.nodes

    set selected \
	"\[string range \[$win.nodes get \[$win.nodes index active\]\] 2 end\]"

    bind $win.nodes <Motion> {%W activate [%W nearest %y]} 
    # single click on button-1 drops into current schematic
    bind $win.nodes <Button-1> "setup_drop_icon $selected %W"
    bind $win.nodes <Double-Button-1> "cppsim_code_menu $selected; abort_icon_mode"
    # single click on button-3 goes to as if a schematic
    if {$tcl_platform(os) == "Darwin"} {
       bind $win.nodes <Button-2> "goto_schematic ICON_$selected %W"
    } else {
       bind $win.nodes <Button-3> "goto_schematic ICON_$selected %W"
    }
    # double click button-3 deletes
#    bind $win.nodes <Double-Button-3> "delete_schematic ICON_$selected"
  }

  # set the directory of which icons to show.  Lose auto_mounter stuff
  set dir [use_first dir '[lindex [$win.dir configure -text] 4]]
  # special case for the "generators" dir
  if {$dir != "generators"} {
    set dir [clean_dir $dir]
  }
  $win.dir configure -text $dir

  # put in a menu item for remove icon menu and add another
  $win.dir.other add command -label "Update listboxes" \
      -command "update_listboxes"
  $win.dir.other add command -label "Make new icon listbox" \
      -command "new_icon_menu"
  $win.dir.other add command -label "Close this icon listbox" \
      -command "waste_icon_listbox $win"
  $win.dir.other add separator

  # "." means not in the auto path
  set dirs(.) foo

  # Now put the icon list into it
  foreach icon [lsort [info commands ICON_*]] {
    set icon_dir [file dirname [lindex [use_first auto_index($icon)] 1]]
    if {$icon_dir == $dir} {
      global SUE_$icon
      if {[info exists SUE_${icon}(modified)]} {
	set prefix [string range "[set SUE_${icon}(modified)]  " 0 1]
      } else {
	set prefix "  "
      }
      
      $win.nodes insert end $prefix[string range $icon 5 end]
    }


    # add directories
    if {![info exists dirs($icon_dir)]} {
      $win.dir.other add command -label $icon_dir \
	  -command "make_icon_listbox $icon_dir $win"
      set dirs($icon_dir) 1
    }
  }

  # now update any other icon listboxes that may be around
  set win_search_val [lsearch $ICON_WINDOWS $win_name]
  set new_win [lindex $ICON_WINDOWS \
		   [expr ${win_search_val} + 1]]
  if {$new_win == ""} {
    # we're done
    return
  }
  make_icon_listbox "" .$new_win

}

} else {

# makes a hierarchical listbox of all the icons based on directory from
# which they come and INCLUDE listbox in the main SUE window.

proc make_icon_listbox {{dir ""} {win ""}} {
  
  global GEOMETRY WIN auto_index ICON_WINDOWS LISTBOX_FONT WIN_DATA tcl_platform

  if {[info exists WIN_DATA(make_icon_listbox)]} {
    # postpone this
    incr WIN_DATA(make_icon_listbox)
    return
  }

  if {$win == ""} {
    set win "$WIN.lb.[lindex $ICON_WINDOWS 0]"
  }

  if {[lrange [split $win .] 1 2] != "[string range $WIN 1 end] lb"} {
    set win "$WIN.lb$win"
  }

  set win_name [string range [file extension $win] 1 end]
  set scroll 0

  if {[winfo exists $win]} {
    if {$dir == ""} {
      # first remember where the scroll bar was to return there
      set scroll [lindex [$win.scroll get] 0]
    }
    # clean out old icon listbox of icons
    $win.nodes delete 0 end
    # clean out old icon listbox of directories.  Wish end worked here too.
    $win.dir.other delete 0 1000

  } else {
    # make a new listbox
    frame $win -relief sunken -bd 2
    
    set win_name [lindex [split $win .] 3]
    set index [expr [lsearch -exact $ICON_WINDOWS $win_name] + 1]
    set before "$WIN.lb.[lindex $ICON_WINDOWS $index]"
    if {$index == 0 || $index >= [llength $ICON_WINDOWS] || \
	    ![winfo exists $before]} {
      pack $win -side top -fill y -expand 1
    } else {
      # pack in correct place
      pack $win -side top -before $before -fill y -expand 1
    }

    if {$dir == ""} {
      set dir [clean_dir [pwd]]
      # if not the first icon listbox, find a directory other than pwd
      if {[lindex $ICON_WINDOWS 0] != $win_name} {
	set win1 "$WIN.lb.[lindex $ICON_WINDOWS 0]"
	set index [expr 4 + [lsearch $ICON_WINDOWS $win_name]]
	if {![catch "$win1.dir.other entrycget $index -label" msg]} {
	  # success
	  set dir $msg
	}
      }
    }

    # make sure this window is the size of the other ones.
    if {[catch "$WIN.lb.[lindex $ICON_WINDOWS 0].nodes cget -width" width]} {
      # problem, probably doesn't exist, use default
      set width $GEOMETRY(listbox)
    }
    if {$width < 3} {
      set width $GEOMETRY(listbox)
    }

    set base_dir_index [string first "/SueLib/" $dir]
    set lib_name [string range $dir [expr ${base_dir_index} + 8] end]

    menubutton $win.dir -text $lib_name -menu $win.dir.other -font $LISTBOX_FONT \
	-relief raised -bd 2 -anchor center -width $width \
	-padx 2 -pady 2

    pack $win.dir -side top -fill x

    menu $win.dir.other -tearoff 0

    scrollbar $win.scroll -command "$win.nodes yview" -highlightthickness 0
    pack $win.scroll -side right -fill y -expand 1
    listbox $win.nodes -yscrollcommand "$win.scroll set" \
	-width $width -highlightthickness 0 -height 4 \
	-exportselection 0 -selectmode single
    pack $win.nodes -side left -fill both -expand 1

    # need to use a fixed width font here so modified "M" looks right
    $win.nodes configure -font $LISTBOX_FONT

#    tk_listboxSingleSelect $win.nodes

#    set selected "\[string range \[$win.nodes get \[$win.nodes curselection\]\] 2 end\]"

    set selected \
	"\[string range \[$win.nodes get \[$win.nodes index active\]\] 2 end\]"

#    set selected [backquote \
#      {[if {[set sel_index [$$win.nodes curselection]] != ""} { \
#	concat [string range [$$win.nodes get $sel_index] 2 end] \
#      }] \
#    }]

    # help
    # bind $win.nodes <space> "help_window %x %y"

#    bind $win.nodes <Any-Enter> \
#     {msg_window "Icon Listbox: Button-1 adds, Shift-Button-1 goes to, Button-2 go to icon view, Double-Button-3 deletes" listbox}
#    bind $win.nodes <Any-Leave> "msg_window __RESTORE__"

    bind $win.nodes <Motion> {%W selection clear 0 end; %W selection set [%W nearest %y]}

    bind $win.nodes <Leave> "listbox_clear_selection $win.nodes"
    # single click on button-1 drops into current schematic
    bind $win.nodes <Button-1> "setup_drop_icon $selected %W"
    bind $win.nodes <Double-Button-1> "cppsim_code_menu $selected; abort_icon_mode"

    # bind $win.nodes <Shift-Button-1> "goto_schematic $selected %W"
    # bind $win.nodes <Button-2> "goto_schematic ICON_$selected %W"

    # single click on button-3 goes to as if a schematic
    if {$tcl_platform(os) == "Darwin"} {
       bind $win.nodes <Button-2> "goto_schematic ICON_$selected %W"
    } else {
       bind $win.nodes <Button-3> "goto_schematic ICON_$selected %W"
    }

    # double click button-3 deletes
    # bind $win.nodes <Double-Button-3> "delete_schematic ICON_$selected"

    # scrolling hotkeys - only used for help_window because of focus
    # see the scroll_listbox command
#    bind $win.nodes <Next> "$win.nodes yview scroll 1 pages"
#    bind $win.nodes <Prior> "$win.nodes yview scroll -1 pages"

#    bind $win.nodes <Down> "$win.nodes yview scroll 1 units"
#    bind $win.nodes <Up> "$win.nodes yview scroll -1 units"
  }

  # set the directory of which icons to show.  Lose auto_mounter stuff
  set dir [use_first dir '[$win.dir cget -text]]


  # special case for the "generators" dir
  if {$dir != "generators"} {
    set dir [clean_dir $dir]
  }

  set base_dir_index [string first "/SueLib/" $dir]
  if {$base_dir_index < 0} {
     set lib_name $dir
  } else {
     set lib_name [string range $dir [expr ${base_dir_index} + 8] end]
  }

  $win.dir configure -text $lib_name

  $win.dir.other add command -label "Autoload directory" \
      -command "autoload_from_listbox $dir"
  $win.dir.other add separator

  # put in a menu item for remove icon menu and add another
  $win.dir.other add command -label "Make new icon listbox" \
      -command "new_icon_menu"
  $win.dir.other add command -label "Close this icon listbox" \
      -command "waste_icon_listbox $win"
  $win.dir.other add separator

  # "." means not in the auto path
  set dirs(.) foo



  # Now put the icon list into it
  foreach icon [lsort [info commands ICON_*]] {
    set icon_dir [file dirname [lindex [use_first auto_index($icon)] 1]]

    set base_dir_index [string first "/SueLib/" $icon_dir]
      if {$base_dir_index < 0} {
         set icon_dir_libname ""
      } else {
         set icon_dir_libname [string range $icon_dir [expr ${base_dir_index} + 8] end]
      }

    if {$icon_dir == $dir} {
      global SUE_$icon
      if {[info exists SUE_${icon}(modified)]} {
	set prefix [string range "[set SUE_${icon}(modified)]  " 0 1]
      } else {
	set prefix "  "
      }

      $win.nodes insert end $prefix[string range $icon 5 end]
    } elseif {$icon_dir_libname == $dir} {
      global SUE_$icon
      if {[info exists SUE_${icon}(modified)]} {
	 set prefix [string range "[set SUE_${icon}(modified)]  " 0 1]
      } else {
  	set prefix "  "
      }

      $win.nodes insert end $prefix[string range $icon 5 end]
    } 


    # add directories
    if {![info exists dirs($icon_dir)]} {
      $win.dir.other add command -label $icon_dir \
	  -command "make_icon_listbox $icon_dir $win"
      set dirs($icon_dir) 1
    }
  }

  # put the scrollbar back where it was
  $win.nodes yview moveto $scroll
  $win.nodes xview moveto 0

  # now update any other icon listboxes that may be around
  set new_win [lindex $ICON_WINDOWS \
		   [expr [lsearch $ICON_WINDOWS $win_name] + 1]]
  if {$new_win == ""} {
    # we're done
    return
  }
  make_icon_listbox "" ".$new_win"
}

# resizes width of included listboxes

proc resize_listboxes {delta} {

  global WIN ICON_WINDOWS

  set old_width [$WIN.lb.schematics.nodes cget -width]
  foreach icon $ICON_WINDOWS {
      set icon_width [$WIN.lb.$icon.nodes cget -width]
      if {$icon_width < $old_width} {
         set old_width $icon_width
      }
  }

  set width [expr $old_width + $delta]

  if {$width < 3} {
    # too small
    return
  }

  set swidth [min [expr $width + 5] 15]

  $WIN.lb.schematics.nodes configure -width $width
  $WIN.lb.schematics.dir configure -width $width
  $WIN.lb.schematics.s.scroll configure -width $swidth

  foreach icon $ICON_WINDOWS {
    $WIN.lb.$icon.nodes configure -width $width
    $WIN.lb.$icon.dir configure -width $width

    $WIN.lb.$icon.scroll configure -width $swidth
  }
}

# end include listbox
}







# Reload cells listed in sue.lib file  
proc update_listboxes {} {

global SUE_LIB_DIR


set sue_lib_file_name $SUE_LIB_DIR/sue.lib
# puts "file = $sue_lib_file_name"
if {[catch {set sue_lib_file [open $sue_lib_file_name r]} error]} {
    # failed, file probably doesn't exist
    set button [tk_dialog_new .sue_lib_issue "Error: sue.lib not found!" \
		    "Error: $error" \
		    "" 0 {Exit}]
    modify_exit
}

#  set initial schematic listbox directory based on first
#  line in sue.lib file


global INIT_SCHEM_DIR
set found_first_valid_dir_flag 0

make_icon_listbox
while {[gets $sue_lib_file line] >= 0} {
    if {[catch {glob $CPPSIM_HOME/SueLib/$line/*.sue} error] != 1} {
       add_auto_path $CPPSIM_HOME/SueLib/$line
       foreach file [glob $CPPSIM_HOME/SueLib/$line/*.sue] {
          source $file
       }
       if {$found_first_valid_dir_flag == 0} {
           set found_first_valid_dir_flag 1
           set INIT_SCHEM_DIR $CPPSIM_HOME/SueLib/$line           
       } elseif {$found_first_valid_dir_flag == 1} {
           set found_first_valid_dir_flag 2
           make_icon_listbox $CPPSIM_HOME/SueLib/$line .icons1 
       } elseif {$found_first_valid_dir_flag == 2} {
           set found_first_valid_dir_flag 3
           make_icon_listbox $CPPSIM_HOME/SueLib/$line .icons2 
       }
    } elseif {[catch {glob $CPPSIMSHARED_HOME/SueLib/$line/*.sue} error] != 1} {
       add_auto_path $CPPSIMSHARED_HOME/SueLib/$line
       foreach file [glob $CPPSIMSHARED_HOME/SueLib/$line/*.sue] {
          source $file
       }
       if {$found_first_valid_dir_flag == 0} {
           set found_first_valid_dir_flag 1
           set INIT_SCHEM_DIR $CPPSIMSHARED_HOME/SueLib/$line           
       } elseif {$found_first_valid_dir_flag == 1} {
           set found_first_valid_dir_flag 2
           make_icon_listbox $CPPSIMSHARED_HOME/SueLib/$line .icons1 
       } elseif {$found_first_valid_dir_flag == 2} {
           set found_first_valid_dir_flag 3
           make_icon_listbox $CPPSIMSHARED_HOME/SueLib/$line .icons2 
       }
    }
}
close $sue_lib_file


#make_icon_listbox C:/home/perrott/Sue2/SueLib/CppSimModule .icons1

make_schematic_listbox
#use_first auto_index(ICON_global2)
}


proc new_icon_menu {} {

  global ICON_WINDOWS

  # make up a name.
  set root [lindex [split [lindex $ICON_WINDOWS 0] 0123456789] 0]
  for {set index 0} {[lsearch $ICON_WINDOWS $root[incr index]] != -1} {} {}
  set win_name $root$index

  lappend ICON_WINDOWS $win_name

  # and do it
  make_icon_listbox "" .$win_name
}


proc waste_icon_listbox {win} {

  global ICON_WINDOWS


  if {[llength $ICON_WINDOWS] < 2} {
    puts "Aborted.  Can't remove last icon lisbox."
    return
  }

  # first remove the window out of ICON_WINDOWS
  set base_win_index [string last ".lb." $win]
  if {$base_win_index < 0} {
     set win_name ""
  } else {
     set win_name [string range $win [expr ${base_win_index} + 4] end]
  }

  set index [lsearch $ICON_WINDOWS $win_name]
  set ICON_WINDOWS [lreplace $ICON_WINDOWS $index $index]

  # then nuke the window
  destroy $win
}

if {$ICON_MENU == "flat" || $ICON_MENU == "hier"} { 

# makes a listbox of all schematics that are in canvases, NOT all 
# schematics.  For example, a schematic procedure might be known but
# since no one has pushed into it, a canvas hasn't been made for it
# and thus it won't show up here.

proc make_schematic_listbox {{dir ""} {win ""}} {

  global SUE GEOMETRY WIN auto_index saved_dir INIT_SCHEM_DIR

  set win [use_first win '.schematics]

  if {[winfo exists $win] == 1} {
     # clean out old schematic listbox entries
     $win.nodes delete 0 end
     # clean out old schematic listbox directories
     $win.dir.other delete 0 1000
  } else {
     # build a top level window
     toplevel $win 
     wm geometry $win $GEOMETRY(schematics)
     wm title $win "schematics"
     wm min $win 0 0
     wm group $win $WIN

     global LISTBOX_FONT

     bind $win <Unmap> {map_others %W "wm iconify"}
     bind $win <Map> {map_others %W "wm deiconify"}

     set dir [clean_dir [pwd]]
     menubutton $win.dir -text $dir -menu $win.dir.other \
        -font $LISTBOX_FONT -relief raised -bd 2 -anchor center
     pack $win.dir -side top -fill x

     menu $win.dir.other

     scrollbar $win.scroll -command "$win.nodes yview"
     pack $win.scroll -side right -fill y
     listbox $win.nodes -yscrollcommand "$win.scroll set"
     pack $win.nodes -side left -fill both -expand 1


     # need to use a fixed width font here
     $win.nodes configure -font $LISTBOX_FONT

     #  listbox $win.nodes

     set selected \
       "\[string range \[$win.nodes get \[$win.nodes index active\]\] 2 end\]"

     bind $win.nodes <Motion> {%W activate [%W nearest %y]} 
     # single click selects
     bind $win.nodes <Double-Button-1> "cppsim_code_menu $selected"
     bind $win.nodes <Button-1> "goto_schematic $selected %W"
 }

  if {[info exists saved_dir]} {
     set dir [use_first dir '$saved_dir]
  } else {
     set dir $INIT_SCHEM_DIR
  }
  set saved_dir $dir

  $win.dir configure -text $dir
  # put in menu items
  $win.dir.other add command -label "Update listboxes" \
       -command "update_listboxes"
  $win.dir.other add separator

  # "." means not in the auto path
  set dirs(.) foo

  # Now put the schematic list into it
  foreach schematic [lsort [info commands SCHEMATIC_*]] {
     set schem_dir [file dirname [lindex [use_first auto_index($schematic)] 1]]
     if {$schem_dir == $dir} {
         global SUE_$schematic
         if {[info exists SUE_${schematic}(modified)]} {
	    set prefix [string range "[set SUE_${schematic}(modified)]  " 0 1]
         } else {
	    set prefix "  "
         }
         if {[string compare $schematic "SCHEMATIC_no_name"] != 0} {
	     $win.nodes insert end $prefix[string range $schematic 10 end]
        }
     }
  # add directories
      if {![info exists dirs($schem_dir)]} {
         $win.dir.other add command -label $schem_dir \
             -command "make_schematic_listbox $schem_dir $win"
         set dirs($schem_dir) 1
      }

  }

}

} else {

# Same as above except INCLUDES list box is in main window

proc make_schematic_listbox {{dir ""} {win ""}} {

  global SUE GEOMETRY WIN LISTBOX_FONT ICON_WINDOWS auto_index saved_dir INIT_SCHEM_DIR

  if {[info exists saved_dir]} {
      if {$dir == ""} {
         return
      }
  }
  set win $WIN.lb.schematics

  if {[winfo exists $WIN.lb.schematics]} {
     $win.nodes delete 0 end
     $win.dir.other delete 0 1000
  } else {

  frame $win -relief sunken -bd 2
  set before "$WIN.lb.[lindex $ICON_WINDOWS 0]"
  if {![winfo exists $before]} {
      pack $win -side top -fill y -expand 1
    } else {
      # pack in correct place
      pack $win -side top -before $before -fill y -expand 1
    }

  # make sure this window is the size of the other ones.
  if {[catch "$WIN.lb.[lindex $ICON_WINDOWS 0].nodes cget -width" width]} {
      # problem, probably doesn't exist, use default
      set width $GEOMETRY(listbox)
  }
  if {$width < 3} {
      set width $GEOMETRY(listbox)
  }

  set swin $win.s
  frame $swin
  pack $swin -side right -fill y

#  button $swin.button -text "a" -command "alphabetize_schematic_listbox" \
#      -padx 0 -pady 0  
#  pack $swin.button -side top

  set dir [clean_dir [pwd]]
  menubutton $win.dir -text $dir -menu $win.dir.other \
        -font $LISTBOX_FONT -relief raised -bd 2 -anchor center -width $width -padx 2 -pady 2
  pack $win.dir -side top -fill x

  menu $win.dir.other -tearoff 0

  scrollbar $swin.scroll -command "$win.nodes yview" -highlightthickness 0
  pack $swin.scroll -side top -fill y -expand 1
  listbox $win.nodes -yscrollcommand "$swin.scroll set" \
      -width $width -highlightthickness 0 -height 4 \
      -exportselection 0 -selectmode single
  pack $win.nodes -side left -fill both -expand 1

  # need to use a fixed width font here
  $win.nodes configure -font $LISTBOX_FONT

#  tk_listboxSingleSelect $win.nodes

    set selected \
       "\[string range \[$win.nodes get \[$win.nodes index active\]\] 2 end\]"

#    set selected [backquote \
#      {[if {[set sel_index [$$win.nodes curselection]] != ""} { \
#	concat [string range [$$win.nodes get $sel_index] 2 end] \
#      }] \
#    }]

  bind $win.nodes <Motion> \
      {%W selection clear 0 end; %W selection set [%W nearest %y]} 

  bind $win.nodes <Leave> "listbox_clear_selection $win.nodes"

  # single click selects
  #bind $win.nodes <Button-1> "goto_schematic $selected %W"
  bind $win.nodes <Button-1> "goto_schematic $selected %W"
  bind $win.nodes <Double-Button-1> "cppsim_code_menu $selected"

  # help
  # bind $win.nodes <space> "help_window %x %y"

  #  bind $win.nodes <Any-Enter> \
      {msg_window "Schematic Listbox: Button-1 goes to, Shift-Button-1 goes to icon, Control-Button-1 adds, Double-Button-3 deletes" listbox}
  #  bind $win.nodes <Any-Leave> "msg_window __RESTORE__"

  # bind $win.nodes <Shift-Button-1> "goto_schematic $selected %W ; change_views"

  # bind $win.nodes <Control-Button-1> "setup_drop_icon $selected %W"

  # double click button-3 deletes
  # bind $win.nodes <Double-Button-3> "delete_schematic $selected"
  
  # scrolling hotkeys - only used for help_window because of focus
  # see the scroll_listbox command
  # bind $win.nodes <Next> "$win.nodes yview scroll 1 pages"
  # bind $win.nodes <Prior> "$win.nodes yview scroll -1 pages"
}

  if {[info exists saved_dir]} {
     set dir [use_first dir '$saved_dir]
  } else {
     set dir $INIT_SCHEM_DIR
  }
  set saved_dir $dir

  set base_dir_index [string first "/SueLib/" $dir]
  if {$base_dir_index < 0} {
     set lib_name $dir
  } else {
     set lib_name [string range $dir [expr ${base_dir_index} + 8] end]
  }

  $win.dir configure -text $lib_name
  # put in menu items
  $win.dir.other add command -label "Update listboxes" \
       -command "update_listboxes"
  $win.dir.other add separator

  # "." means not in the auto path
  set dirs(.) foo

  # Now put the schematic list into it
#  foreach schematic [array names SUE] {
#    if {[string range $schematic 0 4] != "ICON_"} {
#      global SUE_$schematic
#      if {[info exists SUE_${schematic}(modified)]} {
#	set prefix [string range "[set SUE_${schematic}(modified)]  " 0 1]
#      } else {
#	set prefix "  "
#      }
#
#      $win.nodes insert end $prefix$schematic
#    }
#  }

 # Now put the schematic list into it
  foreach schematic [lsort [info commands SCHEMATIC_*]] {
     set schem_dir [file dirname [lindex [use_first auto_index($schematic)] 1]]
     set base_dir_index [string first "/SueLib/" $schem_dir]
      if {$base_dir_index < 0} {
         set schem_dir_libname ""
      } else {
	  set schem_dir_libname [string range $schem_dir [expr ${base_dir_index} + 8] end]
      }

     if {$schem_dir == $dir} {
         global SUE_$schematic
         if {[info exists SUE_${schematic}(modified)]} {
	    set prefix [string range "[set SUE_${schematic}(modified)]  " 0 1]
         } else {
	    set prefix "  "
         }
         if {[string compare $schematic "SCHEMATIC_no_name"] != 0} {
	     $win.nodes insert end $prefix[string range $schematic 10 end]
        }
     } elseif {$schem_dir_libname == $dir} {
         global SUE_$schematic
         if {[info exists SUE_${schematic}(modified)]} {
	    set prefix [string range "[set SUE_${schematic}(modified)]  " 0 1]
         } else {
	    set prefix "  "
         }
         if {[string compare $schematic "SCHEMATIC_no_name"] != 0} {
	     $win.nodes insert end $prefix[string range $schematic 10 end]
        }
     }
  # add directories
      if {![info exists dirs($schem_dir)]} {
         $win.dir.other add command -label $schem_dir \
             -command "make_schematic_listbox $schem_dir $win"
         set dirs($schem_dir) 1
      }
  }
}

}

proc add_schematic_to_listbox {schematic {prefix "  "}} {
  global WIN

  # punt if icon
  if {[string range $schematic 0 4] == "ICON_"} {
    return
  }

  set win $WIN.lb.schematics

  if {[winfo exists $win] != 1} {
    make_schematic_listbox
    # making it will add the current schematic
    return
  }

  if {[winfo exists $win.nodes] == 1} {
      set list_schem_names [regsub -all SCHEMATIC_ \
				[info commands SCHEMATIC_*] ""]
#     set list_schem_names [$win.nodes get 0 end]
     foreach schem_name $list_schem_names {
         if {[string compare $schem_name $schematic] == 0} {
             return
	 }
      }
      if {[string compare $schematic "no_name"] != 0} {
         $win.nodes insert end $prefix$schematic
      }
     }
}

proc remove_schematic_from_listbox {schematic_name} {
  global WIN

  if {[string range $schematic_name 0 4] == "ICON_"} {
    set win $WIN.lb.icons
    set schematic [string range $schematic_name 5 end]
  } else {
    set win $WIN.lb.schematics
    set schematic $schematic_name
  }

  if {[winfo exists $win] != 1} {
    return
  }

  for {set i 0} {$i < [$win.nodes size]} {incr i} {
    if {[string range [$win.nodes get $i] 2 end] == $schematic} {
      $win.nodes delete $i
      return
    }
  }

  puts "couldn't find $schematic_name in listbox to delete."
}


# Changes the prefix (i.e. the first 2 characters in each line) of
# either the schematic or icon prefixes to be modified/unmodified.

proc change_listbox_prefix {schematic_name {prefix "  "}} {
  global WIN

  if {[is_icon $schematic_name]} {
    global ICON_WINDOWS
    if {[info exists ICON_WINDOWS]} {
      set wins $ICON_WINDOWS
    } else {
      set wins .icons
    }
    set schematic [get_rootname $schematic_name]
  } else {
    set wins .schematics
    set schematic $schematic_name
  }

  foreach win $wins {
    if {[string index $win 0] != "."} {
      set win .$win
    }
    set win $WIN.lb$win

    if {[winfo exists $win] != 1} {
      continue
    }

    for {set i 0} {$i < [$win.nodes size]} {incr i} {
      if {[string range [$win.nodes get $i] 2 end] == $schematic} {
	$win.nodes delete $i
	$win.nodes insert $i $prefix$schematic
	continue
      }
    }
  }
}


# Maps (uniconifies) and unmaps (iconifies) the other toplevel windows
# if one is Mapped/unmapped.  Tries to eliminate window icons for the
# "icons" and "schematics" windows since they are unnecessary.

proc map_others {event_win action} {

  global WIN_DATA tcl_platform

  if {[use_first WIN_DATA(mapping)] != ""} {
    return
  }
  set WIN_DATA(mapping) 1

  foreach win [winfo children .] {
    if {($win == ".icons" || $win == ".schematics") && \
	    $action == "wm iconify"} {
#      wm overrideredirect $win true
#      wm withdraw $win
       wm iconify $win
      continue
    } 
    if {$action == "wm deiconify"} {
#       wm overrideredirect $win false
       wm deiconify $win
    }

    if {$win != $event_win} {
      eval $action $win
    }
  }

  # Windows seems to have a problem with restoring focus to main window
  # after de-iconifying  - the lines below fix this issue
  if {$tcl_platform(platform) == "windows"} {
     if {$action == "wm deiconify"} {
        foreach win [winfo children .] {
	   if {$win == ".win1"} {
              focus $win
	   }
	}
     }
  }

  set WIN_DATA(mapping) ""
}


# abbreviates keystrings so they are nicer.

proc abbrev string {

  set control "Ctrl-"

  if {[string range $string 0 7] == "Control-"} {
    return "$control[string range $string 8 end]"
  } else {
    return $string
  }
}


# called from icon listbox

proc autoload_from_listbox {dir} {

  global auto_path

  # first remove from autopath so we will recompute tcltags
  set pos [lsearch $auto_path $dir]
  if {$pos != -1} {
    set auto_path [lreplace $auto_path $pos $pos]
    add_auto_path $dir
  }

  # force wish to load in all of the tclIndexes
  auto_load_index

  auto_load_directory $dir
  
  make_icon_listbox

}

# auto loads all files in the directories.  If the file to be loaded
# appears to be eclipsed by a file higher in the auto_path then the
# higher one will be loaded instead.

proc auto_load_directory {dir {type ""}} {

  global auto_index SUFFIX

  set dir [clean_dir $dir]
  if {$type == ""} {
    puts "Loading all cells from directory $dir"
  }

  foreach file [glob -nocomplain $dir/*$SUFFIX(default)] {
    set cell [file tail [file root $file]]

    if {[info exists auto_index(ICON_$cell)] && \
	    [file dirname [lindex $auto_index(ICON_$cell) 1]] != $dir} {
      # if shadowed, get other
      eval $auto_index(ICON_$cell)
      continue
    }
    if {[info exists auto_index(SCHEMATIC_$cell)] && \
	    [file dirname [lindex $auto_index(SCHEMATIC_$cell) 1]] != $dir} {
      # if shadowed, get other
      eval $auto_index(SCHEMATIC_$cell)
      continue
    }

    if {[file readable $file]} {
      if {[catch "source $file" msg]} {
	puts "ERROR: couldn't read file $file: $msg"
      }
    } else {
#	sue_error "Couldn't read file $file.  Check permissions."
	puts "ERROR: Couldn't read file $file.  Check permissions."
    }

#    sue_error flush
  }
}

# we're back!

proc ready {} {

  global WIN

  $WIN configure -cursor arrow
}


#
# tk_dialog:
#
# This procedure displays a dialog box, waits for a button in the dialog
# to be invoked, then returns the index of the selected button.
#
# Arguments:
# w -		Window to use for dialog top-level.
# title -	Title to display in dialog's decorative frame.
# text -	Message to display in dialog.
# bitmap -	Bitmap to display in dialog (empty string means none).
# default -	Index of button that is to display the default ring
#		(-1 means none).
# args -	One or more strings to display in buttons across the
#		bottom of the dialog box.

proc tk_dialog_new {w title text bitmap default args} {
    global tk_priv tcl_platform

    # 1. Create the top-level window and divide it into top
    # and bottom parts.

# puts entered_tk_dialog_new

    catch {destroy $w}
    toplevel $w -class Dialog
    wm title $w $title
    wm iconname $w Dialog
    frame $w.top -relief raised -bd 1
    pack $w.top -side top -fill both
    frame $w.bot -relief raised -bd 1
    pack $w.bot -side bottom -fill both

    # 2. Fill the top part with bitmap and message.

    message $w.msg -width 5i -text $text -font HELP_FONT
    pack $w.msg -in $w.top -side right -expand 1 -fill both -padx 5m -pady 5m
    if {$bitmap != ""} {
	label $w.bitmap -bitmap $bitmap
	pack $w.bitmap -in $w.top -side left -padx 5m -pady 5m
    }

    # 3. Create a row of buttons at the bottom of the dialog.

    set i 0
    foreach but $args {
	button $w.button$i -text $but -command "set tk_priv(button) $i"
	if {$i == $default} {
	    frame $w.default -relief sunken -bd 1
	    raise $w.button$i $w.default
	    pack $w.default -in $w.bot -side left -expand 1 -padx 3m -pady 2m
	    pack $w.button$i -in $w.default -padx 2m -pady 2m \
		    -ipadx 2m -ipady 1m
	    bind $w <Return> "$w.button$i flash; set tk_priv(button) $i"
	} else {
	    pack $w.button$i -in $w.bot -side left -expand 1 \
		    -padx 3m -pady 3m -ipadx 2m -ipady 1m
	}
	incr i
    }

    # 4. Withdraw the window, then update all the geometry information
    # so we know how big it wants to be, then center the window in the
    # display and de-iconify it.

    wm withdraw $w

    set win_width [winfo screenwidth $w]
    set win_height [winfo screenheight $w]
    set req_width [winfo reqwidth $w]
    set req_height [winfo reqheight $w]
    set parenx [winfo vrootx [winfo parent $w]]
    set pareny [winfo vrooty [winfo parent $w]]
    set x [expr ${win_width}/2 - ${req_width}/2 \
	       - ${parenx}]
    set y [expr ${win_height}/2 - ${req_height}/2 \
	       - ${pareny}]
    wm geom $w +$x+$y
    wm deiconify $w

    # 5. Set a grab and claim the focus too.

    set oldFocus [focus]
    grab $w

    if {$tcl_platform(platform) == "windows" || $tcl_platform(os) == "Darwin"} {
       wm attributes $w -topmost 1
    } else {
       # older versions of Tcl/Tk don't support -topmost option for wm attributes
       wm attributes $w
    }
    focus $w

    # 6. Wait for the user to respond, then restore the focus and
    # return the index of the selected button.

    vwait tk_priv(button)
    destroy $w
    if {[winfo exists $oldFocus]} {
       focus $oldFocus
    }
    return $tk_priv(button)
}



