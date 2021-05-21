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

##############################################################################
# duplicates selected objects.
# A special mode will duplicate text only, including what is in icons.
##############################################################################
proc setup_duplicate_selected {{mode duplicate}} {

  global cur_c scale
  global WIN WIN_DATA SNAP_XY
  global SAVE DUPLICATE_MODE STARTED_COPY_OR_MOVE_FLAG

  set DUPLICATE_MODE $mode
  set STARTED_COPY_OR_MOVE_FLAG 0

  # check to see if anything is selected
  if {[$cur_c find withtag selected] == "" && $mode != "verilog"} {
    return
  }

  enter_mode duplication abort_duplication

  set WIN_DATA($WIN,display_msg) \
      "Button-1 creates duplicates, Mouse moves duplicates, Button-1 ends, Escape aborts"   

  # create new bindings for canvas so that the user can place the items
  bind $cur_c <Any-Button> "start_drag_duplication $SNAP_XY"
  bind $cur_c <Escape> "abort_duplication"

}

# duplicates all instances, wires, lines, arcs, and text

proc duplicate_selected {} {

  global cur_c scale COLORS PROC

  set PROC ""

  write_instances_for_duplication selected
  write_wires_for_duplication selected
  write_draw_items selected

  # remake each line in proc
  set ids ""
  foreach line $PROC {
    # add an element and save id
    lappend ids [eval $line]
  }

  unset PROC

  select_ids $ids
}


# duplicates all text that is selected, including turning text associated with
# an icon into regular text.  Changes all text to normal size with left anchor.

proc duplicate_text {} {

  global cur_c scale COLORS PROC

  set PROC ""

  foreach id [$cur_c find withtag selected] {
    if {[$cur_c type $id] == "text"} {
      set text [lindex [$cur_c itemconfigure $id -text] 4]
      if {$text == ""} {
	# toss turds
	continue
      }
      set origin [round_list_scale [$cur_c coords $id] $scale]

      lappend PROC "  make_text -origin [list $origin] -text [list $text]"
    }
  }

  # remake each line in proc
  set ids ""
  foreach line $PROC {
    # add an element and save id
    lappend ids [eval $line]
  }

  unset PROC

  select_ids $ids
}


proc start_drag_duplication {x y} {

  global cur_c SAVE ORIG_X ORIG_Y

  global WIN WIN_DATA SNAP_XY DUPLICATE_MODE STARTED_COPY_OR_MOVE_FLAG

  set STARTED_COPY_OR_MOVE_FLAG 1
  catch {unset SAVE}

  set SAVE(ids) [$cur_c find withtag selected]

  if {[info exists DUPLICATE_MODE]} {
      set mode $DUPLICATE_MODE
  } else {
      set mode ""
  }
  if {$mode == "text"} {
    duplicate_text
  } elseif {$mode == "verilog"} {
    create_verilog_property
  } else {
    duplicate_selected
  }
  ready

  # was anything duplicated, if not punt.
  if {[$cur_c find withtag selected] == ""} {
    abort_duplication
    return
  }

  # offset it a little to show it was duplicated
  # later we can do the macdraw hack here
#  $cur_c move selected [expr $scale * 2] [expr $scale * 2]

  # set this again since the selection resets it
  set WIN_DATA($WIN,display_msg) \
      "Button-1 creates duplicates, Shift for H/V movement, Button-1 ends, Escape aborts"   

  # create new bindings for canvas so that the user can place the items
  bind $cur_c <Any-Button> ""
  bind $cur_c <Any-Motion> "drag_duplication $SNAP_XY; set SCROLL(status) on; auto_scroll \[incrX SCROLL(mem)\] $SNAP_XY"
  bind $cur_c <Shift-Motion> "control_drag_duplication $SNAP_XY; set SCROLL(status) on; auto_scroll \[incrX SCROLL(mem)\] $SNAP_XY"
  bind $cur_c <Any-ButtonRelease> "active_duplication_mode; set SCROLL(status) off"
  bind $cur_c <Any-Escape> "abort_duplication; set SCROLL(status) off"

  set SAVE(x) $x
  set SAVE(y) $y
  set ORIG_X $x
  set ORIG_Y $y
}

proc active_duplication_mode {} {

  global cur_c
  bind $cur_c <Any-Button-1> "end_duplication; set SCROLL(status) off"
}

##############################################################################
# drag_duplication moves the duplicate items
##############################################################################
proc drag_duplication {x y} {

  global cur_c
  global SAVE

  # move the duplicate stuff
  $cur_c move selected [expr $x-$SAVE(x)] [expr $y-$SAVE(y)]

  set SAVE(x) $x
  set SAVE(y) $y
}


proc control_drag_duplication {x y} {

  global cur_c SAVE
  global ORIG_X ORIG_Y

  # only move in the bigger direction
  if {[expr abs($x - $ORIG_X)] > [expr abs($y - $ORIG_Y)]} {
     set x_new $x
     set y_new $ORIG_Y
  } else {
     set x_new $ORIG_X
     set y_new $y
  }

  # move the duplicate stuff
  $cur_c move selected [expr $x_new-$SAVE(x)] [expr $y_new-$SAVE(y)]

  set SAVE(x) $x_new
  set SAVE(y) $y_new
}

