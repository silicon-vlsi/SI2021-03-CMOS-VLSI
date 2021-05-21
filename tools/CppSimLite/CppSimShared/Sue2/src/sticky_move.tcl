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

# Procedures for moving wires and nodes in sue.  Allows existing wires
# and nodes to be dragged and maintain connectivity


proc setup_drag_move_mode {x y} {

  global cur_c cur_s WIN WIN_DATA SAVE SNAP_XY STRETCH_WIRES

  catch {unset SAVE}

  set ids [$cur_c find withtag selected]

  if {$ids == ""} {
    # nothing selected
    return
  }

  if {[llength $ids] == 1 && [is_tagged $ids wire]} {
    # single wires only move perpendicular to their direction
    set SAVE(movedir) [wire_direction $ids]

  } else {
    # othewise allow movement in both dirs.
    set SAVE(movedir) B
  }

  enter_mode move abort_move_mode

  set WIN_DATA($WIN,display_msg) \
      "Drag mouse cursor move.  Shift for H or V only move.  Escape aborts"

  set SAVE(x) $x
  set SAVE(y) $y

  set SAVE(undo,x) $x
  set SAVE(undo,y) $y

  bind $cur_c <Any-B1-Motion> \
      "move_drag $SNAP_XY; set SCROLL(status) on; auto_scroll \[incrX SCROLL(mem)\] $SNAP_XY"
  bind $cur_c <Any-B1-Shift-Motion> \
      "control_move_drag $SNAP_XY; set SCROLL(status) on; auto_scroll \[incrX SCROLL(mem)\] $SNAP_XY"

  bind $cur_c <Any-B1-ButtonRelease> \
      "end_move_mode; set SCROLL(status) off"
  bind $cur_c <Any-Escape> "abort_move_mode; set SCROLL(status) off"

  if {[is_icon $cur_s]} {
    # don't do wire move in icons
    proc SUE_MOVE_WIRES {x y} {}
    set STRETCH_WIRES ""

  } else {
    # setup wire stretch/move
    setup_sticky_move [find_net_edges]

    # remove connection info as it is likely to change
    remove_connects selected
  }

  ready
}


##############################################################################
# move mode
##############################################################################
proc setup_move_mode args {

  global cur_c WIN WIN_DATA SAVE SNAP_XY STARTED_COPY_OR_MOVE_FLAG

  catch {unset SAVE}

  set STARTED_COPY_OR_MOVE_FLAG 0
  set ids [$cur_c find withtag selected]

  if {$ids == ""} {
    # nothing selected
    return
  }

  if {[llength $ids] == 1 && [is_tagged $ids wire]} {
    # single wires only move perpendicular to their direction
    set SAVE(movedir) [wire_direction $ids]

  } else {
    # othewise allow movement in both dirs.
    set SAVE(movedir) B
  }

  enter_mode move abort_move_mode

  if {[llength $args] > 1} {
    eval begin_move_mode $args 2
    return
  }

  set WIN_DATA($WIN,display_msg) \
      "Button-1 to begin move.  Shift for H or V only move.  Escape aborts"

  bind $cur_c <Any-Button-1> "begin_move_mode $SNAP_XY"
  bind $cur_c <Any-Escape> "abort_move_mode"
}

proc begin_move_mode {x y {button 1}} {

  global cur_c cur_s WIN WIN_DATA SAVE SNAP_XY STRETCH_WIRES STARTED_COPY_OR_MOVE_FLAG

  set STARTED_COPY_OR_MOVE_FLAG 1
  set WIN_DATA($WIN,display_msg) \
      "Move objects using cursor. Shift key constrains move to H or V only. Click Button-1 to end. Escape aborts"

  set SAVE(x) $x
  set SAVE(y) $y

  set SAVE(undo,x) $x
  set SAVE(undo,y) $y

#  bind $cur_c <Any-B${button}-Motion> \
#      "move_drag $SNAP_XY; set SCROLL(status) on; auto_scroll \[incrX SCROLL(mem)\] $SNAP_XY"
#  bind $cur_c <Control-B${button}-Motion> \
#      "control_move_drag $SNAP_XY; set SCROLL(status) on; auto_scroll \[incrX SCROLL(mem)\] $SNAP_XY"

  bind $cur_c <Any-Motion> \
      "move_drag $SNAP_XY; set SCROLL(status) on; auto_scroll \[incrX SCROLL(mem)\] $SNAP_XY"
  bind $cur_c <Shift-Motion> \
      "control_move_drag $SNAP_XY; set SCROLL(status) on; auto_scroll \[incrX SCROLL(mem)\] $SNAP_XY"

  bind $cur_c <Any-B${button}-ButtonRelease> \
      "active_move_mode; set SCROLL(status) off"
  bind $cur_c <Any-Escape> "abort_move_mode; set SCROLL(status) off"

  if {[is_icon $cur_s]} {
    # don't do wire move in icons
    proc SUE_MOVE_WIRES {x y} {}
    set STRETCH_WIRES ""

  } else {
    # setup wire stretch/move
    setup_sticky_move [find_net_edges]

    # remove connection info as it is likely to change
    remove_connects selected
  }

  ready
}


