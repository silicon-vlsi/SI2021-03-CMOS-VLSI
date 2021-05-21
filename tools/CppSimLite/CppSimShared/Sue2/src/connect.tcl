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

# Procedures for finding and displaying connectivity


# Show_connect_point will place a hollow square on unconnected points, leave
# points with two connections untouched and place a solder dot (a black
# solid rectangle) on points of 3 or more connections (e.g. "T" connections).

# If optionally passed a third argument, will try to clean up complicated
# connections by merging wires.

proc show_connect_point {x y {clean ""}} {

  global cur_c scale COLORS

  if {$x == "" || $y == ""} {
    return
  }

  set connects [find_connect $x $y]

  set no_connect [llength $connects]

  # nothing here
  if {$no_connect == 0} {
    return
  }

  set del [expr ${scale} / 3.0]

  # is it an open?
  if {$no_connect == 1} {
    set x1 [expr ${x} - ${del}]
    set y1 [expr ${y} - ${del}]
    set x2 [expr ${x} + ${del}] 
    set y2 [expr ${y} + ${del}]

    # make an open rectangle "open"
    $cur_c create line $x1 $y1 $x2 $y1 $x2 $y2 $x1 $y2 $x1 $y1 -tags open \
	-fill $COLORS(icon_pinbox)
    # lower these in the display list so they are least likely to be current
    # $cur_c lower open
    return
  }

  # {$no_connect == 2} means show no connection info

  # do we need a solder dot?
  if {$no_connect > 2} {

    # make a filled rectangle "solder dot"
    $cur_c create rectangle [expr ${x} - ${del}] [expr ${y} - ${del}] \
       [expr ${x} + ${del}] [expr ${y} + ${del}] -fill $COLORS(wire) -tags dot

    # if we are not asked to clean up, then we are done.
    if {$clean == ""} {
      return
    }

    # If 2 wires overlap, fix it.
    foreach id $connects {
      set dir [wire_compass_direction $id $x $y]

      # ignore terminals and non-manhattan lines
      if {$dir == "T" || $dir == ""} {
	continue
      }

      if {[info exists dirs($dir)]} {
	set wire1_coords [$cur_c coords $id]
	if {[eval nearby $x $y [lrange $wire1_coords 0 1]] == 1} {
	  set coord1 [lrange $wire1_coords 2 3]
	} else {
	  set coord1 [lrange $wire1_coords 0 1]
	}
	
	set wire2_coords [$cur_c coords $dirs($dir)]
	if {[eval nearby $x $y [lrange $wire2_coords 0 1]] == 1} {
	  set coord2 [lrange $wire2_coords 2 3]
	} else {
	  set coord2 [lrange $wire2_coords 0 1]
	}

	if {[is_tagged $id selected] || [is_tagged $dirs($dir) selected]} {
	  set select selected
	} else {
	  set select ""
	}

	$cur_c delete $id
	$cur_c delete $dirs($dir)
	set coord1_ind1 [lindex $coord1 1]
	set coord2_ind1 [lindex $coord2 1]
	set coord1_ind0 [lindex $coord1 0]
	set coord2_ind0 [lindex $coord2 0]
	set delta_x_coor [expr ${x} - ${coord1_ind0}]
	set check_del_x [expr abs(${delta_x_coor})]

	  if {$check_del_x < $del} {
	  remake_wires $x "$y $coord1_ind1 $coord2_ind1" V $select
	} else {
	  remake_wires $y "$x $coord1_ind0 $coord2_ind0" H $select
	}
      } else {
	set dirs($dir) $id
      }
    }
    return
  }

  # if we are not asked to clean up, then we are done.
  if {$clean == ""} {
    return
  }

  # If 2 wires should be one wire, merge them into one longer wire
  set dirs("_dummy") ""
  foreach id $connects {
    set dir [wire_direction $id]
    if {$dir == "T" || $dir == "B"} {
      continue
    }

    if {[info exists dirs($dir)]} {
	set wire1_coords [$cur_c coords $id]
	if {[eval nearby $x $y [lrange $wire1_coords 0 1]] == 1} {
	  set coord1 [lrange $wire1_coords 2 3]
	} else {
	  set coord1 [lrange $wire1_coords 0 1]
	}
	
	set wire2_coords [$cur_c coords $dirs($dir)]
	if {[eval nearby $x $y [lrange $wire2_coords 0 1]] == 1} {
	  set coord2 [lrange $wire2_coords 2 3]
	} else {
	  set coord2 [lrange $wire2_coords 0 1]
	}

      if {[is_tagged $id selected] || [is_tagged $dirs($dir) selected]} {
	set select selected
      } else {
	set select ""
      }

      $cur_c delete $id
      $cur_c delete $dirs($dir)
      set id [eval make_wire_selected $coord1 $coord2 $select]
    }

    set dirs($dir) $id
  }
}

