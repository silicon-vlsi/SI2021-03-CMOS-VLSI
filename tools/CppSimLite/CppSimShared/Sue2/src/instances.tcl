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
# Procedures for making/remaking icons.  There are two display formats
# for icons: either as instances in schematics or as icons for editing.
#
# Note that for performance reasons, icon procedures aren't directly
# executed to display instances (~1000 primitive tcl instructions typically).  
# Instead, they are first compiled into two procedures _MAKE_$type and
# _MAKE90_$type which are then directly executed.  _MAKE90_ is created to
# display the 90 degree rotated version of the icon since canvas do not
# have any built in rotations.  From _MAKE_ and _MAKE90_ and using x and
# y scaling, all rotations/mirroring of the icons is possible.  Note that
# the compiled versions only require about 100 primitive tcl instructions
# to draw their icons typically.
#
# Text rotation will hopefully be fixed with a rotated font family.
##############################################################################

# XFORM is scaled down by 10 since all things are saved at scale 10

set XFORM(R0,x) 0.1
set XFORM(R0,y) 0.1
set XFORM(RX,x) -0.1
set XFORM(RX,y) 0.1
set XFORM(RY,x) 0.1
set XFORM(RY,y) -0.1
set XFORM(RXY,x) -0.1
set XFORM(RXY,y) -0.1
set XFORM(R90,x) -0.1
set XFORM(R90,y) 0.1
set XFORM(R90X,x) 0.1
set XFORM(R90X,y) 0.1
set XFORM(R90Y,x) -0.1
set XFORM(R90Y,y) -0.1
set XFORM(R270,x) 0.1
set XFORM(R270,y) -0.1

set XFORM(R0,start) ""
set XFORM(R0,extent) 1
set XFORM(RX,start) "180 - "
set XFORM(RX,extent) -1
set XFORM(RY,start) "0 - "
set XFORM(RY,extent) -1
set XFORM(RXY,start) "180 + "
set XFORM(RXY,extent) 1
set XFORM(R90,start) "270 + "
set XFORM(R90,extent) 1
set XFORM(R90X,start) "270 - "
set XFORM(R90X,extent) -1
set XFORM(R90Y,start) "90 - "
set XFORM(R90Y,extent) -1
set XFORM(R270,start) "90 + "
set XFORM(R270,extent) 1

set XFORM(R0) ""
set XFORM(RX) ""
set XFORM(RY) ""
set XFORM(RXY) ""
set XFORM(R90) 90
set XFORM(R90X) 90
set XFORM(R90Y) 90
set XFORM(R270) 90

set XFORM(MX,R0) RX
set XFORM(MX,RX) R0
set XFORM(MX,RY) RXY
set XFORM(MX,RXY) RY
set XFORM(MX,R90) R90X
set XFORM(MX,R90X) R90
set XFORM(MX,R90Y) R270
set XFORM(MX,R270) R90Y

set XFORM(MY,R0) RY
set XFORM(MY,RX) RXY
set XFORM(MY,RY) R0
set XFORM(MY,RXY) RX
set XFORM(MY,R90) R90Y
set XFORM(MY,R90X) R270
set XFORM(MY,R90Y) R90
set XFORM(MY,R270) R90X

set XFORM(ROTATE,R0) R90
set XFORM(ROTATE,R90) RXY
set XFORM(ROTATE,RXY) R270
set XFORM(ROTATE,R270) R0
set XFORM(ROTATE,RX) R90Y
set XFORM(ROTATE,RY) R90X
set XFORM(ROTATE,R90X) RX
set XFORM(ROTATE,R90Y) RY

# for changing the anchor points on text (not very good)

