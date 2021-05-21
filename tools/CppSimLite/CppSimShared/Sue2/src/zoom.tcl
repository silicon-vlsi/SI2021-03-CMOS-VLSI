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
# Scales everything in the schematic to new_scale.  New_scale will be rounded
# to the closest integer so that the gridding works.  
#
# Note that this will shift the apparent center of the screen usually.  the
# zoom proc below fixes this.
##############################################################################
proc scale_canvas {new_scale} {

  global cur_c cur_s scale FONT WIN
 
  # scales can only be integers and are bounded by 1 and FONT(MAX)
  set new_scale_rnd [min $FONT(MAX) [max 1 [expr round(${new_scale})]]]

  # Return if we are already there
  if {$scale == $new_scale_rnd} {
    return
  } 

  set scalefactor [expr 1.0 * ${new_scale_rnd} / ${scale}]

  $cur_c scale all 0 0 $scalefactor $scalefactor
#  $WIN.hscroll set [lindex [$cur_c xview] 0] [lindex [$cur_c xview] 1]
#  $WIN.vscroll set [lindex [$cur_c yview] 0] [lindex [$cur_c yview] 1]

  set scale $new_scale_rnd 
  global SUE_$cur_s
  set SUE_${cur_s}(scale) $scale

# puts "scale = $scale"

  # don't want this too small at smallest scalest or you can't hit anything
  $cur_c configure -closeenough [expr 3 + ${scale}/2.0 - sqrt(${scale})]

  # scale fonts (badly).  Allows for 3 font sizes on screen: standard,
  # large, and small. 
  $cur_c itemconfigure size_standard -font $FONT(standard,$scale)
  $cur_c itemconfigure size_small -font $FONT(small,$scale)
  $cur_c itemconfigure size_large -font $FONT(large,$scale)
}


proc visible_bbox {} {

  global cur_c scale WIN

  set canv_bbox [lindex [$cur_c configure -scrollregion] 4]

  set x_left_canv [lindex $canv_bbox 0]
  set y_left_canv [lindex $canv_bbox 1]
  set x_right_canv [lindex $canv_bbox 2]
  set y_right_canv [lindex $canv_bbox 3]
  set wid_canv_bbox [expr ${x_right_canv} - ${x_left_canv}]
  set height_canv_bbox [expr ${y_right_canv} - ${y_left_canv}]
  set xview_curc0 [lindex [$cur_c xview] 0] 
  set yview_curc0 [lindex [$cur_c yview] 0] 
  set xview_curc1 [lindex [$cur_c xview] 1] 
  set yview_curc1 [lindex [$cur_c yview] 1] 
  
  set xleft [expr ${xview_curc0} * ${wid_canv_bbox} \
		 + ${x_left_canv}]
  set yleft [expr ${yview_curc0} * ${height_canv_bbox} \
		 + ${y_left_canv}]
  set xright [expr ${xview_curc1} * ${wid_canv_bbox} \
		  + ${x_left_canv}]
  set yright [expr ${yview_curc1} * ${height_canv_bbox} \
		  + ${y_left_canv}]


  return "$xleft $yleft $xright $yright"
}


# this routines assures that the center of the visual screen doesn't
# move when you scale in and out.

proc zoom {{factor 0.5}} {

  global cur_c scale

  # draw a little temporary rectangle at the center of the screen
  set center [center_bbox [visible_bbox]]
  set id [eval $cur_c create rectangle $center $center -tags box]

  set new_factor $factor
  set new_scale [expr round(${scale} * ${new_factor})]

  # insure that we are actually changing the scale
  while {$new_scale == $scale} {
     set new_factor [expr ${new_factor} * ${new_factor}]
     set new_scale [expr round(${scale} * ${new_factor})]
  }

  scale_canvas $new_scale
  # center the temporary rectangle
  eval center_canvas [round_list [center $id]]

  # lose the temporary rectangle
  $cur_c delete box
}