proc wire_compass_direction {id x y} {

  global cur_c scale

  # if the wire no longer exists, just punt
  if {[$cur_c type $id] == ""} {
    return
  }

  if {[is_tagged $id "term"]} {
    return T
  }

  set coords [$cur_c coords $id]
  set x1 [lindex $coords 0]
  set y1 [lindex $coords 1]
  set x2 [lindex $coords 2]
  set y2 [lindex $coords 3]

  set del [expr ${scale}/2.0]
  set del_xc [expr ${x1} - ${x2}]
  set del_yc [expr ${y1} - ${y2}]
  set del_y [expr ${y} - ${y2}]
  set del_x [expr ${x} - ${x2}]

  set abs_del_xc [expr abs(${del_xc})]
  set abs_del_y [expr abs(${del_y})]
  set abs_del_yc [expr abs(${del_yc})]
  set abs_del_x [expr abs(${del_x})]

  if {$abs_del_xc < $del} {
    if {$abs_del_y < $del} {
      if {$y1 > $y} {
	return S
      } else {
	return N
      }
    } else {
      if {$y2 > $y} {
	return S
      } else {
	return N
      }
    }
  }
  if {$abs_del_yc < $del} {
      if {$abs_del_x < $del} {
      if {$x1 > $x} {
	return W
      } else {
	return E
      }
    } else {
      if {$x2 > $x} {
	return W
      } else {
	return E
      }
    }
  }
  return ""
}

###########################################################################
# find_connect searches through a list of canvas ids and checks:
# 1. If they are a dot or open rectangle, then they are deleted
# 2. If they are a wire then 
#    a. if the wire is too short (a point) then it's deleted
#    b. if the wire isn't too small then if it has an end near "x" and "y"
#       then it's canvas id is appended to "term_list"
#    c. if neither wire endpoint is nearby, split the wire since we only
#       connect to an endpoint.
# Finally the list of wires from 2b are returned by this function
###########################A################################################

proc find_connect {x y} {

  global cur_c scale

  set del [expr $scale/3.0]

  set ids [$cur_c find overlapping [expr ${x} - ${del}] [expr ${y} - ${del}] \
	       [expr ${x} + ${del}] [expr ${y} + ${del}]]

  set term_list ""
  foreach id $ids {
    if {[is_tagged $id "dot"] || [is_tagged $id "open"]} {
      $cur_c delete $id
      continue
    }

    if {[is_tagged $id "term"]} {
      lappend term_list $id
      continue
    }

    if {[is_tagged $id "wire"]} {
      set wire_coords [$cur_c coords $id]
      set p1 [lrange $wire_coords 0 1]
      set p2 [lrange $wire_coords 2 3]

      # point wires should already be wasted, but if not, do it
      if {[eval nearby $p1 $p2] == 1} {
	$cur_c delete $id
	continue
      }

      if {[eval nearby $x $y $p1] == 1 || [eval nearby $x $y $p2] == 1} {
	lappend term_list $id
      } else {
	# split the wire so we can connect to it
	if {[is_tagged $id selected]} {
	  set select selected
	} else {
	  set select ""
	}
	$cur_c delete $id
	set id [eval make_wire_selected $x $y $p1 $select]
	lappend term_list $id
	set id [eval make_wire_selected $x $y $p2 $select]
	lappend term_list $id
      }
    }
  }
  return $term_list
}


# called when loading a file.  The assumoption is that everything is corrent,
# only the opens and nodes aren't shown.

proc show_connect_point_fast {x y {bogus ""}} {

  global cur_c scale COLORS

  set check_connect [no_connects $x $y]
  switch $check_connect {

    0 {
      return
    }

    1 {
      set del [expr ${scale}/3.0]
      set x1 [expr ${x} - ${del}] 
      set y1 [expr ${y} - ${del}]
      set x2 [expr ${x} + ${del}] 
      set y2 [expr ${y} + ${del}]

      # make an open rectangle "open"
      $cur_c create line $x1 $y1 $x2 $y1 $x2 $y2 $x1 $y2 $x1 $y1 -tags open \
	  -fill $COLORS(icon_pinbox)

      return
    }

    2 {
      return
    }

    default {
	set del [expr ${scale}/3.0]

      # make a filled rectangle "solder dot"
      $cur_c create rectangle [expr ${x} - ${del}] [expr ${y} - ${del}] \
	 [expr ${x} + ${del}] [expr ${y} + ${del}] -fill $COLORS(wire) -tags dot
      return
    }
  }
}


proc no_connects {x y} {

  global cur_c scale

  set del [expr ${scale}/3.0]

  set ids [$cur_c find overlapping [expr ${x} - ${del}] [expr ${y} - ${del}] \
	       [expr ${x} + ${del}] [expr ${y} + ${del}]]

  set connects 0
  foreach id $ids {
    if {[is_tagged $id wire] || [is_tagged $id term]} {
      incr connects
    }
  }
  return $connects
}


# Removes all connection symbols on the given point.  Used before dragging.

proc remove_connect_point {x y} {

  global cur_c scale

  set del [expr ${scale}/3.0]
  set ids [$cur_c find overlapping [expr ${x} - ${del}] [expr ${y} - ${del}] \
	       [expr ${x} + ${del}] [expr ${y} + ${del}]]

  foreach id $ids {
    if {[is_tagged $id "dot"] || [is_tagged $id "open"]} {
      $cur_c delete $id
    }
  }
}


