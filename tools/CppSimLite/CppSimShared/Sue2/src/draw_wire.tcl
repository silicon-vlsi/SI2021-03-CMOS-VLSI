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

# Procedures for drawing wires in sue.

# TODO:
#
# Eventually, fix canvas c code so that you can use the find closest to
# highlight where the closest point is to drop the wire if desired.


# What some of the global variables are for:
# SAVE(x) SAVE(y) first x,y coords of current line
# SAVE(id) the canvas id of current wire segment
# SAVE(ids) the canvas ids of all previous wires
# SAVE(HV) whether the current wire segment is vertical/horizontal, or if
#          it is at any angle
# SAVE(dir) records last wire segment being vertical or horizontal


proc setup_draw_wire {} {

  global cur_c cur_s WIN WIN_DATA SNAP_XY SAVE KEYS 

  if {[is_icon $cur_s]} {
    # must be an icon
    puts "Aborting wire mode.  Can't draw wires in icons.  Use lines."
    return
  }

  enter_mode wire abort_wire_mode

  set WIN_DATA($WIN,display_msg) \
      "Button-1 adds segment, Double-Button-1 (or Button-3) ends, Delete/Backspace removes last segment, Escape cancels"

  # Wire mode global variable initialization
  catch {unset SAVE}
  
  set SAVE(HV) 1
  set SAVE(id) ""
  set SAVE(ids) ""
  set SAVE(dir) ""

  set SAVE(selection) [$cur_c find withtag selected]
  # deselect everything
  select_ids ""

  # pressing the shift and button-1 causes the current wire being drawn to have 
  # non manhattan geometry

  bind $cur_c <Button-1> "set SAVE(HV) 1 ; begin_draw_wire $SNAP_XY"
  bind $cur_c <Shift-Button-1> "set SAVE(HV) 0 ; begin_draw_wire $SNAP_XY"
  bind $cur_c <Double-Button-1> "really_end_wire_mode $SNAP_XY"
  bind $cur_c <Button-3> "really_end_wire_mode $SNAP_XY"
  bind $cur_c <Any-KeyPress-Delete> "remove_wire_segment $SNAP_XY"
  bind $cur_c <Any-KeyPress-BackSpace> "remove_wire_segment $SNAP_XY"
  # L10 is the "cut" key
  bind $cur_c <L10> "remove_wire_segment $SNAP_XY"
#  bind $cur_c <Any-Escape> "abort_wire_mode"
  bind $cur_c <$KEYS(abort_wire)> "abort_wire_mode"
  bind $cur_c <Motion> "look_for_terminals_and_wires $SNAP_XY"

  set WIN_DATA($WIN,display_msg) \
      "Button-1 adds segment, Double-Button-1 (or Button-3) ends, Delete/Backspace removes last segment, Escape cancels"

}

proc end_wire_mode {x y} {

  global cur_c COLORS
  global SAVE

  # finish the current wire -- unless the user never started any wires
  if {$SAVE(id) != ""} {
    finish_wire_segment $x $y

    # clean up all wires
    foreach id $SAVE(ids) {
      show_connect_wire $id clean
    }

    # flag that this canvas has been modified
    is_modified
  }

  $cur_c itemconfigure term -fill $COLORS(icon_pinbox)
  $cur_c itemconfigure wire -fill $COLORS(wire)

  leave_mode wire

  setup_draw_wire
}

proc really_end_wire_mode {x y} {

  global cur_c COLORS
  global SAVE

  # finish the current wire -- unless the user never started any wires
  if {$SAVE(id) != ""} {
    finish_wire_segment $x $y

    # clean up all wires
    foreach id $SAVE(ids) {
      show_connect_wire $id clean
    }

    # flag that this canvas has been modified
    is_modified
  }

  $cur_c itemconfigure term -fill $COLORS(icon_pinbox)
  $cur_c itemconfigure wire -fill $COLORS(wire)

  leave_mode wire
}


