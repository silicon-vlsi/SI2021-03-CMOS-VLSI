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

# Pops up a window to edit properies of an instance and then deletes
# the old instance and calls a new one.

# bind it to double-click button-1 in make_schematic.tcl
# bind $cur_c <Double-Button-1> {prop_edit_menu %x %y}

# Input x and y should NOT go through canvasx,canvasy commands

proc prop_edit_menu {x y} {

  global cur_c cur_s scale DISABLE_CANVAS_EVENT SUE CPPSIMSHARED_HOME CPPSIM_HOME \
         tcl_platform

  if {$tcl_platform(platform) == "windows"} {
     set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin/win32"
  } else {
     set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin"
  }

  # stop general canvas binding events
  set DISABLE_CANVAS_EVENT 1

  set current_id [$cur_c find withtag current]
  set id [find_origin $current_id]

  if {[is_tagged $id origin] != 1} {
    # not an icon
    return
  }

  # instance-specific data (like W=1.2u) are here

  upvar #0 ${cur_s}_inst$id i_data 

  set type $i_data(type)
  # instance-generic data (like default L=0.6u) are here
  upvar #0 icon_$type g_data

  set prop_list ""
  foreach name $g_data(prop_names) {
    lappend prop_list [list $name $i_data(_$name)]
  }

  if {$prop_list == ""} {
    return
  }

  set wrooty [winfo rooty $cur_c]
  set wrootx [winfo rootx $cur_c]
  set winy [expr ${wrooty} + ${y}]
  set winx [expr ${wrootx} + ${x}]
  set title $type

 
  # create the menu
  set new_prop_list [prop_menu $winx $winy $title $prop_list]
  if {$new_prop_list == ""} {
    # empty list means the user hit cancel
    return
  }

  if {$new_prop_list == $prop_list} {
    # nothing changed so punt
    return
  }

  # store the new values in the data structure
  foreach pair $new_prop_list {
    set name [lindex $pair 0]
    set value [lindex $pair 1]

    set i_data(_$name) $value
  }

  # now remake the icon (this will flag modified)
  set new_id [remake $id $id]

  # flag that this canvas has been modified
  is_modified

  ########   Disable this for now!! ########
  # for IOs, update the corresponding icon or schematic if there is one
  if {[lsearch "input output inout" $type] != -1 && 0} {
    # save the current canvas, schematic, and scale
    set save_cur_c $cur_c
    set save_cur_s $cur_s
    set save_scale $scale

    # look to see if the corresponding one is in a canvas yet
    set cell [corresponding_cell $cur_s]

    if {![info exists SUE($cell)]} {
      # put it in a canvas
      change_views 
      if {$save_cur_s == $cur_s} {
	# doesn't exist, punt
	return
      }
      zoom_to_fit

      # change back
      change_views
    }

    set new 0

    # if the old name is a duplicate of another IO then add a new one
    # to corresponding cell
    foreach id [$cur_c find withtag icon_$type] {
      upvar #0 ${cur_s}_inst$id i_data
      if {$id != $new_id && \
	      $i_data(_name) == [lindex [lindex $prop_list 0] 1]} {
	set new 1
	break
      }
    }

    # set up current canvas, schematic, and scale to corresponding cell
    set cur_s $cell
    upvar #0 SUE_$cur_s data
    set cur_c $data(canvas)
    set scale $data(scale)

    if {$new == 0} {
      # try to find the terminal with the same name
      foreach id [$cur_c find withtag icon_$type] {
	upvar #0 ${cur_s}_inst$id i_data
	if {$i_data(_name) == [lindex [lindex $prop_list 0] 1]} {
	  # got it
	  set i_data(_name) [lindex [lindex $new_prop_list 0] 1]
	  remake $id $id
	  set new -1
	}
      }
    }

    if {$new != -1} {
      # make a new IO in the corresponding cell
      set id [make $type -name [lindex [lindex $new_prop_list 0] 1]]
      is_modified
      remember_modified
    }

    # restore the current canvas, schematic, and scale
    set cur_c $save_cur_c
    set cur_s $save_cur_s
    set scale $save_scale
  }
}


# Pops up a window to edit properies.  Returns a list of the new properties