##############################################################################
# abort_duplication removes the new event bindings and returns the
# drawing state to general and destroys the duplication box
##############################################################################
proc abort_duplication {} {

  global cur_c SAVE SAVE STARTED_COPY_OR_MOVE_FLAG

  if {$STARTED_COPY_OR_MOVE_FLAG == 0} {

    # the user didn't even press Button-1 to start the copy yet
    leave_mode duplication
    return
  }

  # delete the duplicate things
  $cur_c delete selected

  # select the previously selected ids again
  select_ids $SAVE(ids)

  leave_mode duplication
}

##############################################################################
# ends duplication
##############################################################################
proc end_duplication {} {

  global cur_c cur_s

  # shows connection info of newly created stuff
  show_connects selected

  # flag that this canvas has been modified
  is_modified

  leave_mode duplication
  ready

  set ids [get_intersect_tag selected origin]

  foreach id $ids {
     upvar #0 ${cur_s}_inst$id i_data
     set type $i_data(type)
     upvar #0 icon_$type g_data
     foreach name $g_data(prop_names) {
	if {[string compare $name "name"] == 0} {
	    if {[string compare $i_data(_$name) ""] != 0} {
		if {[string compare $type "input"] != 0 && \
		    [string compare $type "output"] != 0 && \
		    [string compare $type "inout"] != 0 && \
		    [string compare $type "global"] != 0 && \
		    [string compare $type "name_net"] != 0 && \
		    [string compare $type "name_net_s"] != 0} {
                        set i_data(_$name) ""
                        remake $id $id
		}
	    }
	}
     }
  }

}


proc setup_paste_mode {} {

  global cur_c scale cur_s
  global WIN WIN_DATA SNAP_XY
  global SAVE

  if {[info commands paste] == ""} {
    puts "Nothing to paste."
    return
  }

  enter_mode duplication abort_duplication

  set WIN_DATA($WIN,display_msg) \
      "Button moves selected, release Button to end, Escape aborts"   

  # create new bindings for canvas so that the user can place the items
#  bind $cur_c <Any-Button> "start_drag_duplication $SNAP_XY"
  bind $cur_c <Escape> "abort_duplication"

  catch {unset SAVE}

  set SAVE(ids) [$cur_c find withtag selected]

  # deselect everything
  select_ids ""

  paste

  # move into center of screen

  # find center of paste stuff
  set bbox [$cur_c bbox selected]
  set x [expr int(([lindex $bbox 2] + [lindex $bbox 0])/(2 * $scale)) * $scale]
  set y [expr int(([lindex $bbox 3] + [lindex $bbox 1])/(2 * $scale)) * $scale]

  # get the center of the visible screen

  set canv_bbox [lindex [$cur_c configure -scrollregion] 4]

  set x_left_canv [lindex $canv_bbox 0]
  set y_left_canv [lindex $canv_bbox 1]
  set wid_canv_bbox [expr [lindex $canv_bbox 2] - [lindex $canv_bbox 0]]
  set height_canv_bbox [expr [lindex $canv_bbox 3] - [lindex $canv_bbox 1]]

  
  set xleft [expr [lindex [$cur_c xview] 0] * $wid_canv_bbox \
           + $x_left_canv]
  set yleft [expr [lindex [$cur_c yview] 0] * $height_canv_bbox \
           + $y_left_canv]
  set xright [expr [lindex [$cur_c xview] 1] * $wid_canv_bbox \
           + $x_left_canv]
  set yright [expr [lindex [$cur_c yview] 1] * $height_canv_bbox \
           + $y_left_canv]

  set xcenter [expr ($xleft + $xright)/2]
  set ycenter [expr ($yleft + $yright)/2]

  # now move it
  $cur_c move selected [expr $scale * int(($xcenter - $x) / $scale)] \
      [expr $scale * int(($ycenter - $y) / $scale)]

  # set this again since the selection resets it
#  set WIN_DATA($WIN,display_msg) \
#      "Button moves duplicates, release Button ends, Escape aborts"   

  ready
  end_duplication

}


# creates an icon property that is a guess at what the user wants the
# verilog property to be.  This is a helper function.

proc create_verilog_property {} {

  global cur_s cur_c

  if {![is_icon $cur_s]} {
    puts "Aborted.  Can only create verilog properties for icons."
    return
  }

  set name [get_rootname $cur_s]

  set text "-type fixed -name verilog -text \{$name \[unique_name \"\" \$name $name\]\n\("

  set ios [concat [$cur_c find withtag icon_input] \
	       [$cur_c find withtag icon_inout] \
	       [$cur_c find withtag icon_output]]
  set separator ""

  foreach id $ios {
    upvar #0 ${cur_s}_inst$id i_data
    if {[is_bus $i_data(_name)]} {
      set ident [lindex [split $i_data(_name) "\["] 0]
      set io_name "\{$i_data(_name)\}"
    } else {
      set ident $i_data(_name)
      set io_name $i_data(_name)
    }
    set text "$text$separator.$ident\(\$$io_name\)"
    set separator ","
  }

  set text "$text\)\\;\}"
       
  select_id [make_text -text $text]
}