proc abort_wire_mode {} {

  global cur_c SAVE

  # delete current line segments
  $cur_c delete tmp

  # delete previous line segments
  if {[info exists SAVE(ids)]} {
    foreach id $SAVE(ids) {
      $cur_c delete $id
    }
  }

  # reselect what was selected before
  select_ids $SAVE(selection)

  puts "Aborting wire mode"

  leave_mode wire
}

proc begin_draw_wire {x y} {

  global cur_c SAVE SNAP_XY WIRE_END_FLAG wire_snap_x wire_snap_y

  # save the coords and just make a point wire.  This will get rubber
  # banded into a real wire

  set xval $x
  set yval $y

  if {[info exists wire_snap_x]} {
      if {$wire_snap_x != 100000} {
          set xval $wire_snap_x
          set yval $wire_snap_y
      }
  }
  set SAVE(x) $xval
  set SAVE(y) $yval
  set SAVE(id) [create_line $xval $yval $xval $yval]

  bind $cur_c <B1-Motion> "set SAVE(HV) 1 ; rubber_band_wire $SNAP_XY"
  bind $cur_c <Motion> "set SAVE(HV) 1 ; rubber_band_wire $SNAP_XY"
  bind $cur_c <Shift-B1-Motion> "set SAVE(HV) 0 ; rubber_band_wire $SNAP_XY"
  bind $cur_c <Shift-Motion> "set SAVE(HV) 0 ; rubber_band_wire $SNAP_XY"

  # Additional button-1 presses give us a new segment
  bind $cur_c <Button-1> "set SAVE(HV) 1 ; add_wire_segment $SNAP_XY"
  bind $cur_c <Shift-Button-1> "set SAVE(HV) 0 ; add_wire_segment $SNAP_XY"

  set WIRE_END_FLAG 0
}

proc add_wire_segment {x y} {

  global SAVE WIRE_END_FLAG wire_snap_x wire_snap_y

  if {$WIRE_END_FLAG == 1} {
      set xval $x
      set yval $y
      if {[info exists wire_snap_x]} {
         if {$wire_snap_x != 100000} {
            set xval $wire_snap_x
            set yval $wire_snap_y
         }
      }
      end_wire_mode $xval $yval
  } else {
      # first finish the last wire segment before starting the next one
      finish_wire_segment $x $y

      # SAVE(nx) and SAVE(ny) are the actual position of the wire
      set SAVE(x) $SAVE(nx)
      set SAVE(y) $SAVE(ny)

      # start a new segment
      set SAVE(id) [create_line $SAVE(x) $SAVE(y) $SAVE(x) $SAVE(y)]
  }
}


proc finish_wire_segment {x y} {

  global cur_c SAVE

  # delete current line, gets recreated below with good coords
  $cur_c delete $SAVE(id)
  set SAVE(id) ""

  # if the wire segment has no length, punt
  if {[nearby $SAVE(x) $SAVE(y) $SAVE(nx) $SAVE(ny)]} {
    return
  }

  # make a real wire (was a line before)
  set id [make_wire_selected $SAVE(x) $SAVE(y) $SAVE(nx) $SAVE(ny) selected]

  # show_connect_wire on the previous segment if there is one
  #if {[llength $SAVE(ids)] != 0} {
  #show_connect_wire [lindex $SAVE(ids) 0]
  #}

  set SAVE(ids) [concat $id $SAVE(ids)]

  # if we are manhattan, switch the direction for the new segment 
  if {$SAVE(HV) == 1} {
    if {$SAVE(dir) == ""} {
      set SAVE(dir) [wire_direction $id]
    }
    if {$SAVE(dir) == "V"} {
      set SAVE(dir) H
    } elseif {$SAVE(dir) == "H"} {
      set SAVE(dir) V
    }
  }
}

# remove a segment of the wire during drawing