set XFORM(R0,w) w
set XFORM(R0,nw) nw
set XFORM(R0,e) e
set XFORM(R0,ne) ne
set XFORM(R0,center) center
set XFORM(R0,c) center
set XFORM(RX,w) e
set XFORM(RX,nw) ne
set XFORM(RX,e) w
set XFORM(RX,ne) nw
set XFORM(RX,center) center
set XFORM(RX,c) center
set XFORM(RY,w) w
set XFORM(RY,nw) nw
set XFORM(RY,e) e
set XFORM(RY,ne) ne
set XFORM(RY,center) center
set XFORM(RY,c) center
set XFORM(RXY,w) e
set XFORM(RXY,nw) ne
set XFORM(RXY,e) w
set XFORM(RXY,ne) nw
set XFORM(RXY,center) center
set XFORM(RXY,c) center
set XFORM(R90,w) center
set XFORM(R90,nw) center
set XFORM(R90,e) center
set XFORM(R90,ne) center
set XFORM(R90,center) center
set XFORM(R90,c) center
set XFORM(R270,w) center
set XFORM(R270,nw) center
set XFORM(R270,e) center
set XFORM(R270,ne) center
set XFORM(R270,center) center
set XFORM(R270,c) center
set XFORM(R90X,w) center
set XFORM(R90X,nw) center
set XFORM(R90X,e) center
set XFORM(R90X,ne) center
set XFORM(R90X,center) center
set XFORM(R90X,c) center
set XFORM(R90Y,w) center
set XFORM(R90Y,nw) center
set XFORM(R90Y,e) center
set XFORM(R90Y,ne) center
set XFORM(R90Y,center) center
set XFORM(R90Y,c) center

# For display properties

# ul = upper left
set XFORM(labelx,ul,R0) 0
set XFORM(labely,ul,R0) 1
set XFORM(labelx,ul,RX) 2
set XFORM(labely,ul,RX) 1
set XFORM(labelx,ul,RY) 0
set XFORM(labely,ul,RY) 3
set XFORM(labelx,ul,RXY) 2
set XFORM(labely,ul,RXY) 3
set XFORM(labelx,ul,R90) 2
set XFORM(labely,ul,R90) 1
set XFORM(labelx,ul,R270) 0
set XFORM(labely,ul,R270) 3
set XFORM(labelx,ul,R90X) 0
set XFORM(labely,ul,R90X) 1
set XFORM(labelx,ul,R90Y) 2
set XFORM(labely,ul,R90Y) 3

set XFORM(anchor,ul,R0) se
set XFORM(anchor,ul,RX) sw
set XFORM(anchor,ul,RY) ne
set XFORM(anchor,ul,RXY) nw
set XFORM(anchor,ul,R90) sw
set XFORM(anchor,ul,R270) ne
set XFORM(anchor,ul,R90X) se
set XFORM(anchor,ul,R90Y) nw

# ur = upper right
set XFORM(labelx,ur,R0) 2
set XFORM(labely,ur,R0) 1
set XFORM(labelx,ur,RX) 0
set XFORM(labely,ur,RX) 1
set XFORM(labelx,ur,RY) 2
set XFORM(labely,ur,RY) 3
set XFORM(labelx,ur,RXY) 0
set XFORM(labely,ur,RXY) 3
set XFORM(labelx,ur,R90) 2
set XFORM(labely,ur,R90) 3
set XFORM(labelx,ur,R270) 0
set XFORM(labely,ur,R270) 1
set XFORM(labelx,ur,R90X) 0
set XFORM(labely,ur,R90X) 3
set XFORM(labelx,ur,R90Y) 2
set XFORM(labely,ur,R90Y) 1

set XFORM(anchor,ur,R0) sw
set XFORM(anchor,ur,RX) se
set XFORM(anchor,ur,RY) nw
set XFORM(anchor,ur,RXY) ne
set XFORM(anchor,ur,R90) nw
set XFORM(anchor,ur,R270) se
set XFORM(anchor,ur,R90X) ne
set XFORM(anchor,ur,R90Y) sw

# someday, I'll figure out the terms for ll and lr.



# Entry point to display an instance of an icon.  If the icon has a procedure
# defined but that procedure hasn't yet been compiled yet into _MAKE_$type
# and _MAKE90_$type then it is first compiled and then displayed.