# drag objects and attached wires

proc move_drag {x y} {

  global cur_c SAVE

  if {$SAVE(movedir) == "V"} {
    set y $SAVE(y)

  } elseif {$SAVE(movedir) == "H"} {
    set x $SAVE(x)
  }

  $cur_c move selected [expr ${x} - ${SAVE(x)}] [expr ${y} - ${SAVE(y)}]

  SUE_MOVE_WIRES [expr ${x} - ${SAVE(x)}] [expr ${y} - ${SAVE(y)}]

  set SAVE(x) $x
  set SAVE(y) $y
}


# constrains movements to either horizontal or vertical, which is more.

proc control_move_drag {x y} {

  global cur_c SAVE

  if {$SAVE(movedir) == "V"} {
    set y $SAVE(y)
  } elseif {$SAVE(movedir) == "H"} {
    set x $SAVE(x)
  } else {
    # only move in the bigger direction
      if {[expr abs(${x} - ${SAVE(undo,x)})] > [expr abs(${y} - ${SAVE(undo,y)})]} {
      set y $SAVE(undo,y)
    } else {
      set x $SAVE(undo,x)
    }
  }

  $cur_c move selected [expr ${x} - ${SAVE(x)}] [expr ${y} - ${SAVE(y)}]

  SUE_MOVE_WIRES [expr ${x} - ${SAVE(x)}] [expr ${y} - ${SAVE(y)}]

  set SAVE(x) $x
  set SAVE(y) $y
}

proc active_move_mode {} {

global cur_c

  bind $cur_c <Button-1> \
      "end_move_mode; set SCROLL(status) off"

}

# fix up connectivity since we're done moving

proc end_move_mode {} {

  global cur_c SAVE

  # cleans up dragged wires
  end_sticky_move

  # show connection info
  show_connects selected clean

  if {[nearby $SAVE(x) $SAVE(y) $SAVE(undo,x) $SAVE(undo,y)] != 1} {
    # setup undo of move
    
    lappend NEW_PROC "global cur_c cur_s scale COLORS"

    set ids [$cur_c find withtag selected]

    lappend NEW_PROC "select_ids [list $ids]"
    lappend NEW_PROC "setup_sticky_move \[find_net_edges\]"
    lappend NEW_PROC "remove_connects selected"

    lappend NEW_PROC "$cur_c move selected [expr ${SAVE(undo,x)} - ${SAVE(x)}] [expr ${SAVE(undo,y)} - ${SAVE(y)}]"
    lappend NEW_PROC "SUE_MOVE_WIRES [expr ${SAVE(undo,x)} - ${SAVE(x)}] [expr ${SAVE(undo,y)} - ${SAVE(y)}]"
    lappend NEW_PROC "end_sticky_move"
    lappend NEW_PROC "show_connects selected clean"

    # define the procedure that undoes the delete
    proc undo {} [join $NEW_PROC "\n"]

    # now save away this proc
    save_undo

    # flag that this canvas has been modified
    is_modified
  }

  leave_mode move
  ready
}

proc abort_move_mode {} {

  global cur_c SAVE STARTED_COPY_OR_MOVE_FLAG

  if {[info exists SAVE] != 1 || $STARTED_COPY_OR_MOVE_FLAG == 0} {

    # the user didn't even press Button-1 to start the move yet
    leave_mode move
    return
  }

  # move everything back to where it was
  $cur_c move selected [expr ${SAVE(undo,x)} - ${SAVE(x)}] \
      [expr ${SAVE(undo,y)} - ${SAVE(y)}]
  
  SUE_MOVE_WIRES [expr ${SAVE(undo,x)} - ${SAVE(x)}] \
      [expr ${SAVE(undo,y)} - ${SAVE(y)}]

  # cleans up dragged wires
  end_sticky_move

  # puts connection info back
  show_connects selected 

  leave_mode move
}

##############################################################################
# Finds the wires/terminals that are not selected but are attached to selected
# wires/terms.  Returns a list of coords which specify this boundary.
##############################################################################