proc remove_wire_segment {x y} {

  global cur_c
  global SAVE

  if {[llength $SAVE(ids)] == 0} {

    # let use start wire at a new point
    if {$SAVE(id) != ""} {
      $cur_c delete $SAVE(id)
    }

    # start over again
    leave_mode wire

    setup_draw_wire

    return
  }

  # delete current line
  $cur_c delete $SAVE(id)

  # get information and then delete previous line
  set SAVE(id) [lindex $SAVE(ids) 0]
  set coords [$cur_c coords $SAVE(id)]
  $cur_c delete $SAVE(id)

  set SAVE(x) [lindex $coords 0]
  set SAVE(y) [lindex $coords 1]

  set SAVE(id) [eval create_line $SAVE(x) $SAVE(y) $x $y]

  # if we are manhattan, switch the direction for the new segment 
  if {$SAVE(HV) == 1} {
    if {$SAVE(dir) == "V"} {
      set SAVE(dir) H
    } else {
      set SAVE(dir) V
    }
  }
  
  # remove previous line from list of ids
  set SAVE(ids) [lrange $SAVE(ids) 1 end]

  # remove_connect_wire on the previous segment if there is one
  remove_connect_point $SAVE(x) $SAVE(y)
}


# makes a wire.  The outside world calls this procedure

proc make_wire {x1 y1 x2 y2} {

  global cur_c COLORS

  $cur_c create line $x1 $y1 $x2 $y2 -tags wire -fill $COLORS(wire)
}


# could be merged into make_wire but isn't since we wat make_wire to
# be as fast as possible.

proc make_wire_selected {x1 y1 x2 y2 {selected ""}} {

  global cur_c COLORS

  if {$selected == ""} {
    $cur_c create line $x1 $y1 $x2 $y2 -tags wire -fill $COLORS(wire)
  } else {
    $cur_c create line $x1 $y1 $x2 $y2 -tags "wire selected" \
	-fill $COLORS(selected)
  }
}


