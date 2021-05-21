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
# delete_selected removes all netlist and drawing objects which are selected
# or just removes an optional id if given
##############################################################################
proc delete_selected {{ids ""}} {

  global cur_c cur_s
  
  if {$ids == ""} {
    set ids [$cur_c find withtag selected]
  }

  global DELETE_FLAG
  set DELETE_FLAG 1

  delete_selected_undo
  unset DELETE_FLAG

  # waste any edit markers, anchors, or other temporary items
  $cur_c delete tmp

  set changed 0

  foreach id $ids {
    if {[$cur_c type $id] == ""} {
      # already been deleted
      continue
    }

    # delete instances, remember term coords for updating show connect 
    if {[is_tagged $id origin]} {

      foreach term_id [get_intersect_tag inst$id term] {
	set show_coords([round_list [center $term_id]]) 1
      }

      # delete old icon and lose the old data structure
      $cur_c delete inst$id
      global ${cur_s}_inst$id
      unset ${cur_s}_inst$id

      set changed 1
      continue
    }

    # delete wires, remember end coords for connection info
    if {[is_tagged $id wire]} {
      set coords [$cur_c coords $id]
      set show_coords([round_list [lrange $coords 0 1]]) 1
      set show_coords([round_list [lrange $coords 2 3]]) 1
      $cur_c delete $id

      set changed 1
      continue
    }

    # delete edit_markers with along with drawing objects
    if {[is_tagged $id draw_item]} {
      # don't allow the origin of the icon to be deleted
      if {[is_tagged $id origin_icon]} {
	continue
      }

      $cur_c delete $id

      set changed 1
      continue
    }
    # people like to delete opens and watch the wire dissapear, so...
    if {[is_tagged $id open]} {
      set coords [$cur_c coords $id]
      set center [center $id]
      $cur_c delete $id
      set wire_ids [eval $cur_c find overlapping [lrange $coords 0 1] \
			[lrange $coords 4 5]]
      foreach wire_id $wire_ids {
	if {[is_tagged $wire_id wire]} {
	  set wire_coords [$cur_c coords $wire_id]
	  if {[eval nearby $center [lrange $wire_coords 0 1]]} {
	    set show_coords([round_list [lrange $wire_coords 2 3]]) 1
	    $cur_c delete $wire_id
	    set changed 1
	    break
	  }
	  if {[eval nearby $center [lrange $wire_coords 2 3]]} {
	    set show_coords([round_list [lrange $wire_coords 0 1]]) 1
	    $cur_c delete $wire_id
	    set changed 1
	    break
	  }
	}
      }
      continue
    }
  }

  # update the nodes on the connection points
  if {[info exists show_coords]} {
    foreach xy [array names show_coords] {
      eval show_connect_point $xy clean
    }
  }

  if {$changed} {
    # flag that this canvas has been modified
    is_modified
  }

  display_selection
  ready
}

proc delete_selected_undo {{mode undo}} {

  global PROC scale 

  set PROC ""
  lappend NEW_PROC "global cur_c cur_s scale COLORS"

  write_instances selected
  write_wires selected
  write_draw_items selected

  if {$PROC == ""} {
    # nothing is there
    return
  }

  lappend NEW_PROC "set save_scale \$scale"
  lappend NEW_PROC "scale_canvas $scale"
  foreach line $PROC {
    lappend NEW_PROC "lappend ids \[$line\]"
  }
  lappend NEW_PROC "select_ids \$ids"
  lappend NEW_PROC "scale_canvas \$save_scale"

  if {$mode == "undo"} {
    lappend NEW_PROC "show_connects selected"

    # define the procedure that undoes the delete
    proc undo {} [join $NEW_PROC "\n"]

    unset PROC

    # now save away this proc
    save_undo

  } else {
    # define the paste procedure
    proc paste {} [join $NEW_PROC "\n"]

    puts "Copied selected to clipboard."
  }
}


#############################################################################
# routines for dropping icons in schematics
#############################################################################

proc setup_drop_icon {icon {win ""}} {


  global cur_c WIN WIN_DATA SNAP_XY saved_icon KEYS ICON_MENU

  if {$win != "ignore_win_for_selection" && $ICON_MENU == ""} {
     set active_ind [$win curselection]
     if {$active_ind != ""} {
       set active_val [$win get $active_ind]
       set ind [string last " " $active_val]
       set new_val [string range $active_val [expr ${ind} + 1] end]
       set icon $new_val
     } else {
       return
     }
  }

  if {$icon == ""} {
      if {[info exists saved_icon]} {
          set icon $saved_icon
      }
  }

  $cur_c addtag other_opens withtag open

  set saved_icon $icon

  enter_mode icon abort_icon_mode

  set WIN_DATA($WIN,display_msg) \
      "Button-1 drops $icon, hold Shift to repeat drops, Escape aborts."

  set id [make $icon]
  if {$id == ""} {
    leave_mode icon
    return
  }
  select_id $id

  # select_id resets this
  set WIN_DATA($WIN,display_msg) \
      "Button-1 drops $icon, hold Shift to repeat drops, Escape aborts."

  bind $cur_c <B1-Motion> "move_icon $SNAP_XY"
  bind $cur_c <Motion> "move_icon $SNAP_XY"
  bind $cur_c <Button-1> "end_icon_mode"
  bind $cur_c <Shift-Button-1> "end_multiple_icon_mode"
  bind $cur_c <Escape> "abort_icon_mode"
  bind $cur_c <Double-Button-1> "end_icon_mode"
  bind $cur_c <$KEYS(flip_horizontal)> "transform_selected MX"
  bind $cur_c <$KEYS(flip_vertical)> "transform_selected MY"
  bind $cur_c <$KEYS(rotate)> "transform_selected ROTATE"
}

proc move_icon {x y} {

  global cur_c

  set id [lindex [$cur_c find withtag selected] 0]
  set origin_id [find_origin $id]

  set coords [lrange [$cur_c coords $origin_id] 0 1]

  $cur_c move selected [expr $x - [lindex $coords 0]] \
      [expr $y - [lindex $coords 1]]

  $cur_c delete open&&!other_opens

}

proc end_icon_mode {} {

  global cur_s cur_c

  # show connection info on new icon unless we are in an icon
  if {![is_icon $cur_s]} {
    show_term_connects selected
  }

  $cur_c dtag other_opens

  # flag that this canvas has been modified
  is_modified

  leave_mode icon
}

proc end_multiple_icon_mode {} {

  global cur_s

  # show connection info on new icon unless we are in an icon
  if {![is_icon $cur_s]} {
    show_term_connects selected
  }

  # flag that this canvas has been modified
  is_modified

  leave_mode icon
  setup_drop_icon "" "ignore_win_for_selection"
}

proc abort_icon_mode {} {

  global cur_c

  # it would be nice if this reselected previously selected -- later
  $cur_c delete selected

  leave_mode icon
}