proc find_net_edges {} {

  global cur_c

  # mark everything in the bbox of selected with "edge"
  set bbox [$cur_c bbox selected]
  if {$bbox == ""} {
    return
  }
  eval $cur_c addtag edge overlapping $bbox

  # now remove all the things that are selected
  $cur_c dtag selected edge

  foreach id [$cur_c find withtag edge] {
    if {[is_tagged $id wire]} {
      set coords [$cur_c coords $id]
      test_net_edge [lrange $coords 0 1] net_edge
      test_net_edge [lrange $coords 2 3] net_edge
      continue
    }

    if {[is_tagged $id term]} {
      test_net_edge [center $id] net_edge
    }
  }
  $cur_c dtag edge

  if {[info exists net_edge]} {
    set xylist ""
    foreach coord [array names net_edge] {
      lappend xylist $coord
    }
    return $xylist
  }
}

##############################################################################
# tests a net edge to see if it contains any attached wires or terminals
# that are selected and returns the list of coords, otherwise returns "".
##############################################################################
proc test_net_edge {coord array} {

  global cur_c scale
  upvar 1 $array net_edge

  set d [expr ${scale}/3.0]
  set x [lindex $coord 0]
  set y [lindex $coord 1]

  set ids [$cur_c find overlapping [expr ${x} - ${d}] [expr ${y} - ${d}] \
	       [expr ${x} + ${d}] [expr ${y} + ${d}]]

  foreach id $ids {
    if {[is_tagged $id selected] != 1} {
      continue
    }

    # need opens and dots here so user can grab and move them
    if {[is_tagged $id wire] || [is_tagged $id term] || \
	    [is_tagged $id dot] || [is_tagged $id open]} {
      set net_edge([round_list $coord]) 1
    }
  }
  return ""
}


proc setup_sticky_move {coord_list} {

  global cur_c MOVE_WIRES STRETCH_WIRES

  catch {unset MOVE_WIRES}
  set STRETCH_WIRES ""

  foreach coord $coord_list {
    eval mark_attached_wires $coord
  }

  if {[info exists MOVE_WIRES] != 1} {
    proc SUE_MOVE_WIRES {x y} {}
    return
  }

  set PROC ""

  # want to group id's in order.  lsort groups but order is weird because
  # the id 99 is after 100.
  foreach tag [lsort [array names MOVE_WIRES]] {
    lappend PROC $MOVE_WIRES($tag)
  }

  proc SUE_MOVE_WIRES {x y} [join $PROC "\n"]
}


proc end_sticky_move {} {

  global cur_c STRETCH_WIRES

  # remove overlapping wires and generally clean up
# xxxxxx should we use remove_extra_wires or show_connect_wire???
  foreach id $STRETCH_WIRES {
#    show_connect_wire $id clean
    remove_extra_wires $id
  }
}


# mark all attached wires.  Add a new wire to an attached term

