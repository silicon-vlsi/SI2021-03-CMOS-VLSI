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
# sets state so that the user can repeatedly draw arcs
# by pressing left mouse button, dragging it somewhere, and then letting it
# go at the end point for the arc
##############################################################################
proc setup_arc_mode {} {

  global cur_c WIN WIN_DATA SNAP_XY

  enter_mode arc abort_arc_mode

  set WIN_DATA($WIN,display_msg) "Button-1 begins arc, Escape aborts"

  bind $cur_c <Button-1> "begin_arc_draw $SNAP_XY"
  bind $cur_c <Escape> "abort_arc_mode"
}

##############################################################################
# Makes an arc.  Called from outside world
##############################################################################
proc make_arc {x1 y1 x2 y2 args} {

  global cur_c COLORS

  set id [eval $cur_c create arc $x1 $y1 $x2 $y2 -tags draw_item \
	      -style arc -fill $COLORS(draw_item) -outline $COLORS(draw_item) $args]

  $cur_c addtag arc withtag $id
  return $id
}

##############################################################################
# begin_arc_draw creates the arc that user can drag around.
##############################################################################
proc begin_arc_draw {x y} {

  global cur_c WIN WIN_DATA SNAP_XY ARC

  set WIN_DATA($WIN,display_msg) \
      "Move cursor to produce arc, Button-1 to end, Escape aborts"

  set ARC(x) $x
  set ARC(y) $y

  # make the starting arc -- prepare for warnings
  set ARC(id) [make_arc $x $y $x $y -start 0 -extent 270]

  # setup bindings to drag the arc, and set the finish point
  bind $cur_c <Motion> "drag_arc $SNAP_XY"
  bind $cur_c <B1-ButtonRelease> "active_arc_mode"
  bind $cur_c <Escape> "abort_arc_mode"
}

proc active_arc_mode {} {

global cur_c

bind $cur_c <Button-1> "end_arc_mode"
}
##############################################################################
# drags around the new arc
##############################################################################
proc drag_arc {x y} {

  global cur_c ARC

  # reshape the arc. Needed since Tk expects that
  # x1, y1 for a rectangle or arc are always less than x2,y2
  if {$ARC(x) > $x} {
    if {$ARC(y) > $y} { # topleft quadrant
      catch "$cur_c coords $ARC(id) $x $y $ARC(x) $ARC(y)"
    } else { # bottomleft quadrant
      catch "$cur_c coords $ARC(id) $x $ARC(y) $ARC(x) $y"
    }
  } else {
    if {$ARC(y) > $y} { # topright quadrant
      catch "$cur_c coords $ARC(id) $ARC(x) $y $x $ARC(y)"
    } else { # bottomright quadrant
      catch "$cur_c coords $ARC(id) $ARC(x) $ARC(y) $x $y"
    }
  }
}

##############################################################################
# end_arc_mode finishes the drawing of the arc
##############################################################################
proc end_arc_mode {} {

  global cur_c ARC

  # is the arc bbox a point or a line? 
  set c [$cur_c coords $ARC(id)]
  if {[nearby_num [lindex $c 0] [lindex $c 2]] || \
	  [nearby_num [lindex $c 1] [lindex $c 3]]} {
    # throw away a point arc
    puts "Arc too small, aborted."
    abort_arc_mode
    return
  }

  select_id $ARC(id)
  create_arc_edit_markers $ARC(id)

  # flag that this canvas has been modified
  is_modified

  leave_mode arc
}

##############################################################################
# abort_arc_mode deletes the arc being drawn and gets back to a good state
##############################################################################
proc abort_arc_mode {} {

  global cur_c ARC

  if {[info exists ARC(id)]} {
    $cur_c delete $ARC(id)
  }

  leave_mode arc
}



# disgusting procedures to try to make arc editing nice

proc create_arc_edit_markers {id} {

  global cur_c WIN WIN_DATA

  set coords [$cur_c coords $id]
  set x1 [lindex $coords 0]
  set y1 [lindex $coords 1]
  set x2 [lindex $coords 2]
  set y2 [lindex $coords 3]

  # edit markers for the overall size
  set mark_id [create_edit_mark $x1 $y1]
  $cur_c addtag "proc resize $id $mark_id 0" withtag $mark_id
  set mark_id [create_edit_mark $x2 $y2]
  $cur_c addtag "proc resize $id $mark_id 2" withtag $mark_id

  # edit markers for the arc start and extent
  # note that start and extent are in degrees, sin, cos take radians.
  set deg_to_rad [expr 3.14159265 / 180]
  set extent [expr [lindex [$cur_c itemconfigure $id -extent] 4] * $deg_to_rad]
  set start [expr [lindex [$cur_c itemconfigure $id -start] 4] * $deg_to_rad]
  set center [center $id]
  set dx [expr abs($x2-$x1)]
  set dy [expr abs($y2-$y1)]
  if {$dx > $dy} {
    set radius [expr $dx/2 + 20]
  } else {
    set radius [expr $dy/2 + 20]
  }
  
  set mark_id [create_edit_mark \
		   [expr cos($start)*$radius + [lindex $center 0]] \
		   [expr -sin($start)*$radius + [lindex $center 1]]]
  $cur_c addtag "proc arc_start $id $mark_id" withtag $mark_id

  set mark_id [create_edit_mark \
		   [expr cos($start+$extent)*$radius + [lindex $center 0]] \
		   [expr -sin($start+$extent)*$radius + [lindex $center 1]]]
  $cur_c addtag "proc arc_extent $id $mark_id" withtag $mark_id

  set WIN_DATA($WIN,display_msg) \
      "Button-1 moves markers: 2 are for arc size, 1 for arc start, 1 for arc end"
}

proc arc_start {id mark_id} {

  global cur_c

  set deg_to_rad [expr 3.14159265 / 180]
  set start [lindex [$cur_c itemconfigure $id -start] 4]
  set extent [lindex [$cur_c itemconfigure $id -extent] 4]
  set mark_coords [center $mark_id]
  set center [center $id]
  set dx [expr [lindex $mark_coords 0] - [lindex $center 0]]
  set dy [expr [lindex $mark_coords 1] - [lindex $center 1]]
  set new_start [expr int(atan2($dx,$dy) / $deg_to_rad - 90)]
  set new_extent [expr int($extent - ($new_start - $start))]

  if {$new_extent > 345 && $new_extent < 380} {
    # make it a an oval
    set new_extent 359
    set new_start [expr int($start + $extent - $new_extent)]
  }

  $cur_c itemconfigure $id -start $new_start
  $cur_c itemconfigure $id -extent $new_extent
}

proc arc_extent {id mark_id} {

  global cur_c

  set deg_to_rad [expr 3.14159265 / 180]
  set start [lindex [$cur_c itemconfigure $id -start] 4]
  set mark_coords [center $mark_id]
  set center [center $id]
  set dx [expr [lindex $mark_coords 0] - [lindex $center 0]]
  set dy [expr [lindex $mark_coords 1] - [lindex $center 1]]
  set degs [expr atan2($dx,$dy) / $deg_to_rad + 270]
  set extent [expr int($degs - $start)]

  if {$extent <= 0} {
    incr extent 360
  }

  if {$extent > 345 && $extent < 380} {
    # make it a an oval
    set extent 359
  }

  $cur_c itemconfigure $id -extent $extent
}