proc zoom_to_fit {} {

  global cur_c scale

  $cur_c addtag object all
  $cur_c dtag grid object

  set bbox [$cur_c bbox object]

  if {$bbox == ""} {
    return
  }

  # This will size up the fonts for getting the correct bbox
  # this, I think is due to a weirdness in the fonts

  scale_canvas 10

  # $cur_c delete bbox
  set bbox [$cur_c bbox object]

  zoom_to_bbox $bbox
  eval center_canvas [center_bbox [$cur_c bbox object]]

  $cur_c dtag object
}


proc get_schematic_bbox {} {

  global cur_c scale

  # Don't look at the grid if it exists
  $cur_c addtag object all
  $cur_c dtag grid object

  set bbox [$cur_c bbox object]
  if {$bbox == ""} {
    return "0 0 0 0"
  }

  $cur_c dtag object

  return $bbox
}


proc zoom_to_bbox {bbox {border 3}} {

  global cur_c scale
  global WIN

  set new_border [expr ${border} * ${scale}]
  set bbox0 [lindex $bbox 0]
  set bbox1 [lindex $bbox 1]
  set bbox2 [lindex $bbox 2]
  set bbox3 [lindex $bbox 3]

  set x1 [expr ${bbox0} - ${new_border}]
  set y1 [expr ${bbox1} - ${new_border}]
  set x2 [expr ${bbox2} + ${new_border}]
  set y2 [expr ${bbox3} + ${new_border}]

  set desired_width [expr ${x2} - ${x1}]
  set desired_height [expr ${y2} - ${y1}]

  set view_bbox [visible_bbox]
  
  set bbox_v0 [lindex $view_bbox 0]
  set bbox_v1 [lindex $view_bbox 1]
  set bbox_v2 [lindex $view_bbox 2]
  set bbox_v3 [lindex $view_bbox 3]
  set old_width [expr ${bbox_v2} - ${bbox_v0}]
  set old_height [expr ${bbox_v3} - ${bbox_v1}]


  set xratio [expr ${old_width}/${desired_width}]
  set yratio [expr ${old_height}/${desired_height}]

  if {$yratio > $xratio} {
    set ratio $xratio
  } else {
    set ratio $yratio
  }

  set new_scale [expr int(${ratio} * ${scale})]

  scale_canvas $new_scale
}


# places the given coords at the center of the window

proc center_canvas {x y} {

  global cur_c scale WIN

# specs of visible region
  set current_bbox [visible_bbox]

  set bbox0 [lindex $current_bbox 0]
  set bbox1 [lindex $current_bbox 1]
  set bbox2 [lindex $current_bbox 2]
  set bbox3 [lindex $current_bbox 3]
  set width_bbox [expr ${bbox2} - ${bbox0}]
  set height_bbox [expr ${bbox3} - ${bbox1}]

# specs of overall region
  set canv_bbox [lindex [$cur_c configure -scrollregion] 4]

  set x_left_canv [lindex $canv_bbox 0]
  set y_left_canv [lindex $canv_bbox 1]
  set x_right_canv [lindex $canv_bbox 2]
  set y_right_canv [lindex $canv_bbox 3]
  set wid_canv_bbox [expr ${x_right_canv} - ${x_left_canv}]
  set height_canv_bbox [expr ${y_right_canv} - ${y_left_canv}]

  set new_x [expr (${x} - ${width_bbox}/2 - ${x_left_canv})/${wid_canv_bbox}]
  set new_y [expr (${y} - ${height_bbox}/2 - ${y_left_canv})/${height_canv_bbox}]

  $cur_c xview moveto $new_x
  $cur_c yview moveto $new_y
}

# pans the window

proc pan_canvas {x y} {

  global cur_c scale WIN PAN_X_ORIG PAN_Y_ORIG
  global PAN_WIDTH PAN_HEIGHT

  set current_bbox [visible_bbox]
  set bbox0 [lindex $current_bbox 0]
  set bbox1 [lindex $current_bbox 1]
  set canv_x [expr ${x} - ${bbox0}]
  set canv_y [expr ${y} - ${bbox1}]

  set new_x [expr 0.5 + (-${canv_x} + ${PAN_X_ORIG})/${PAN_WIDTH}]
  set new_y [expr 0.5 + (-${canv_y} + ${PAN_Y_ORIG})/${PAN_HEIGHT}]

  $cur_c xview moveto $new_x
  $cur_c yview moveto $new_y
}