proc make {icon_type args} {

  global cur_c cur_s scale FONT XFORM DISPLAY_PROPS COLORS
  upvar #0 icon_$icon_type g_data

  # has the drawing procedure been made yet for this type?
  if {[info commands _MAKE_$icon_type] == ""} {
    # first define the drawing procedure
    global icon

    catch {unset icon}

    set icon(icon) instance
    set icon(tag) "icon inst\$id"
    set icon(type) $icon_type

    if {[catch [list ICON_$icon_type $args] msg]} {
      # we got a problem.  It's probably can't find icon.
      if {$msg == "invalid command name \"ICON_$icon_type\""} {
	puts "ERROR: Can't find ICON definition for \"$icon_type\""
      } else {
	puts "$msg in ICON \"$icon_type\""
      }
      return
    }

    lappend icon(func) "return \$id"

    eval proc _MAKE_$icon_type [list "orient $g_data(prop_names)"] \
                               [list [join $icon(func) "\n"]]

    lappend icon(func90) "return \$id"

    eval proc _MAKE90_$icon_type [list "orient $g_data(prop_names)"] \
                                 [list [join $icon(func90) "\n"]]

    unset icon
  }


  # now use it

  # get the keywords and go
  # Note that call_use_keyword is defining arbitrary variables.
  call_use_keyword $args $g_data(defaults)
  set id [eval _MAKE$XFORM($orient)_$icon_type $orient $g_data(arglist)]

  upvar #0 ${cur_s}_inst$id i_data

  # save the orientation
  set i_data(orient) $orient

  # scale, mirror it, and move it (it is already rotated)
  $cur_c scale inst$id 0 0 [expr $XFORM($orient,x)*$scale] [expr $XFORM($orient,y)*$scale]

  eval $cur_c move inst$id $origin



  # show display props at desired positions
  if {[info exists g_data(_primitive)] != 1} {
    set bbox ""
    foreach prop_pair $DISPLAY_PROPS {
      set prop [lindex $prop_pair 0]
      set pos [lindex $prop_pair 1]
      if {[info exists i_data(_$prop)] && $i_data(_$prop) != ""} {
	if {[lindex $prop_pair 2] == ""} {
	  set text $i_data(_$prop)
	} else {
	  set text "[lindex $prop_pair 2]$i_data(_$prop)"
	}
	if {$pos == "origin"} {
	  set coords [$cur_c coords $id]
	  $cur_c create text [lindex $coords 0] [lindex $coords 1] \
	      -anchor $XFORM($orient,c) -fill $COLORS(icon_text_param) \
	      -text $text -font $FONT(standard,$scale) \
	      -tags "inst$id icon size_standard scaletext icon_text_param"
	} else {
	  # only compute the bbox once and only if you have to
	  if {$bbox == ""} {
	    set bbox [$cur_c bbox inst$id]
	  }
# the following corresponds to instance name (I think)
	  $cur_c create text [lindex $bbox $XFORM(labelx,$pos,$orient)] \
	      [lindex $bbox $XFORM(labely,$pos,$orient)] \
	      -anchor $XFORM(anchor,$pos,$orient) -fill $COLORS(icon_text_param) \
	      -text "$text" -font $FONT(standard,$scale) \
	      -tags "inst$id icon size_standard scaletext icon_text_param"
	}
      }
    }
  }
  return $id
}

##############################################################################
# Makes the icon for editing
# No drawing procedure needs to be made since this is only called solely.
##############################################################################
proc icon_make {type args} {

  global cur_c cur_s scale FONT XFORM icon

  catch {unset icon}

  set icon(icon) icon
  set icon(tag) "draw_item"
  set icon(type) $type

  # set scale to 10 for terminals and call the icon
  set save_scale $scale
  set scale 10

  eval ICON_$type $args

  set scale $save_scale

  # the orientation of the icon is always R0
  set orient R0

  foreach line $icon(func) {
    eval $line
  }

  $cur_c scale all 0 0 [expr $scale/10.0] [expr $scale/10.0]

  unset icon
}


##############################################################################
# creates the anchor rectangle (i.e., the origin) for an instance.
##############################################################################