proc rubber_band_wire {x y} {

  global cur_c COLORS SNAP_XY WIRE_END_FLAG
  global SAVE wire_snap_x wire_snap_y scale

  if {[info exists SAVE(id)] != 1} {
    return
  }

  # remove the old wire segment
  $cur_c delete $SAVE(id)

  # stretch previous wire if there is one and it's manhattan
  if {$SAVE(ids) != ""} {

    set id [lindex $SAVE(ids) 0]

    if {$SAVE(dir) == "V"} {
      if {$SAVE(x) == ""} {
	set SAVE(x) $x
      }
      stretch_wire $id end [expr $x - $SAVE(x)] 0
      set SAVE(x) $x
    } elseif {$SAVE(dir) == "H"} {
      if {$SAVE(y) == ""} {
	set SAVE(y) $y
      }
      stretch_wire $id end 0 [expr $y - $SAVE(y)]
      set SAVE(y) $y
    }
  } 


  # check to see if there are touching wires or terminals

  if {$SAVE(HV) == 1} {
      # determine whether to be a horizontal or a vertical line
      set x1 $SAVE(x)
      set y1 $SAVE(y)
      set x2 $x
      set y2 $y
      if {[expr abs($x1-$x2)] >= [expr abs($y1-$y2)]} {
           set xc $x2
           set yc $y1 
      } else {
             set xc $x1
             set yc $y2
      }
  } else {
      set xc $x
      set yc $y
  }

  set del [expr $scale * 10.0/7.0]
  set ids [$cur_c find overlapping [expr $x-$del] [expr $y-$del] [expr $x+$del] [expr $y+$del]]

  $cur_c itemconfigure term -fill $COLORS(icon_pinbox)
  $cur_c itemconfigure wire -fill $COLORS(wire)

  set WIRE_END_FLAG 0
  set wire_snap_x 100000
  set wire_snap_y 100000

  set wire_id [lindex $SAVE(ids) 0]

  set found_wire_flag 0

  if {$ids != ""} {
      foreach id $ids {
	 if {$id == $wire_id} {
             continue
	 }

         set tag [$cur_c gettags $id]
	  if {[lsearch $tag term] != -1 && $found_wire_flag == 0}  {

              $cur_c itemconfigure $id -fill $COLORS(active)

	      set coordinates [$cur_c coords $id]
              set x1 [lindex $coordinates 0]
              set y1 [lindex $coordinates 1]
              set x2 [lindex $coordinates 2]
              set y2 [lindex $coordinates 3]

              set wire_snap_x [expr ($x1 + $x2)/2]
              set wire_snap_y [expr ($y1 + $y2)/2]              

	  } elseif {[lsearch $tag wire] != -1} {

              set found_wire_flag 1

              $cur_c itemconfigure $id -fill $COLORS(active)

	      set coordinates [$cur_c coords $id]
              set x1 [lindex $coordinates 0]
              set y1 [lindex $coordinates 1]
              set x2 [lindex $coordinates 2]
              set y2 [lindex $coordinates 3]
              if {$y1 > $y2} {
                 set temp $y1
                 set y1 $y2
                 set y2 $temp
	      }
              if {$x1 > $x2} {
                 set temp $x1
                 set x1 $x2
                 set x2 $temp
	      }


              if {$x1 == $x2} {
#             vertical wire
                  set wire_snap_x $x1
                  if {$wire_snap_x == $SAVE(x) || $x == $SAVE(x)} {
		      if {$y > $SAVE(y)} {
                          set wire_snap_y $y1
		      } else {
                          set wire_snap_y $y2
		      }
                      break
		  }

		  if {$y >= $y1 && $y <= $y2} { 
                     set wire_snap_y $SAVE(y)
		  } else {
  		      if {$y > $y2} {
                          set wire_snap_y $y2
		      } else {
                          set wire_snap_y $y1
		      }
		  }
	      } else {
#             horizontal wire
                  set wire_snap_y $y1
                  if {$wire_snap_y == $SAVE(y) || $y == $SAVE(y)} {
		      if {$x > $SAVE(x)} {
                          set wire_snap_x $x1
		      } else {
                          set wire_snap_x $x2
		      }
                      break
		  }

		  if {$x >= $x1 && $x <= $x2} {
                      set wire_snap_x $SAVE(x)
		  } else {
		      if {$x > $x2} {
                         set wire_snap_x $x2
		      } else {
                         set wire_snap_x $x1
		      }
		  }
	      }
	  }
      }
  }


  # remake the wire segment with the new coords
  if {$wire_snap_x == 100000} {
      set SAVE(id) [create_line $SAVE(x) $SAVE(y) $x $y]
  } else {
      set WIRE_END_FLAG 1
      if {($SAVE(x) == $wire_snap_x) || ($SAVE(y) == $wire_snap_y)} {
         set SAVE(id) [create_line $SAVE(x) $SAVE(y) $wire_snap_x $wire_snap_y]
      } else {
	  set id_prev [lindex $SAVE(ids) 0]
	  set coordinates [$cur_c coords $id_prev]
	  if {$SAVE(x) == $x} {
	     if {[lindex $coordinates 0] == [lindex $coordinates 2]} {
                set SAVE(id) [create_line $SAVE(x) $SAVE(y) $x $y]
                set WIRE_END_FLAG 0
	     } else {
	        set coordinates [lreplace $coordinates 2 2 $wire_snap_x]
                $cur_c coords $id_prev $coordinates
                set SAVE(x) $wire_snap_x
                set SAVE(id) [create_line $SAVE(x) $SAVE(y) $wire_snap_x $wire_snap_y]
	     }
	  } else {
	      if {[lindex $coordinates 1] == [lindex $coordinates 3]} {
                set SAVE(id) [create_line $SAVE(x) $SAVE(y) $x $y]
                set WIRE_END_FLAG 0
	      } else {
	        set coordinates [lreplace $coordinates 3 3 $wire_snap_y]
                $cur_c coords $id_prev $coordinates
                set SAVE(y) $wire_snap_y
                set SAVE(id) [create_line $SAVE(x) $SAVE(y) $wire_snap_x $wire_snap_y]
	      }
	  }

      }
  }

  # Do it now
  #update idletasks
}