# simply calls remove_connect_point on the two endpoint of the wire.

proc remove_connect_wire {wire} {

  global cur_c

  set coords [$cur_c coords $wire]
  if {$coords == ""} {
    return
  }

  remove_connect_point [lindex $coords 0] [lindex $coords 1]
  remove_connect_point [lindex $coords 2] [lindex $coords 3]
}


# show_connect_wire calls show_connect_point on the two endpoints
# of the wire.  It also looks to see if there are any terms, open,
# or dots overlapping the current wire if in clean mode.

proc show_connect_wire {wire {clean ""}} {

  global cur_c

  set coords [$cur_c coords $wire]
  if {$coords == ""} {
    return
  }

  show_connect_point [lindex $coords 0] [lindex $coords 1] $clean
  show_connect_point [lindex $coords 2] [lindex $coords 3] $clean

  if {$clean != "clean"} {
    return
  }

  eval $cur_c addtag connect overlapping $coords

  # now remove all the things but terminals, opens, and dots.
  intersect_tag connect1 connect term
  intersect_tag connect2 connect open
  intersect_tag connect3 connect dot
  $cur_c dtag connect
  $cur_c addtag connect1 withtag connect2
  $cur_c addtag connect1 withtag connect3
  $cur_c dtag connect2
  $cur_c dtag connect3

  foreach id [$cur_c find withtag connect1] {
    set center [center $id]
    if {$center != ""} {
      eval show_connect_point $center $clean
    }
  }

  $cur_c dtag connect1
}


# calls show_connect_point on the location of all terminals with a
# given tag

proc show_term_connects {tag {clean ""}} {

  global cur_c

  foreach id [get_intersect_tag $tag term] {
    set center [center $id]
    show_connect_point [lindex $center 0] [lindex $center 1] $clean
  }
}


# calls remove_connect_point on the location of all terminals with a 
# given tag

proc remove_term_connects {tag} {

  global cur_c

  foreach id [get_intersect_tag $tag term] {
    eval remove_connect_point [center $id]
  }
}



# called after loading a file, duplicating, etc.
#
# calls show_connect_point on the location of all terminals and wires.
# If tag is given will do so only on those items withtag "tag".

proc show_connects {{tag ""} {clean_in ""}} {

  global cur_c cur_s

  if {[is_icon $cur_s]} {
    # don't show connects in an icon
    return
  }

  if {$tag == ""} {
    set term_ids [$cur_c find withtag term]
    set wire_ids [$cur_c find withtag wire]

  } else {
    set term_ids [get_intersect_tag $tag term]
    set wire_ids [get_intersect_tag $tag wire]

    # special case for moving just dots or just opens
    if {$term_ids == "" && $wire_ids == ""} {
      set term_ids [concat [get_intersect_tag $tag open] \
			[get_intersect_tag $tag dot]]
    }
  }

  if {$clean_in == "fast"} {
    set point_proc show_connect_point_fast
  } else {
    set point_proc show_connect_point
  }

  foreach id $term_ids {
    set center [round_list [center $id]]
    if {[info exists mark($center)]} {
      # already been here
      continue
    }

    set mark($center) 1
    $point_proc [lindex $center 0] [lindex $center 1] $clean_in
  }

  foreach id $wire_ids {
    set coords [round_list [$cur_c coords $id]]
    set p1 [lrange $coords 0 1]
    set p2 [lrange $coords 2 3]

    # have we been here?
    if {[info exists mark($p1)] != 1} {

      # remember that we were here and show connect
      set mark($p1) 1
      $point_proc [lindex $p1 0] [lindex $p1 1] $clean_in
    }

    # have we been here?
    if {[info exists mark($p2)] != 1} {
      # remember that we were here and show connect
      set mark($p2) 1
      $point_proc [lindex $p2 0] [lindex $p2 1] $clean_in
    }
  }

  # lower opens in the display list so they are least likely to be current
  # $cur_c lower open
}


##############################################################################
# removes dots and opens attached to tag (usually selected)
##############################################################################

proc remove_connects {{tag ""}} {

  global cur_c scale

  if {$tag == ""} {
    $cur_c delete dot
    $cur_c delete open

    # that was easy, too bad it is never used
    return
  }

  # waste all temporary stuff
  $cur_c delete tmp

  set bbox [$cur_c bbox selected]
  if {$bbox == ""} {
    return
  }

  # mark all potential dot/opens with the tag "remove"
  eval $cur_c addtag remove overlapping $bbox
  $cur_c dtag icon remove
  $cur_c dtag wire remove
  $cur_c dtag draw_item remove

  set del2 [expr 2*${scale}/3.0]

  foreach id [$cur_c find withtag remove] {
    set coords [$cur_c coords $id]
    set x [lindex $coords 0]
    set y [lindex $coords 1]

    foreach test_id [$cur_c find overlapping $x $y \
			 [expr ${x}+${del2}] [expr ${y}+${del2}]] {
      if {[is_tagged $test_id selected]} {
	if {[is_tagged $test_id wire] || [is_tagged $test_id term]} {
	  $cur_c delete $id
	  break
	}
      }
    }
  }

  $cur_c dtag remove
}