proc prop_menu {x y title prop_list} {

  global command CPPSIMSHARED_HOME CPPSIM_CELL tcl_platform

  # Just in case there is an old one around
  catch {destroy .prop_menu}

  toplevel .prop_menu 
  wm geometry .prop_menu "+$x+$y"
  wm title .prop_menu $title
    
#  bind .prop_menu <Escape> {catch {destroy .prop_menu} ; return}

  frame .prop_menu.data
  frame .prop_menu.args
  frame .prop_menu.values

  global tab_list
  set tab_list ""

  global _PROP_LIST
  set _PROP_LIST $prop_list
  # build a line for each property
  foreach assoc $prop_list {
    set prop [lindex $assoc 0]
    if {$tcl_platform(os) == "Darwin"} {
       label .prop_menu.arg_$prop -text $prop -pady 4 
    } else {
       label .prop_menu.arg_$prop -text $prop
    }
    pack .prop_menu.arg_$prop -side top -in .prop_menu.args -anchor w

    # the textvariable for an entry must be global
    global _PROP_$prop
    # need another evaluation in here
    set _PROP_$prop [get_assoc $prop $prop_list]
    set entry .prop_menu.value_$prop
    entry $entry -width 20 -textvariable _PROP_$prop -relief sunken -bd 2
    pack $entry -side top -in .prop_menu.values

    bind $entry <Return> {set command [end_prop_menu]}
    bind $entry <Tab> "tab_through_entries \$tab_list"
    lappend tab_list $entry

    bind $entry <Escape> {set command "" ;catch {destroy .prop_menu}}
    bind $entry <Destroy> {set command "" ;catch {destroy .prop_menu}}

    # now add some EMACS-like bindings
    bind $entry <Control-n> "tab_through_entries \$tab_list"
    bind $entry <Control-p> "tab_through_entries \$tab_list backword"
    bind $entry <Control-b> \
      "$entry icursor \[expr \[$entry index insert\] - 1\]"
    bind $entry <Control-f> \
      "$entry icursor \[expr \[$entry index insert\] + 1\]"
    bind $entry <Control-a> "$entry icursor 0"
    bind $entry <Control-e> "$entry icursor end"
    bind $entry <Control-d> "$entry delete insert"
    bind $entry <Control-k> "$entry delete insert end"
  }

  if {$tcl_platform(os) == "Darwin"} {
     pack .prop_menu.args -side left -fill y -expand y -in .prop_menu.data
     pack .prop_menu.values -side right -fill y -expand y -in .prop_menu.data
  } else {
     pack .prop_menu.args -side left -in .prop_menu.data
     pack .prop_menu.values -side right -in .prop_menu.data
  }


  frame .prop_menu.buttons

  frame .prop_menu.default -relief sunken -bd 1
  button .prop_menu.done -text "Done" -command \
      {set command [end_prop_menu]}
  pack .prop_menu.done -in .prop_menu.default -padx 1m -pady 1m -ipadx 2m
  pack .prop_menu.default -side left -in .prop_menu.buttons \
      -padx 4m -ipadx 1m -pady 1m -expand 1

  button .prop_menu.cancel -text "Cancel" \
      -command {set command "" ; catch {destroy .prop_menu}}
  pack .prop_menu.cancel -side left -in .prop_menu.buttons \
      -padx 4m -ipadx 2m -pady 1m -expand 1

  label .prop_menu.note -text [cppCreateModuleLabel $title] -justify left 
  pack .prop_menu.note -side top
  pack .prop_menu.buttons -side bottom
  cppCreateCodeButtons $CPPSIM_CELL .prop_menu

  pack .prop_menu.data -side top
    
  set command 0
  [lindex $tab_list 0] icursor end
  focus [lindex $tab_list 0]

  # Border pixels for X windows.  Can't seem to figure these out.
  set XBORDER 3
  set YBORDER 25

  # If the prop_menu floats off the screen, move it back on.
  set win_width [winfo screenwidth .prop_menu]
  set prop_width [winfo width .prop_menu]
  set win_height [winfo screenheight .prop_menu]
  set prop_height [winfo height .prop_menu]
  set dx [min [expr ${win_width}-${win_height}- \
		   ${x}-${XBORDER}] 0]
  set dy [min [expr ${win_height}-${prop_height}- \
		   ${y}-${YBORDER}] 0]
  # MHP 9/28/2016:  commented out lines below to avoid problems with multi-monitors
  # if {$dx < 0 || $dy < 0} {
  #     wm geometry .prop_menu "+[expr ${x}+${dx}]+[expr ${y}+${dy}]"    
  # }
   
  catch {grab set .prop_menu}

  vwait command
  return $command
}