proc mark_attached_wires {x y} {

  global cur_c scale MOVE_WIRES STRETCH_WIRES

  set del [expr ${scale}/3.0]
  set ids [$cur_c find overlapping [expr ${x} - ${del}] [expr ${y} - ${del}] \
	       [expr ${x} + ${del}] [expr ${y} + ${del}]]

  foreach id $ids {
    if {[is_tagged $id dot] && [is_tagged $id selected] != 1} {
      set wire_id [make_wire $x $y $x $y]
      set MOVE_WIRES($wire_id,y) "$cur_c move $wire_id 0 \$y"
      set MOVE_WIRES($wire_id,end) "stretch_wire $wire_id end \$x 0"
      set wire_id [make_wire $x $y $x $y]
      set MOVE_WIRES($wire_id,end) "stretch_wire $wire_id end 0 \$y"

      lappend STRETCH_WIRES $wire_id
      return
    }
  }

  foreach id $ids {
    # Note that this will allow opens/dots, if selected, to survive.
    if {[is_tagged $id selected]} { 
      continue
    }

    if {[is_tagged $id open] || [is_tagged $id dot]} {
      $cur_c delete $id
      continue
    }

    if {[is_tagged $id wire]} {
      set coords [$cur_c coords $id]
      set p1 [lrange $coords 0 1]
      set p2 [lrange $coords 2 3]

      # lose point wires (shouldn't be necessary)
      if {[eval nearby $p1 $p2] == 1} {
	$cur_c delete $id
	continue
      }

      if {[eval nearby $x $y $p1] == 1} {
	set wire_dir [wire_direction $id]

	if {$wire_dir == "H"} {
	  set MOVE_WIRES($id,y) "$cur_c move $id 0 \$y" 
	  set MOVE_WIRES($id,begin) "stretch_wire $id begin \$x 0"

	} elseif {$wire_dir == "V"} {
	  set MOVE_WIRES($id,x) "$cur_c move $id \$x 0" 
	  set MOVE_WIRES($id,begin) "stretch_wire $id begin 0 \$y"

	} elseif {$wire_dir == "B"} {
	  set MOVE_WIRES($id,begin) "stretch_wire $id begin \$x \$y"
	  continue
	}

	eval remove_connect_point $p2
	eval mark_attached $id $wire_dir $p2

	continue
      }
      if {[eval nearby $x $y $p2] == 1} {
	set wire_dir [wire_direction $id]

	if {$wire_dir == "H"} {
	  set MOVE_WIRES($id,y) "$cur_c move $id 0 \$y" 
	  set MOVE_WIRES($id,end) "stretch_wire $id end \$x 0"

	} elseif {$wire_dir == "V"} {
	  set MOVE_WIRES($id,x) "$cur_c move $id \$x 0" 
	  set MOVE_WIRES($id,end) "stretch_wire $id end 0 \$y"

	} elseif {$wire_dir == "B"} {
	  set MOVE_WIRES($id,begin) "stretch_wire $id end \$x \$y"
	  continue
	}

	eval remove_connect_point $p1
	eval mark_attached $id $wire_dir $p1

	continue
      }
      continue
    }

    if {[is_tagged $id term]} {
      set wire_id [make_wire $x $y $x $y]
      set MOVE_WIRES($wire_id,y) "$cur_c move $wire_id 0 \$y"
      set MOVE_WIRES($wire_id,end) "stretch_wire $wire_id end \$x 0"
      set wire_id [make_wire $x $y $x $y]
      set MOVE_WIRES($wire_id,end) "stretch_wire $wire_id end 0 \$y"

      lappend STRETCH_WIRES $wire_id
    }
  }
}


# marks the objects attached to a wire at point x,y not including id.

proc mark_attached {wire_id wire_dir x y} {

  global cur_c scale MOVE_WIRES STRETCH_WIRES

  set del [expr ${scale}/3.0]
  set ids [$cur_c find overlapping [expr ${x} - ${del}] [expr ${y} - ${del}] \
	       [expr ${x} + ${del}] [expr ${y} + ${del}]]

  # if there is more than one wire attached or a term, add a new wire
  set attach_list ""

  foreach id $ids {
    if {$id == $wire_id} {
      continue
    }

    if {[is_tagged $id selected]} {
      continue
    }

    if {[is_tagged $id wire]} {
      set coords [$cur_c coords $id]
      set p1 [lrange $coords 0 1]
      set p2 [lrange $coords 2 3]

      # lose point wires (shouldn't be necessary)
      if {[eval nearby $p1 $p2] == 1} {
	continue
      }

      if {[eval nearby $x $y $p1] == 1} {
	lappend attach_list "$id begin"
	continue
      }
      if {[eval nearby $x $y $p2] == 1} {
	lappend attach_list "$id end"
	continue
      }
      continue
    }

    if {[is_tagged $id term]} {
      # this will force us to add a wire since there are at least two attached
      lappend attach_list "foo bar"
      lappend attach_list "foo bar"
    }
  }

  if {[llength $attach_list] == 0} {
    # nothing is attached to this wire so put the wire on the clean up list
    lappend STRETCH_WIRES $wire_id
    return
  }

  if {[llength $attach_list] > 1} {
    # make a new wire
    set id [make_wire $x $y $x $y]
    set end end

    if {$wire_dir == "V"} {
      set MOVE_WIRES($id,$end) "stretch_wire $id $end \$x 0"
    } else {
      set MOVE_WIRES($id,$end) "stretch_wire $id $end 0 \$y"
    }

  } else {
    set id [lindex [lindex $attach_list 0] 0]
    set end [lindex [lindex $attach_list 0] 1]

    set dir [wire_direction $id]
    if {$wire_dir == $dir} {
      if {$wire_dir == "V"} {
	set MOVE_WIRES($id,$end) "stretch_wire $id $end 0 \$y"
      } else {
	set MOVE_WIRES($id,$end) "stretch_wire $id $end \$x 0"
      }
    } else {
      if {$wire_dir == "V"} {
	set MOVE_WIRES($id,$end) "stretch_wire $id $end \$x 0"
      } else {
	set MOVE_WIRES($id,$end) "stretch_wire $id $end 0 \$y"
      }
    }
  }

  lappend STRETCH_WIRES $id
}


# removes extra wires due to overlapping wires around the given wire id

