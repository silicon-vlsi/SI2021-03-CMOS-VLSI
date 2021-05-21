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
# setup_line_mode sets state so that the user can draw one line
# by pressing left mouse button, dragging it somewhere, and then letting it
# go at the end point for the line
##############################################################################
proc setup_line_mode {} {

  global cur_c 
  global WIN WIN_DATA SNAP_XY SAVE

  enter_mode line abort_line_mode

  # Line mode global variable initialization
  catch {unset SAVE}

  set WIN_DATA($WIN,display_msg) \
      "Button-1 start line segments, Double-Button-1 (or Button-3) ends, Shift for H or V only move, Delete removes last segment, Escape aborts"

  bind $cur_c <Button-1> "begin_line_segment $SNAP_XY"
  bind $cur_c <Double-Button-1> {end_line_mode}
  bind $cur_c <Button-3> {end_line_mode}
  bind $cur_c <Escape> {abort_line_mode}
}

##############################################################################
# begin_line_segment create the line that user can drag around.
##############################################################################
proc begin_line_segment {x y} {

  global cur_c COLORS
  global SAVE SNAP_XY
  global LINE_x LINE_y LINE_LAST_x LINE_LAST_y

  set LINE_x $x
  set LINE_y $y
  set LINE_LAST_x $x
  set LINE_LAST_y $y

  # make the line
  set SAVE(id) \
      [$cur_c create line $x $y $x $y -tags "draw_item" -fill $COLORS(draw_item) ]

  # setup bindings to drag the line, and add extra line segments
  bind $cur_c <Motion> "drag_line_segment $SNAP_XY"
  bind $cur_c <Shift-Motion> "controlled_drag_line_segment $SNAP_XY"
  bind $cur_c <Button-1> "add_line_segment $SNAP_XY"
  bind $cur_c <Any-KeyPress-Delete> "remove_line_segment $SNAP_XY"
  bind $cur_c <Any-KeyPress-BackSpace> "remove_line_segment $SNAP_XY"
  # L10 is the "cut" key
  bind $cur_c <L10> "remove_line_segment $SNAP_XY"
}

##############################################################################
# removes last_line segment drawn, except if there's only one line segment
##############################################################################
proc remove_line_segment {x y} {

  global cur_c
  global SAVE

  # augment the existing line, by adding a new coordinate
  set coords [$cur_c coords $SAVE(id)]
  set length [llength $coords]

  # if more than one segment, remove last one
  if {$length != 4} {
    set coords [lreplace $coords [expr $length - 4] [expr $length - 1] $x $y]
    eval $cur_c coords $SAVE(id) $coords 
  }
}

##############################################################################
# add_line_segment create the line that user can drag around.
##############################################################################
proc add_line_segment {x y} {

  global cur_c
  global SAVE LINE_x LINE_y LINE_LAST_x LINE_LAST_y

  if {$x == $LINE_x && $y == $LINE_y} {
      end_line_mode
      return
  }

  set LINE_LAST_x $x
  set LINE_LAST_y $y

  # augment the existing line, by adding a new coordinate
  set coords [$cur_c coords $SAVE(id)]
  lappend coords $x $y
  eval $cur_c coords $SAVE(id) $coords

}

##############################################################################
# drag_line_segment drags around the new line that's being created
##############################################################################
proc drag_line_segment {x y} {

  global cur_c
  global SAVE

  # resize new line's last coordinate
  set coords [$cur_c coords $SAVE(id)]
  set length [llength $coords]
  set coords [lreplace $coords [expr $length - 2] [expr $length - 1] $x $y]

  # remake the line with the new coords
  eval $cur_c coords $SAVE(id) $coords
}

proc controlled_drag_line_segment {x y} {

  global cur_c
  global SAVE LINE_LAST_x LINE_LAST_y

  if {[expr abs($x - $LINE_LAST_x)] > [expr abs($y - $LINE_LAST_y)]} {
      set x_new $x
      set y_new $LINE_LAST_y
    } else {
      set x_new $LINE_LAST_x
      set y_new $y
    }

  # resize new line's last coordinate
  set coords [$cur_c coords $SAVE(id)]
  set length [llength $coords]
  set coords [lreplace $coords [expr $length - 2] [expr $length - 1] \
          $x_new $y_new]

  # remake the line with the new coords
  eval $cur_c coords $SAVE(id) $coords
}