proc look_for_terminals_and_wires {x y} {

  global cur_c COLORS wire_snap_x wire_snap_y scale

  # check to see if there are touching wires or terminals

  set del [expr $scale * 10.0/7.0]
  set ids [$cur_c find overlapping [expr $x-$del] [expr $y-$del] [expr $x+$del] [expr $y+$del]]

  $cur_c itemconfigure term -fill $COLORS(icon_pinbox)
  $cur_c itemconfigure wire -fill $COLORS(wire)

  set wire_snap_x 100000
  set wire_snap_y 100000

  set WIRE_END_FLAG 0
  set found_wire_flag 0
  if {$ids != ""} {
      foreach id $ids {
         set tag [$cur_c gettags $id]
	  if {[lsearch $tag term] != -1 && $found_wire_flag == 0}  {
	      set coordinates [$cur_c coords $id]
              set x1 [lindex $coordinates 0]
              set y1 [lindex $coordinates 1]
              set x2 [lindex $coordinates 2]
              set y2 [lindex $coordinates 3]

              set wire_snap_x [expr ($x1 + $x2)/2]
              set wire_snap_y [expr ($y1 + $y2)/2]              

              $cur_c itemconfigure $id -fill $COLORS(active)
	  } elseif {[lsearch $tag wire] != -1} {
              set found_wire_flag 1
	      set coordinates [$cur_c coords $id]
              set x1 [lindex $coordinates 0]
              set y1 [lindex $coordinates 1]
              set x2 [lindex $coordinates 2]
              set y2 [lindex $coordinates 3]
              if {$y1 > $y2} {
                 set temp $y1
                 set y1 $y2
                 set y2 $temp
	      }
              if {$x1 > $x2} {
                 set temp $x1
                 set x1 $x2
                 set x2 $temp
	      }

              if {$x1 == $x2} {
#             vertical wire
                  set wire_snap_x $x1
		  if {$y >= $y1 && $y <= $y2} { 
                     set wire_snap_y $y
		  } else {
  		      if {$y > $y2} {
                          set wire_snap_y $y2
		      } else {
                          set wire_snap_y $y1
		      }
		  }
	      } else {
#             horizontal wire
                  set wire_snap_y $y1
		  if {$x >= $x1 && $x <= $x2} {
                     set wire_snap_x $x
		  } else {
		      if {$x > $x2} {
                         set wire_snap_x $x2
		      } else {
                         set wire_snap_x $x1
		      }
		  }
	      }


              $cur_c itemconfigure $id -fill $COLORS(active)
	  }
      }
  }

}


# Internal procedures that draws lines.  If the lines are manhattan, it
# figures out the correct coords (the mouse is only correct on one coord). 
# The correct coords are stored in SAVE(nx) and SAVE(ny).

proc create_line {x1 y1 x2 y2} {

  global cur_c SAVE COLORS

  if {$SAVE(HV) == 1} {
    # determine whether to be a horizontal or a vertical line
    if {[expr abs($x1-$x2)] >= [expr abs($y1-$y2)]} {
      set SAVE(nx) $x2
      set SAVE(ny) $y1
      return [$cur_c create line $x1 $y1 $x2 $y1 -tag tmp -fill $COLORS(wire)]
    } else {
      set SAVE(nx) $x1
      set SAVE(ny) $y2
      return [$cur_c create line $x1 $y1 $x1 $y2 -tag tmp -fill $COLORS(wire)]
    }
  } else {
    set SAVE(nx) $x2
    set SAVE(ny) $y2
    set SAVE(dir) "B"
    return [$cur_c create line $x1 $y1 $x2 $y2 -tag tmp -fill $COLORS(wire)]
  }
}

# given a wire id and the end from which to stretch (either "begin" or "end")
# stretch the wire appropriately

proc stretch_wire {id end dx dy} {

  global cur_c

  set coords [$cur_c coords $id]
  if {$coords == ""} {
    return
  }

  if {$end == "begin"} {
    $cur_c coords $id [expr [lindex $coords 0] + $dx] \
	[expr [lindex $coords 1] + $dy] [lindex $coords 2] [lindex $coords 3]
  } else {
    $cur_c coords $id [lindex $coords 0] [lindex $coords 1] \
	    [expr [lindex $coords 2] + $dx] [expr [lindex $coords 3] + $dy] 
  }
}