proc remove_extra_wires {wire_id} {

  global cur_c scale

  set dir [wire_direction $wire_id]

  if {$dir == "B"} {
    # boston wires don't need to be cleaned up
    return
  }

  set wire_coords [$cur_c coords $wire_id]
  set bbox [$cur_c bbox $wire_id]

  # delete this wire, we will remake it later
  $cur_c delete $wire_id

  if {$wire_coords == ""} {
    return
  }

  set del [expr ${scale}/3.0]

  # save coords for all wires parallel to the given wire
  if {$dir == "V"} {
    set coord [lindex $wire_coords 0]
    set coords [list [lindex $wire_coords 1] [lindex $wire_coords 3]]

  } else {
    set coord [lindex $wire_coords 1]
    set coords [list [lindex $wire_coords 0] [lindex $wire_coords 2]]
  }

  set select ""

  foreach id [eval $cur_c find overlapping $bbox] {
    if {[is_tagged $id wire]} {
      set c [$cur_c coords $id]

      if {[wire_direction $id] == $dir} {
	# wire is parallel, remember coords and delete
	if {$dir == "V"} {
	  lappend coords [lindex $c 1]
	  lappend coords [lindex $c 3]

	} else {
	  lappend coords [lindex $c 0]
	  lappend coords [lindex $c 2]
	}

	if {[is_tagged $id selected]} {
	  set select selected
	}
	$cur_c delete $id
	continue
      }

      # otherwise, look for wire ends that T

      set cind0 [lindex $c 0]
      set cind1 [lindex $c 1]
      set cind2 [lindex $c 2]
      set cind3 [lindex $c 3]
      set exprv0 [expr abs(${cind0} - ${coord})]
      set exprv1 [expr abs(${cind1} - ${coord})]
      set exprv2 [expr abs(${cind2} - ${coord})]
      set exprv3 [expr abs(${cind3} - ${coord})]

      if {$dir == "V"} {
	  if {${exprv0} < ${del}} {
	  lappend coords [lindex $c 1]
	  continue
	} 
	  if {${exprv2} < ${del}} {
	  lappend coords [lindex $c 3]
	}
      } else {
	  if {${exprv1} < ${del}} {
	  lappend coords [lindex $c 0]
	  continue
	} 
	  if {${exprv3} < ${del}} {
	  lappend coords [lindex $c 2]
	}
      }
      continue
    }

    if {[is_tagged $id term]} {
      if {$dir == "V"} {
	lappend coords [lindex [center $id] 1]
      } else {
	lappend coords [lindex [center $id] 0]
      }
    }
  }
  remake_wires $coord $coords $dir $select
}


# takes a list of points that are colinear and wires them correctly.
# Tosses points that are the same.

proc remake_wires {coord coords dir {selected ""}} {

  global cur_c scale COLORS

  set coords [lsort -real $coords]
  set ids ""
  set del [expr ${scale}/3.0]

  if {$dir == "V"} {
    set old_y [lindex $coords 0]

    foreach y $coords {
        set expr_ymyold [expr ${y}-${old_y}]
	if {${expr_ymyold} >= ${del}} { 
	set id [make_wire_selected $coord $old_y $coord $y $selected]
	show_connect_point $coord $old_y clean
      }
      set old_y $y
    }
    if {$old_y != ""} {
      show_connect_point $coord $old_y clean
    }
    return $ids
  }

  if {$dir == "H"} {
    set old_x [lindex $coords 0]

    foreach x $coords {
      set expr_xmxold [expr $x-$old_x]
	if {${expr_xmxold} >= ${del}} { 
	set id [make_wire_selected $old_x $coord $x $coord $selected]
	show_connect_point $old_x $coord clean
      }
      set old_x $x
    }
    if {$old_x != ""} {
      show_connect_point $old_x $coord clean
    }
    return $ids
  }
}


# Utility procedures for wires and connectivity
# Returns:  H - horizontal wire, V - vertical wire, T - terminal, B - Boston

proc wire_direction {id} {

  global cur_c scale

  # if the wire no longer exists, just punt
  if {[$cur_c type $id] == ""} {
    return
  }

  if {[is_tagged $id term]} {
    return T
  }

  set coords [$cur_c coords $id]
  set x1 [lindex $coords 0]
  set y1 [lindex $coords 1]
  set x2 [lindex $coords 2]
  set y2 [lindex $coords 3]

  set del [expr ${scale}/3.0]

  set x1mx2 [expr abs($x1-$x2)]
  if {${x1mx2} < ${del}} {
    return V
  }
  set y1my2 [expr abs($y1-$y2)]
  if {${y1my2} < ${del}} {
    return H
  }
  return B
}