##############################################################################
# end_line_mode finishes the drawing of the line and returns the bindings
# state to the previous input state
##############################################################################
proc end_line_mode {} {

  global cur_c
  global SAVE WIN WIN_DATA

  set coords [$cur_c coords $SAVE(id)]
  set length [llength $coords]

  if {$coords == ""} {
    # get rid of the point line
    abort_line_mode
    return
  }

  if {[lindex $coords [expr $length - 4]] == \
	  [lindex $coords [expr $length - 2]] && \
	  [lindex $coords [expr $length - 3]] == \
	  [lindex $coords [expr $length - 1]]} {

    if {$length == 4} {
      # get rid of the point line
      abort_line_mode
      return
    }

    # toss out last two point of line
    set coords [lreplace $coords [expr $length - 2] [expr $length - 1]]
    eval $cur_c coords $SAVE(id) $coords
  }

  select_id $SAVE(id)
  create_line_edit_markers $SAVE(id)

  # flag that this canvas has been modified
  is_modified

  leave_mode line

  set WIN_DATA($WIN,display_msg) \
      "Button-1 moves markers, Alt-Double-Button-1 adds mark near current, Control-Double-Button-1 deletes current mark"
}

##############################################################################
# abort_line_mode deletes the line being drawn and returns to a good state
##############################################################################
proc abort_line_mode {} {

  global cur_c
  global SAVE

  # get rid of the line being drawn
  if {[info exists SAVE(id)]} {
    $cur_c delete $SAVE(id)
  }
  puts "Aborting line mode"

  leave_mode line
}


# Makes a line.  Called from outside world

proc make_line {args} {

  global cur_c

  set id [eval $cur_c create line $args -tags draw_item]

  return $id
}


proc create_line_edit_markers {id} {

  global cur_c WIN WIN_DATA

  set coords [$cur_c coords $id]

  for {set i 0} {$i < [llength $coords]} {incr i 2} {
    set mark_id [create_edit_mark [lindex $coords $i] \
		     [lindex $coords [expr $i + 1]]]
    $cur_c addtag "proc resize $id $mark_id $i" withtag $mark_id

    $cur_c bind $mark_id <Alt-Double-Button-1> "add_vertex $id $mark_id $i"
    $cur_c bind $mark_id <Control-Double-Button-1> "delete_vertex $id $mark_id $i"
  }

  set WIN_DATA($WIN,display_msg) \
      "Button-1 moves markers, Alt-Double-Button-1 adds mark near current, Control-Double-Button-1 deletes current mark"
}


proc resize {id mark_id index} {

  global cur_c

  set coords [$cur_c coords $id]
  set mark_coords [center $mark_id]
  set new_coords [lreplace $coords $index [expr $index + 1] \
		      [lindex $mark_coords 0] [lindex $mark_coords 1]]
  eval $cur_c coords $id $new_coords
}


proc delete_vertex {id mark_id index} {

  global cur_c

  set coords [$cur_c coords $id]

  # can't delete a vertex if there is only one line segment
  if {[llength $coords] <= 4} {
    return
  }

  # delete all marks since their indices are now wrong.
  $cur_c delete edit_marker

  set new_coords [lreplace $coords $index [expr $index + 1]]

  eval $cur_c coords $id $new_coords

  create_line_edit_markers $id
}


proc add_vertex {id mark_id index} {

  global cur_c scale

  set coords [$cur_c coords $id]
  set mark_coords [center $mark_id]

  set new_coords [linsert $coords $index \
		      [expr $scale + [lindex $mark_coords 0]] \
		      [expr $scale + [lindex $mark_coords 1]]]

  # delete all marks since their indices are now wrong.
  $cur_c delete edit_marker

  eval $cur_c coords $id $new_coords

  create_line_edit_markers $id
}
