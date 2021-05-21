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


# Deselects everything and then selects the given id.
# if the optional add is given, only adds to selection or
# unselects if already selected

proc select_id {id {add ""}} {

  global cur_c cur_s COLORS

  # get rid of any edit markers, and other temp. stuff
  $cur_c delete tmp

  set tag [find_origin_tag $id]

  # in "special" mode, if the object is already selected, just return
  if {$add == "special"} {
    if {[is_tagged $id selected]} {
      return
    } else {
      set add ""
    }
  }

  if {$add == ""} {

    upvar #0 SUE_$cur_s data

    if { $data(type) == "I" } {
        setup_icon_colors_selected
    } else {
        setup_schematic_colors_selected
    }
    $cur_c dtag selected

    # select 
    $cur_c addtag selected withtag $tag

    # change color to show selected
    $cur_c itemconfigure selected -fill $COLORS(selected)
    $cur_c itemconfigure selected&&arc -outline $COLORS(selected)

  } else {
    # in add selection mode, deselect if selected
    if {[is_tagged $id selected]} {
      # unselect 
      $cur_c itemconfigure selected -fill $COLORS(active)
      $cur_c itemconfigure selected&&arc -outline $COLORS(active)
      $cur_c dtag $tag selected

    } else {
      # select 
      $cur_c addtag selected withtag $tag

      # change color to show selected
      $cur_c itemconfigure selected -fill $COLORS(selected)
      $cur_c itemconfigure selected&&arc -outline $COLORS(selected)
    }
  }
  display_selection
}

proc setup_icon_colors_selected {} {

global cur_c COLORS

  $cur_c itemconfigure selected&&icon_pinbox -fill $COLORS(icon_pinbox)
  $cur_c itemconfigure selected&&(wire||dot) -fill $COLORS(wire)
  $cur_c itemconfigure selected&&draw_item -fill $COLORS(icon)
  $cur_c itemconfigure selected&&icon_text_param -fill $COLORS(icon_text_param)
  $cur_c itemconfigure selected&&term -fill $COLORS(icon_pinbox)
  $cur_c itemconfigure selected&&draw_item&&scaletext -fill $COLORS(icon_text)
  $cur_c itemconfigure selected&&icon&&!scaletext -fill $COLORS(icon)
  $cur_c itemconfigure selected&&icon_text -fill $COLORS(icon_text)
  $cur_c itemconfigure selected&&term -fill $COLORS(icon_pinbox)
  $cur_c itemconfigure selected&&arc -outline $COLORS(icon)
  $cur_c raise selected&&term

}

proc setup_schematic_colors_selected {} {

global cur_c COLORS

  $cur_c itemconfigure selected&&icon_pinbox -fill $COLORS(icon_pinbox)
  $cur_c itemconfigure selected&&(wire||dot) -fill $COLORS(wire)
  $cur_c itemconfigure selected&&draw_item -fill $COLORS(draw_item)

  $cur_c itemconfigure selected&&icon -fill $COLORS(icon)
  $cur_c itemconfigure selected&&icon_text -fill $COLORS(icon_text)
  $cur_c itemconfigure selected&&icon_text_param -fill $COLORS(icon_text_param)
  $cur_c itemconfigure selected&&term -fill $COLORS(icon_pinbox)
  $cur_c itemconfigure selected&&arc -outline $COLORS(icon)
  $cur_c raise selected&&term

}


# deselects all then selects all ids given

proc select_ids {ids} {

  global cur_c cur_s COLORS

  # get rid of any edit markers, and other temp. stuff
  $cur_c delete tmp

  # deselect everything

  upvar #0 SUE_$cur_s data

  if { $data(type) == "I" } {
        setup_icon_colors_selected
    } else {
        setup_schematic_colors_selected
    }

  $cur_c dtag selected

  foreach id $ids {
    set tag [find_origin_tag $id]

    # select 
    $cur_c addtag selected withtag $tag
  }

  # change color to show selected
  $cur_c itemconfigure selected -fill $COLORS(selected)
  $cur_c itemconfigure selected&&arc -outline $COLORS(selected)
  
  display_selection
}


# displays selection in message box

proc display_selection {} {

  global cur_c cur_s
  global WIN WIN_DATA

  set sel_id [lindex [$cur_c find withtag selected] 0]
  if {$sel_id == ""} {
    # nothing selected
    set WIN_DATA($WIN,display_msg) ""
    return
  }

  set id [find_origin $sel_id]

  upvar #0 ${cur_s}_inst${id} i_data
  set name [use_first i_data(_name)]

  set type [find_type $id]
  set WIN_DATA($WIN,display_msg) "selected: $type $name \#$id"
}