proc icon_setup {arglist defaults} {

  # do this in context of the calling procedure
  uplevel call_use_keyword $arglist [list $defaults]

  global icon COLORS

  set icon(func) ""

  if {$icon(icon) == "instance"} {
    upvar #0 icon_$icon(type) icon_type
    catch {unset icon_type}

    set icon_type(defaults) $defaults
    set icon_type(arglist) ""
    set icon_type(prop_names) ""
    set icon_type(creator) $icon(type)
    
    # if it's a generator, setup data structure for generator
    if {[info exists icon(generator)]} {
      set icon_type(generator) $icon(generator)
      set icon_type(gargs) $icon(gargs)
      set icon_type(gdefaults) $icon(gdefaults)
    }

    lappend icon(func) "global cur_c cur_s scale FONT XFORM env"
    lappend icon(func) "set id \[\$cur_c create line 0 0 0 0 -fill $COLORS(icon) -tags \"origin icon icon_$icon(type)\"\]"
    lappend icon(func) {$cur_c addtag inst$id withtag $id}
    lappend icon(func) "upvar #0 \${cur_s}_inst\$id i_data"
    lappend icon(func) "set i_data(type) $icon(type)"

    set icon(func90) $icon(func)
  }
}


##############################################################################
# creates a line for an icon
# args are a list of x,y pairs
##############################################################################
proc icon_line {args} {

  global icon COLORS

  lappend icon(func) "\$cur_c create line $args -tags \"$icon(tag)\" -fill $COLORS(icon)"
  lappend icon(func90) "\$cur_c create line [lreverse_sue2 $args] -tags \"$icon(tag)\" -fill $COLORS(icon)"
}

##############################################################################
# creates an arc for an icon.  Note, an oval is an arc with an extent 
# of 359 degrees or so (can't use 360).
##############################################################################
proc icon_arc {args} {

  global icon COLORS


  if {$icon(icon) == "instance"} {

    # need to munge the orientation into the start and the extent
    # can't use lreplace
    set pos [expr [lsearch $args "-extent"] - 1]
    if {$pos != -2} {
      set args "[lrange $args 0 [incr pos]] \[expr \$XFORM(\$orient,extent)*[lindex $args [incr pos]]\] [lrange $args [incr pos] end]"
    }

    set pos [expr [lsearch $args "-start"] - 1]
    if {$pos != -2} {
	set args "[lrange $args 0 [incr pos]] \[expr \$XFORM(\$orient,start)[lindex $args [incr pos]]\] [join [lrange $args [incr pos] end]]"
    }
  set args [join $args " "]
  }

  lappend icon(func) \
      "\$cur_c create arc $args -style arc -fill $COLORS(icon) -tags \"$icon(tag) arc\" -outline $COLORS(icon)"
  lappend icon(func90) \
      "\$cur_c create arc [lindex $args 1] [lindex $args 0] [lindex $args 3] [lindex $args 2] [join [lrange $args 4 end]] -style arc -fill $COLORS(icon) -tags \"$icon(tag) arc\" -outline $COLORS(icon)"

}


##############################################################################
# creates text for an icon
# can pass in an optional anchor position
##############################################################################
proc icon_text {origin text {size standard} {anchor w}} {

  global icon COLORS

#  if {[string compare $text \"\$name\"] == 0} {
#      set type $icon(type)
#      if {[string compare $type "input"] != 0 && \
#	  [string compare $type "output"] != 0 && \
#	  [string compare $type "inout"] != 0 && \
#	  [string compare $type "global"] != 0 && \
#	  [string compare $type "name_net"] != 0 && \
#	  [string compare $type "name_net_s"] != 0} {
#            regsub \" $text "" new_text
#            set text \"x$new_text
#     }
# }
  lappend icon(func) "\$cur_c create text $origin -text $text -font \$FONT($size,\$scale) -tags \"$icon(tag) size_$size scaletext icon_text_param\" -anchor \$XFORM(\$orient,$anchor) -fill $COLORS(icon_text_param)"
  lappend icon(func90) "\$cur_c create text [lindex $origin 1] [lindex $origin 0] -text $text -font \$FONT($size,\$scale) -tags \"$icon(tag) size_$size scaletext icon_text_param\" -anchor \$XFORM(\$orient,$anchor) -fill $COLORS(icon_text_param)"
}