proc end_prop_menu {} {

  global _PROP_LIST

  set prop_list $_PROP_LIST

  catch {destroy .prop_menu} 

  set new_prop_list ""
  foreach assoc $prop_list {
    set prop [lindex $assoc 0]

    global _PROP_$prop
    # MHP:  modified 9/28/2014 to remove white space from property text
    # lappend new_prop_list [list $prop [set _PROP_$prop]]
    lappend new_prop_list [list $prop [string map {" " ""} [set _PROP_$prop]]]
  }

  # Note that we are leaving around some global variables of the 
  # form _PROP_x and tab_list.

  return $new_prop_list
}


# Changes the focus to the next (or previous) entry in the list

proc tab_through_entries {list {dir forward}} {

  set i [lsearch $list [focus]]

  if {$i < 0} {
    # no focus yet
    focus [lindex $list 0]
    return
  }
  
  set insert [[lindex $list $i] index insert]

  if {$dir == "forward"} {
    incr i
    if {$i >= [llength $list]} {
      set i 0
    }
  } else {
    incr i -1
    if {$i < 0} {
    set length_list [llength $list]
	set i [expr ${length_list} - 1]
    }
  }

  [lindex $list $i] icursor $insert
  focus [lindex $list $i]
}


proc generator_edit_menu {x y} {

  global cur_c cur_s DISABLE_CANVAS_EVENT

  # stop general canvas binding events
  set DISABLE_CANVAS_EVENT 1

  set current_id [$cur_c find withtag current]
  set id [find_origin $current_id]

  if {[is_tagged $id origin] != 1} {
    # not an icon
    return
  }

  # instance-specific data (like W=1.2u) are here
  upvar #0 ${cur_s}_inst$id i_data

  set type $i_data(type)
  # instance-generic data (like default L=0.6u) are here
  upvar #0 icon_$type g_data

  # if this isn't a generator, just edit properties
  if {[info exists g_data(generator)] != 1} {
    prop_edit_menu $x $y
    return
  }

  # put together the prop list for the prop menu with name at the front.
  set prop_list [list "name $type"]
  foreach pair $g_data(gdefaults) {
    if {[set pos [lsearch $g_data(gargs) -[lindex $pair 0]]] != -1} {
      # not defaulted
      lappend prop_list \
	  [list [lindex $pair 0] [lindex $g_data(gargs) [incr pos]]]
    } else {
      # defaulted
      lappend prop_list $pair
    }
  }

  if {$prop_list == ""} {
    # this should ever happen
    return
  }

  set wry [winfo rooty $cur_c]
  set wrx [winfo rootx $cur_c]
  set winy [expr ${wry} + ${y}]
  set winx [expr ${wrx} + ${x}]
  set title $g_data(generator)
  set message "Edit Generator:" 

  # create the menu
  set new_prop_list [prop_menu $winx $winy $message $title $prop_list]
  if {$new_prop_list == ""} {
    # empty list means the user hit cancel
    return
  }

  if {$prop_list == $new_prop_list} {
    # nothing changed, punt
    return
  }

  set name [lindex [lindex $new_prop_list 0] 1]

  set gargs ""
  foreach prop [lrange $new_prop_list 1 end] {
    # only add to gargs if not defaulted
    if {[lindex $prop 1] != [get_assoc [lindex $prop 0] $g_data(gdefaults)]} {
      lappend gargs -[lindex $prop 0] [lindex $prop 1]
    }
  }

  # generate the new generator
  if {[eval regenerate $g_data(generator) $name $gargs] == 0} {
    # generator errored out
    return
  }

  # now make the new icon to replace
  set bogus_id [make $name]

  # now replace the current icon (this will flag modified)
  set new_id [remake $id $bogus_id]

  # delete bogus icon and data structures
  $cur_c delete inst$bogus_id
  upvar #0 ${cur_s}_inst$bogus_id i_data
  unset i_data

  # select it
  select_id $new_id add
  unhighlite_selected
}