proc find_type {id} {

  global cur_c cur_s

  if {[is_tagged $id origin]} {
    set tags [$cur_c gettags $id]
    set name [string range [lindex $tags [lsearch $tags "icon_*"]] 5 end]

    upvar #0 ${cur_s}_inst${id} i_data

    return "$name $i_data(orient)"
  }

  if {[is_tagged $id wire]} {
    return wire
  }

  if {[is_tagged $id origin_icon]} {
    return origin
  }

  if {[is_tagged $id draw_item]} {
    set type [$cur_c type $id]
    return "$type draw_item"
  }

  if {[is_tagged $id open]} {
    return open
  }

  if {[is_tagged $id dot]} {
    return dot
  }
}


##############################################################################
# Utility procedures for stroking out a rectangle on the canvas and 
# selecting what's inside of the stroked out rectangle
# and also does icon select
# setup_select_region start select region mode
##############################################################################
proc icon_select_and_setup_select_region {x y x_snap y_snap {add ""}} {

 global cur_c cur_s COLORS SAVE
 global NOSNAP_XY SNAP_XY
 global DISABLE_CANVAS_EVENT

 if {$DISABLE_CANVAS_EVENT == 1} {
    set DISABLE_CANVAS_EVENT 0
    return
 }
 set id [$cur_c find withtag current]

 set tag_selected [is_tagged $id selected]  
 set tag_grid [is_tagged $id grid]
 set tag [find_origin_tag $id]

#### Do icon_select operation if there is an icon to select and ####
#### it is not a grid line ####
 if {$tag != "" && $tag_grid != 1} {
  if {$tag_selected} {
     setup_drag_move_mode $x_snap $y_snap
     return
  }
  select_id $id $add

#### Otherwise, create a selection box
 } else {

  global WIN WIN_DATA

  enter_mode select_region

  set WIN_DATA($WIN,display_msg) "Drag select box"

  bind $cur_c <Any-B1-Motion> \
      "select_region_drag $NOSNAP_XY; set SCROLL(status) on; auto_scroll \[incrX SCROLL(mem)\] $SNAP_XY"
  bind $cur_c <Any-B1-ButtonRelease> "end_select_region; set SCROLL(status) off"
#  bind $cur_c <Any-Escape> "abort_select_region; set SCROLL(status) off"

  # toggle mode with shift key
#  bind $cur_c <Any-space> {toggle SAVE(mode) $IDIOT_DELAY}

  if {$add == ""} {
    # deselect everything

    upvar #0 SUE_$cur_s data

    if { $data(type) == "I" } {
        setup_icon_colors_selected
    } else {
        setup_schematic_colors_selected
    }
    $cur_c dtag selected

  }

  # get rid of any edit markers, and other temp. stuff
  $cur_c delete tmp

  set SAVE(x) $x
  set SAVE(y) $y

  set SAVE(lastx) $x
  set SAVE(lasty) $y

  set SAVE(mode) 0
  set SAVE(add) $add

  $cur_c create line $SAVE(x) $SAVE(y) $SAVE(x) $SAVE(y) \
      -fill $COLORS(stroke_box) -tags "stroke_box sb1"
  $cur_c create line $SAVE(x) $SAVE(y) $SAVE(x) $SAVE(y) \
      -fill $COLORS(stroke_box) -tags "stroke_box sb2"
  $cur_c create line $SAVE(x) $SAVE(y) $SAVE(x) $SAVE(y) \
      -fill $COLORS(stroke_box) -tags "stroke_box sb3"
  $cur_c create line $SAVE(x) $SAVE(y) $SAVE(x) $SAVE(y) \
      -fill $COLORS(stroke_box) -tags "stroke_box sb4"
 }
}