proc delta_cursor {x y} {

  set bbox [visible_bbox]

  set bbox0 [lindex $bbox 0]
  set bbox1 [lindex $bbox 1]
  set bbox2 [lindex $bbox 2]
  set bbox3 [lindex $bbox 3]
  if {$x < ${bbox0}} {
      set deltax [expr ${x} - ${bbox0}]
  } elseif {$x > ${bbox2}} {
      set deltax [expr $x - ${bbox2}]
  } else {
      set deltax 0
  }

  if {$y < ${bbox1}} {
      set deltay [expr $y - ${bbox1}]
  } elseif {$y > ${bbox3}} {
      set deltay [expr $y - ${bbox3}]
  } else {
      set deltay 0
  }

  return "$deltax $deltay"
}


# this routine will scroll the screen if the cursor is off screen.  
# Unfortunately, it changes x and y and cannot tell the motion routine 
# so things sometimes look weird for awhile.

proc auto_scroll {mem x y} {

  global cur_c WIN SCROLL

  if {$SCROLL(status) != "on" || $mem < $SCROLL(mem)} {
    return
  }

#### fix this later
  return

  set delta [delta_cursor $x $y]
  set scroll_inc [lindex [$cur_c configure -scrollincrement] 4]

  set xwin [lindex [$WIN.hscroll get] 2]
  set xinc [expr round([lindex ${delta} 0] / ${scroll_inc})]
  $cur_c xview [expr ${xwin} + ${xinc}]

  set ywin [lindex [$WIN.vscroll get] 2]
  set yinc [expr round([lindex ${delta} 1] / ${scroll_inc})]
  $cur_c yview [expr ${ywin} + ${yinc}]

  if {$SCROLL(just_packed) == 1} {
    set SCROLL(just_packed) 0
    if {$xinc != 0 || $yinc != 0} {
      puts "SCROLL $WIN: delta = $delta, xwin = $xwin, ywin = $ywin, xinc = $xinc, yinc = $yinc"
      puts "$WIN x = [$WIN.hscroll get]"
      puts "$WIN y = [$WIN.vscroll get]"
      puts [visible_bbox]
    }
  }

  after 250 \
      "auto_scroll $mem [expr ${x} + ${scroll_inc}*${xinc}] [expr ${y}+${scroll_inc}*${yinc}]"
}


# allows the user to pan the screen

proc setup_pan_canvas {x y} {

  global cur_c
  global WIN WIN_DATA SNAP_XY KEYS PAN_X_ORIG PAN_Y_ORIG
  global PAN_WIDTH PAN_HEIGHT

# modified this from the center mode - kept the mode the same
#    name as before out of laziness
  enter_mode center abort_pan_canvas

  set PAN_X_ORIG $x
  set PAN_Y_ORIG $y

# specs of overall region
  set canv_bbox [lindex [$cur_c configure -scrollregion] 4]
  set bbox0 [lindex $canv_bbox 0]
  set bbox1 [lindex $canv_bbox 1]
  set bbox2 [lindex $canv_bbox 2]
  set bbox3 [lindex $canv_bbox 3]
  set PAN_WIDTH [expr ${bbox2} - ${bbox0}]
  set PAN_HEIGHT [expr ${bbox3} - ${bbox1}]

  set WIN_DATA($WIN,display_msg) \
      "Drag mouse to pan, release Button-1 to abort"

   bind $cur_c <Any-B1-Motion> "pan_canvas $SNAP_XY"

  bind $cur_c <Any-B1-ButtonRelease>  "abort_pan_canvas"
#  bind $cur_c <Escape> "abort_pan_canvas"
}

proc center_cursor {x y} {

  global cur_c

  center_canvas $x $y

  leave_mode center
}