##############################################################################
# creates an icon terminal (i.e. a small solid square)
##############################################################################
proc icon_term {args} {
  call_by_keyword $args {{origin {0 0}} {type inout} {name ""} \
			     {orient R0} {priority term}}
  global icon COLORS

  set x [lindex $origin 0]
  set y [lindex $origin 1]

  if {$icon(icon) != "instance"} {

    # in an icon, actually show the full icon, not just a rectangle.
    # pretend to be a schematic and save state
    copy_array new_icon icon

    make $type -name $name -origin "$x $y" -orient $orient

    # now restore state
    copy_array icon new_icon
    return
  }

  # assumes the scale is 10
  set del [expr 10.0/6]

  # need to quote all special chars
  regsub -all {\[|\]} $name \\\\& name

  lappend icon(func) \
      "\$cur_c create rectangle [expr $x - $del] [expr $y - $del] [expr $x + $del] [expr $y + $del] -tags \"$icon(tag) term \{name $priority $type $name\}\" -fill $COLORS(icon_pinbox) -outline {}"
  lappend icon(func90) \
      "\$cur_c create rectangle [expr $y - $del] [expr $x - $del] [expr $y + $del] [expr $x + $del] -tags \"$icon(tag) term \{name $priority $type $name\}\" -fill $COLORS(icon_pinbox) -outline {}"
}


##############################################################################
# create properties for the icon.  All text in icons are properties.
##############################################################################