##############################################################################
# drags the stroke box
##############################################################################
proc select_region_drag {x y} {

  global cur_c SAVE

  if {![info exists SAVE(mode)]} {
    # if you got here on mistake
    abort_select_region
    return
  }

  if {$SAVE(mode) == 0} {
    # resize drag box to cursor location
    $cur_c coords sb1 $SAVE(x) $SAVE(y) $x $SAVE(y)

    $cur_c coords sb2 $SAVE(x) $SAVE(y) $SAVE(x) $y
    $cur_c coords sb3 $x $SAVE(y) $x $y
    $cur_c coords sb4 $SAVE(x) $y $x $y

  } else {
    # move the box
    set dx [expr ${x} - ${SAVE(lastx)}]
    set dy [expr ${y} - ${SAVE(lasty)}]

    $cur_c move sb1 $dx $dy
    $cur_c move sb2 $dx $dy
    $cur_c move sb3 $dx $dy
    $cur_c move sb4 $dx $dy

    incr SAVE(x) $dx
    incr SAVE(y) $dy
  }

  set SAVE(lastx) $x
  set SAVE(lasty) $y
}

##############################################################################
# select what's in the stroke box
##############################################################################
proc end_select_region {} {

  global cur_c COLORS
  
  # select everything totally enclosed by the stroke bbox
  set bbox [$cur_c bbox stroke_box]

  # delete the dragged box since no longer needed
  $cur_c delete stroke_box

  select_in_bbox $bbox

  $cur_c itemconfigure selected -fill $COLORS(selected)
  $cur_c itemconfigure selected&&arc -outline $COLORS(selected)

  display_selection

  leave_mode select_region
}

proc abort_select_region {} {

  global cur_c

  # delete the dragged box since no longer needed
  $cur_c delete stroke_box

  leave_mode select_region
}

# doesn't select dots and opens.

proc select_in_bbox {bbox} {

  global cur_c

  if {$bbox == ""} {
    return
  }

  set ids [eval $cur_c find enclosed $bbox]

  # if nothing was enclosed then return nothing
  if {$ids == ""} {
    return ""
  }

  set x1 [lindex $bbox 0]
  set y1 [lindex $bbox 1]
  set x2 [lindex $bbox 2]
  set y2 [lindex $bbox 3]

  # instid is a speed hack since is_tagged takes about 1ms
  set instid ""
  foreach id $ids {
    if {$instid != "" && [is_tagged $id $instid]} {
      continue
    }

    if {[is_tagged $id wire] || [is_tagged $id draw_item] || \
	    [is_tagged $id dot]} {
      $cur_c addtag selected withtag $id
      set instid ""
      continue
    }

    if {[is_tagged $id origin]} {
      set instid inst$id
      set instbbox [$cur_c bbox inst$id]

      # select an inst only if fully enclosed by original bbox
      if {[lindex $instbbox 0] > $x1 && [lindex $instbbox 1] > $y1 && \
	      [lindex $instbbox 2] < $x2 && [lindex $instbbox 3] < $y2} {
	$cur_c addtag selected withtag inst$id
      }
    }
  }
  ready
}


# selects the entire wire connected to the segment under the cursor
# if branch is non-nil, then the segment only up to the first solder dot is
# selected

proc select_entire_wire {{branch stop}} {

  global cur_c
  global DISABLE_CANVAS_EVENT

  # stop general canvas binding events
  set DISABLE_CANVAS_EVENT 1

  set id [$cur_c find withtag current]

  if {[is_tagged $id wire]} {

    set coords [$cur_c coords $id]
    eval find_attached_wire [lrange $coords 0 1] wires $branch
    eval find_attached_wire [lrange $coords 2 3] wires $branch

  } elseif {[is_tagged $id dot] || [is_tagged $id open]} {
    eval find_attached_wire [center $id] wires $branch

  } else {
    # who knows how we got here
    return
  }

  if {[info exists wires] != 1} {
    return
  }

  set ids ""
  foreach id [array names wires] {
    lappend ids $id
  }

  # finally select the entire wire
  select_ids $ids
}


proc find_attached_wire {x y array branch} {

  global cur_c scale
  upvar 1 $array wires

  set del [expr ${scale}/3.0]

  set ids [$cur_c find overlapping [expr ${x} - ${del}] [expr ${y} - ${del}] \
	       [expr ${x} + ${del}] [expr ${y} + ${del}]]

  if {$branch == "stop"} {
    foreach id $ids {
      if {[is_tagged $id dot]} {
	return
      }
    }
  }

  foreach id $ids {
    if {[info exists wires($id)]} {
      # we've already been here
      continue
    }

    if {[is_tagged $id wire]} {
      set wires($id) 1

      # this is the slow braindead way of doing it, but it's easy...
      set coords [$cur_c coords $id]
      eval find_attached_wire [lrange $coords 0 1] wires $branch
      eval find_attached_wire [lrange $coords 2 3] wires $branch
    }
  }
}