proc abort_pan_canvas {} {

  global cur_c

  leave_mode center
}


##############################################################################
# user drags a stroke box which zooms window to contents
##############################################################################
proc setup_zoom_box {} {

  global cur_c COLORS SAVE
  global WIN WIN_DATA SNAP_XY

  enter_mode zoom abort_zoom_box

#  set WIN_DATA($WIN,display_msg) "Button-1 begins zoom box, Escape aborts"

#  bind $cur_c <Any-Button-1> "begin_zoom_box $SNAP_XY"
#  bind $cur_c <Any-Escape> "abort_zoom_box"

  # get rid of any random stroke boxes
  $cur_c delete stroke_box 
}

proc begin_zoom_box {x y} {

  setup_zoom_box

  global cur_c COLORS SAVE tcl_platform
  global WIN WIN_DATA SNAP_XY

  set WIN_DATA($WIN,display_msg) \
      "Drag out zoom box. Escape aborts"

  set SAVE(x) $x
  set SAVE(y) $y

  set SAVE(mode) 0

  $cur_c create line $SAVE(x) $SAVE(y) $SAVE(x) $SAVE(y) \
      -fill $COLORS(stroke_box) -tags "stroke_box sb1"
  $cur_c create line $SAVE(x) $SAVE(y) $SAVE(x) $SAVE(y) \
      -fill $COLORS(stroke_box) -tags "stroke_box sb2"
  $cur_c create line $SAVE(x) $SAVE(y) $SAVE(x) $SAVE(y) \
      -fill $COLORS(stroke_box) -tags "stroke_box sb3"
  $cur_c create line $SAVE(x) $SAVE(y) $SAVE(x) $SAVE(y) \
      -fill $COLORS(stroke_box) -tags "stroke_box sb4"

  if {$tcl_platform(os) == "Darwin"} {
     bind $cur_c <Any-B2-Motion> \
         "drag_zoom_box $SNAP_XY; set SCROLL(status) on; auto_scroll \[incrX SCROLL(mem)\] $SNAP_XY"
     bind $cur_c <Any-B2-ButtonRelease> "end_zoom_box; set SCROLL(status) off"
  } else {
     bind $cur_c <Any-B3-Motion> \
         "drag_zoom_box $SNAP_XY; set SCROLL(status) on; auto_scroll \[incrX SCROLL(mem)\] $SNAP_XY"
     bind $cur_c <Any-B3-ButtonRelease> "end_zoom_box; set SCROLL(status) off"
  }
  bind $cur_c <Any-Escape> "abort_zoom_box; set SCROLL(status) off"

  # toggle mode with shift key
  bind $cur_c <Any-space> {toggle SAVE(mode) $IDIOT_DELAY}
}


set TOGGLE_LOCK 0

proc toggle {var {delay 500}} {

  global TOGGLE_LOCK

  upvar 1 $var a

  if {$TOGGLE_LOCK == 0} {
      set a [expr 1 - ${a}]
  }

  if {$delay > 100} {
    incr TOGGLE_LOCK
    after $delay incr TOGGLE_LOCK -1
  }
}