proc icon_property {args} {
  call_by_keyword $args {{origin {0 0}} {type text} {name ""} {label ""} \
			     {text ""} {default ""} {anchor w} {size standard}}
  global icon scale env FONT COLORS

  if {$icon(icon) == "instance"} {
    # make an instance of the icon for insertion into a schematic

    # labels have precedence
    if {$label != ""} {
      # need to quote all special chars
      regsub -all {\[|\]|\"|\{|\}|\\} $label \\\\& label

      icon_text $origin \"$label\" $size $anchor
      return
    }

    # generic data for the icon goes here
    upvar #0 icon_$icon(type) g_data

    switch $type {
      "text" { 
	icon_text $origin \"$text\" $size $anchor
      }
      "user" { 
	# can't use lappend since it puts {} around \$$name
	set g_data(arglist) [concat $g_data(arglist) \$$name]
	lappend g_data(prop_names) $name
    
	if {$text == ""} {
	  set text "\$$name"
	}

	lappend icon(func) "set i_data(_$name) $text"
	lappend icon(func90) "set i_data(_$name) $text"

	set g_data(_$name) $text
	set g_data(_$name,default) [eval concat $default]
      }
      "fixed" {
	# must be a fixed property
	set g_data(_$name) $text
      }
      "comment" {
      }
      default {
	puts "Warning: Unknown type \"$type\", ignoring."
      }
    }

  } else {
    # show the icon for editing in its own happy window

    if {$label != ""} { 
      icon_text $origin \{$label\} $size $anchor
    } else {
      # these definitions only show up in the icon editing window

      # remove origin, size, and anchor since they are passed separately
      foreach keyword {origin size anchor} {
	set pos [lsearch $args "-$keyword"]
	if {$pos != -1} {
	  set args [lreplace $args $pos [expr $pos+1]]
	}
      }
      icon_text $origin \{$args\} $size $anchor
    }
  }
}


# a call to this procedure needs to be in any icon definition that
# wants to be a generator.

proc icon_generator {arglist defaults} {

  global icon

  # do this in context of the calling procedure
  uplevel call_use_keyword $arglist [list $defaults]

  # figure out generator name
  set name [string range [lindex [info level -1] 0] 5 end]

  # figure out the arg list
  set args ""
  foreach input $defaults {
    set variable [lindex $input 0]
    upvar 1 $variable value
    if {[lindex $input 1] != $value} {
      lappend args -$variable $value
    }
  }

  # remember generator info
  set icon(generator) $name
  set icon(gargs) $args
  set icon(gdefaults) $defaults
}


proc make_icon_origin {} {

  global cur_c cur_s scale COLORS

  set plus [expr $scale/2.0]
  set minus [expr 0.0 - $scale/2.0]

  $cur_c create line 0 0 0 $plus 0 $minus 0 0 $plus 0 $minus 0 -fill \
      $COLORS(icon) -tags "origin_icon draw_item icon_[get_rootname $cur_s]"
}


# flips or rotates selected depending on direction, either MX, MY, or ROTATE.
# Note that this will leave wires floating.  I never thought that dragging
# wires during this type of operation really saved any work.

proc transform_selected {dir} {

  global cur_c cur_s scale XFORM

  # produce a list of selected objects only including origins for icons
  set ids ""
  foreach id [$cur_c find withtag selected] {
    if {[is_tagged $id origin]} {
      lappend ids $id
      continue
    }
    # this will ignore icon pieces and icon origins
    if {[is_tagged $id origin_icon] || [is_tagged $id icon]} {
      continue
    }
    lappend ids $id
  }

  # if we are only transforming one object and its an origin, then we
  # only want to rotate/mirror it around its own origin.
  if {[llength $ids] == 1 && [is_tagged $ids origin]} {
    transform_icon $ids $dir
    ready
    return
  }

  # find group center for transformation
  set bbox [$cur_c bbox selected]
  if {$bbox == ""} {
    ready
    return
  }

  set xcenter \
      [expr round(([lindex $bbox 0]+[lindex $bbox 2])/(2.0*$scale))*$scale] 
  set ycenter \
      [expr round(([lindex $bbox 1]+[lindex $bbox 3])/(2.0*$scale))*$scale] 

  remove_connects selected

  # remember spots that need to be cleaned up later
  eval $cur_c addtag clean overlapping $bbox
  $cur_c dtag selected clean
  set clean_wires ""
  set clean_terms ""
  foreach id [$cur_c find withtag clean] {
    if {[is_tagged $id wire]} {
      lappend clean_wires $id
      continue
    }
    if {[is_tagged $id term]} {
      lappend clean_terms [center $id]
    }
  }
  $cur_c dtag clean

  # rotation is different than mirroring
  if {$dir == "ROTATE"} {
    $cur_c move selected [expr 0 - $xcenter] [expr 0 - $ycenter]

    foreach id $ids {
      set coords [$cur_c coords $id]
      set num_coords [llength $coords]
      set new_coords ""
      for {set i 0} {$i < $num_coords} {incr i 2} {
	lappend new_coords [expr 0 - [lindex $coords [expr $i + 1]]] \
	    [lindex $coords $i]
      }
      eval $cur_c coords $id $new_coords
    }

    $cur_c move selected $xcenter $ycenter

    foreach id $ids {
      if {[is_tagged $id origin]} {
	transform_icon $id $dir
      } elseif {[$cur_c type $id] == "arc"} {
	set start [eval expr $XFORM(R90,start) \
		       [lindex [$cur_c itemconfigure $id -start] 4]]
	set extent [expr $XFORM(R90,extent) * \
			[lindex [$cur_c itemconfigure $id -extent] 4]]
	$cur_c itemconfigure $id -start $start -extent $extent
      }
    }
  } else {
    # mirroring
    if {$dir == "MX"} {
      $cur_c scale selected $xcenter $ycenter -1.0 1.0
      set orient RX
    }
    if {$dir == "MY"} {
      $cur_c scale selected $xcenter $ycenter 1.0 -1.0
      set orient RY
    }

    foreach id $ids {
      if {[is_tagged $id origin]} {
	transform_icon $id $dir
      } elseif {[$cur_c type $id] == "arc"} {
	set start [eval expr $XFORM($orient,start) \
		       [lindex [$cur_c itemconfigure $id -start] 4]]
	set extent [expr $XFORM($orient,extent) * \
			[lindex [$cur_c itemconfigure $id -extent] 4]]
	$cur_c itemconfigure $id -start $start -extent $extent
      }
    }
  }

  if {[is_icon $cur_s]} {
    # don't show connects in an icon
    ready
    return
  }

  show_connects selected

  foreach point $clean_terms {
    eval show_connect_point $point clean
  }
  foreach id $clean_wires {
    show_connect_wire $id clean
  }

  # now clean up connect on wires/terms that could have been transformed 
  # into connection
  eval $cur_c addtag clean overlapping [$cur_c bbox selected]
  $cur_c dtag selected clean
  set clean_wires ""
  set clean_terms ""
  foreach id [$cur_c find withtag clean] {
    if {[is_tagged $id wire]} {
      lappend clean_wires $id
      continue
    }
    if {[is_tagged $id term]} {
      lappend clean_terms [center $id]
    }
  }
  $cur_c dtag clean

  foreach point $clean_terms {
    eval show_connect_point $point clean
  }
  foreach id $clean_wires {
    show_connect_wire $id clean
  }

  ready
}


# rotates/mirrors a given icon only

proc transform_icon {id dir} {

  global cur_s

  upvar #0 ${cur_s}_inst$id i_data
  set orient $i_data(orient)

  # set the new orientation
  global XFORM
  set i_data(orient) $XFORM($dir,$orient)

  # now remake the icon
  remake $id $id

  # flag that this canvas has been modified
  is_modified
}


# delete a id_old and then remake it using the information in
# the global data structure for id.  Thus can remake same or replace.

proc remake {id_old id {modify ""}} {

  global cur_c cur_s

  # get origin and orient of old id
  set coords [lrange [$cur_c coords $id_old] 0 1]
  upvar #0 ${cur_s}_inst$id_old i_data_old
  set orient $i_data_old(orient)

  upvar #0 ${cur_s}_inst$id i_data
  set type $i_data(type)
  upvar #0 icon_$type g_data

  # build up the args
  set args "-origin [list $coords] -orient $orient"
  foreach name $g_data(prop_names) {
    lappend args -$name [use_first i_data(_$name) g_data(_$name,default)]
  }

  # is this selected
  set selected [is_tagged $id_old selected]

  # remember term locations
  foreach term_id [get_intersect_tag inst$id_old term] {
    set term_coords([round_list [center $term_id]]) 1
  }

  # delete old icon and lose the old data structure
  $cur_c delete inst$id_old
  unset i_data_old

  # now make the new icon 
  set new_id [eval make $type $args]

  # possibly update connection info if terminal locations have changed
  # Note that this will break if two terminals in the same icon overlap
  foreach term_id [get_intersect_tag inst$new_id term] {
    set xy [round_list [center $term_id]]
    if {[info exists term_coords($xy)]} {
      unset term_coords($xy)
    } else {
      set term_coords([round_list [center $term_id]]) 1
    }
  }
  if {![is_icon $cur_s] && [info exists term_coords]} {
    foreach xy [array names term_coords] {
      eval show_connect_point $xy clean
    }
  }

  if {$modify == ""} {
    # flag that this canvas has been modified
    is_modified
  }

  # reselect it if it was selected
  if {$selected} {
    select_id $new_id add
    unhighlite_selected
  }

  return $new_id
}


# generates a new icon procedure from "generator" to be called "name"
# if one doesn't already exist.

proc generate {generator name args} {

  global cur_c cur_s icon

  # Just return if it already exists
  if {[info command ICON_$name] != ""} {
    return
  }

  add_auto_path $generator

  # extract out the name from the full path if given
  set genname [lindex [split_filename $generator] 1]

  set icon(icon) instance
  set icon(tag) "icon inst\$id"
  set icon(type) $name

  if {[catch {ICON_$genname ""} msg]} {
    # we got a problem.  It's probably can't find generator.
    if {$msg == "invalid command name \"ICON_$genname\""} {
      puts "ERROR: Can't find GENERATOR definition for \"$genname\" while making \"$name\""
    } else {
      puts "$msg in GENERATOR \"$genname\""
    }

    unset icon
    return
  }

  unset icon

  set bogus_id $msg

  # delete bogus icon and data structures
  $cur_c delete inst$bogus_id
  upvar #0 ${cur_s}_inst$bogus_id i_data
  catch {unset i_data}

  set comment "  icon_property -origin {-100 -200} -type comment -text \"Generated from $genname $args\""
  eval proc ICON_$name args \
      [list [join [list [info body ICON_$genname] $comment] \n]]

  # now make the new icon to compile and setup
  set id [eval make $name $args]
  if {$id == ""} {
    # generator errored out
    return 0
  }

  # delete the output
  $cur_c delete inst$id

  upvar #0 icon_$name g_data
  # this will insure that the generator name is correct
  set g_data(generator) $genname

  # need to add this to auto_path
  add_to_auto_index $name generators/ICON_$name

  make_icon_listbox
  return 1
}


# replaces an existing generator

proc regenerate {generator name args} {

  global cur_c MODIFY_ICON auto_index

  # save old generator args just in case we fail
  upvar #0 icon_$name g_data
  set old_args [use_first g_data(gargs)]

  # kill the old icon
  delete_schematic ICON_$name

  # waste the old one if there is one
  catch {rename ICON_$name ""}
  catch {unset g_data}

  # lose any compiled version that may be around
  catch {rename _MAKE_$name ""}
  catch {rename _MAKE90_$name ""}

  if {[eval generate $generator $name $args] == 0} {
    # generator errored out, recreate existing
    if {$old_args != ""} {
      eval generate $generator $name $old_args

    } else {
      # waste the old one if there is one
      catch {rename ICON_$name ""}
      upvar #0 icon_$name g_data
      catch {unset g_data}
    }

    return 0
  }

  # special case for changing the generator default arguments
  if {$generator == $name} {
    catch {rename SCHEMATIC_$name TMP_PROC}
    delete_schematic $name
    catch {rename TMP_PROC SCHEMATIC_$name}
  } else {
    delete_schematic $name
  }

  # store this icon so it will propagate 
  set MODIFY_ICON([incr MODIFY_ICON(_index)]) $name

  return 1
}


# replaces an instance with another instance in a schematic

proc setup_replace_instance {} {

  global cur_c
  global WIN WIN_DATA

  enter_mode replace abort_replace_mode

  # check to see if an instance is selected
  set id_old [lindex [get_intersect_tag selected origin] 0]
  if {$id_old == ""} {
    abort_replace_mode
    return
  }

  set WIN_DATA($WIN,display_msg) \
      "Button-1 on icon to replace selected with, Escape cancels"

  bind $cur_c <Any-Button-1> "replace_instance"
  bind $cur_c <Any-Escape> "abort_replace_mode"

  # binding icon items to highlight when you are over them
  $cur_c bind icon <Any-Enter> {item_enter_or_leave "enter"}
  $cur_c bind icon <Any-Leave> {item_enter_or_leave "leave"}
}

proc replace_instance {} {

  global cur_c cur_s DONT_REPLACE_PROPS

  set id_old [lindex [get_intersect_tag selected origin] 0]

  set id [find_origin [$cur_c find withtag current]]

  if {[is_tagged $id origin] != 1 || $id == "" || $id == $id_old} {
    abort_replace_mode
    return
  }

  upvar #0 ${cur_s}_inst$id_old i_data_old
  upvar #0 ${cur_s}_inst$id i_data

  # get the name and other save properties out of the old_icon
  # and put these properties temporarily into the id
  foreach prop $DONT_REPLACE_PROPS {
    if {[info exists i_data_old(_$prop)] && [info exists i_data(_$prop)]} {
      set save(_$prop) $i_data(_$prop)
      set i_data(_$prop) $i_data_old(_$prop)
    }
  }

  puts "Replacing $i_data_old(type) with $i_data(type)"

  remake $id_old $id

  # restore existing properties to id
  if {[info exists save]} {
    foreach prop [array names save] {
      set i_data($prop) $save($prop)
    }
  }

  leave_mode replace
}

proc abort_replace_mode {} {

  puts "Aborting replace mode."
  leave_mode replace
}


proc show_icon_term_names {} {

  global cur_c scale COLORS FONT

  foreach id [$cur_c find withtag selected] {
    if {[is_tagged $id term]} {
      # get the terminal name
      set tags [$cur_c gettags $id]
      set name_list [lindex $tags [lsearch $tags "name*"]]
      set name [lindex $name_list 3]

      set coords [$cur_c coords $id]
      set x [lindex $coords 0]
      set y [lindex $coords 1]

      $cur_c create text $x $y -tags "tmp scaletext size_standard" \
	  -fill $COLORS(anchor) -text $name -font $FONT(standard,$scale)
    }
  }
}