##############################################################################
# drags the stroke box
##############################################################################
proc drag_zoom_box {x y} {

  global cur_c SAVE

  set info_x [info exists SAVE(x)]
  set info_y [info exists SAVE(y)]
  set info_lastx [info exists SAVE(lastx)]
  set info_lasty [info exists SAVE(lasty)]
  set info_mode [info exists SAVE(mode)]

  if {$info_x == 0} {
     set SAVE(x) $x
  }
  if {$info_y == 0} {
     set SAVE(y) $y
  }
  if {$info_lastx == 0} {
     set SAVE(lastx) $x
  }
  if {$info_lasty == 0} {
     set SAVE(lasty) $y
  }
  if {$info_mode == 0} {
     set SAVE(mode) 0
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
# zooms to zoom box
##############################################################################
proc end_zoom_box {} {

  global cur_c SAVE
  
  # get the bbox of the stroke_box
  set bbox [$cur_c bbox stroke_box]

  zoom_to_bbox $bbox
  eval center_canvas [center_bbox [$cur_c bbox stroke_box]]

  # get rid of stroke box
  $cur_c delete stroke_box 

  leave_mode zoom
}


# aborts the zoom box

proc abort_zoom_box {} {

  global cur_c

  # get rid of stroke box
  $cur_c delete stroke_box 

  leave_mode zoom
}


# toggles the grid on or off depending on status

proc toggle_grid {} {

  global cur_s cur_c
  global SUE_${cur_s} GRID_SPACING

  if {[info exists SUE_${cur_s}(grid)] != 1} {
    set SUE_${cur_s}(grid) 0
  }

  if {[set SUE_${cur_s}(grid)] == 0} {
    set SUE_${cur_s}(grid) 1
    make_grid $GRID_SPACING
  } else {
    set SUE_${cur_s}(grid) 0
    $cur_c delete grid
  }
}

proc turn_on_grid {} {

  global cur_s cur_c
  global SUE_${cur_s} GRID_SPACING

  if {[info exists SUE_${cur_s}(grid)] != 1} {
    set SUE_${cur_s}(grid) 0
  }

  if {[set SUE_${cur_s}(grid)] == 0} {
    set SUE_${cur_s}(grid) 1
    make_grid $GRID_SPACING
  } 
}

proc turn_off_grid {} {

  global cur_s cur_c
  global SUE_${cur_s} GRID_SPACING

  set SUE_${cur_s}(grid) 0
  $cur_c delete grid

}


# change the grid spacing from the default

proc change_grid {} {

  global cur_c cur_s GRID_SPACING

  set rooty_curc [winfo rooty $cur_c]  
  set rootx_curc [winfo rootx $cur_c]  
  set winy [expr ${rooty_curc} + 50]
  set winx [expr ${rootx_curc} + 50]
  set title "Grid Spacing"
  set message "Enter New Grid Spacing:" 
  set prop_list [list [list "Grid Spacing" $GRID_SPACING]]

  # create the menu
  set new_prop_list [prop_menu $winx $winy $message $title $prop_list]
  if {$new_prop_list == "" || $new_prop_list == $prop_list} {
    # empty list means the user hit cancel or didn't change anything
    return
  }

  set GRID_SPACING [lindex [lindex $new_prop_list 0] 1]
  puts "Grid spacing changed to $GRID_SPACING"		    

  # Now change the grid in this cell if it is already on

  global SUE_${cur_s}

  if {[info exists SUE_${cur_s}(grid)] != 1} {
    return
  }

  if {[set SUE_${cur_s}(grid)] != 0} {
    $cur_c delete grid
    make_grid $GRID_SPACING
  }
}


proc make_grid {{spacing 5}} {

  global cur_c COLORS scale

  $cur_c delete grid

  set bbox "0 0 0 0"

  set inc [expr ${spacing} * ${scale}]
  set bboxind0 [lindex $bbox 0]
  set bboxind1 [lindex $bbox 1]
  set bboxind2 [lindex $bbox 2]
  set bboxind3 [lindex $bbox 3]
  set xcenter [expr int((${bboxind2}+${bboxind0})/(2*${scale}))*${scale}]
  set ycenter [expr int((${bboxind3}+${bboxind1})/(2*${scale}))*${scale}]

  set x1 [expr ${xcenter} - 1000 * ${scale}]
  set x2 [expr ${xcenter} + 1000 * ${scale}]
  set y1 [expr ${ycenter} - 1000 * ${scale}]
  set y2 [expr ${ycenter} + 1000 * ${scale}]

  for {set x $x1} {$x < $x2} {incr x $inc} {
      $cur_c create line $x $y1 $x $y2 -dash {.} -dashoffset 10 -fill $COLORS(grid) -tags grid
  }

  for {set y $y1} {$y < $y2} {incr y $inc} {
      $cur_c create line $x1 $y $x2 $y -dash {.} -dashoffset 10 -fill $COLORS(grid) -tags grid
  }

  $cur_c lower grid

  ready
}
