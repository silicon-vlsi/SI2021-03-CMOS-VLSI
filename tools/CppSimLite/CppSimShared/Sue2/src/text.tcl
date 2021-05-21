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

# Procedures for adding and modifying text.

# Note that we don't need to restrict the user to grids for text but
# we do anyways

proc setup_text_mode {} {

  global cur_c WIN WIN_DATA SNAP_XY

  enter_mode text abort_text_mode

  # make sure nothing has this tag leftover
  $cur_c dtag mod_text

  set WIN_DATA($WIN,display_msg) "Button-1 begins text, Escape aborts"

  bind $cur_c <Button-1> "add_text $SNAP_XY"
  bind $cur_c <Escape> "abort_text_mode"
}


proc setup_modify_text_mode {} {

  global cur_c UNDO

  enter_mode text abort_modify_text_mode

  # make sure nothing has this tag leftover
  $cur_c dtag mod_text

  set id [$cur_c find withtag current]

  # this new text will get a special tag for now
  $cur_c addtag mod_text withtag $id

  # on ctrl-c, revert string to original
  set UNDO [lindex [$cur_c itemconfigure $id -text] 4]

  bind $cur_c <Escape> "abort_modify_text_mode"

  modify_text
}


# Makes text.  Used by outside world

proc make_text {args} {
  call_by_keyword $args {{origin {0 0}} {text {}} {size standard} {anchor w}}

  global cur_c cur_s scale FONT COLORS

  set id [$cur_c create text [lindex $origin 0] [lindex $origin 1] \
	      -text $text -font $FONT($size,$scale) -fill $COLORS(icon_text) \
	      -tags "draw_item size_$size scaletext" -anchor $anchor]

  return $id
}


# Adds text if nothing is there or modifies existing text

proc add_text {x y} {

  global cur_c UNDO

  set id [make_text -origin "$x $y"]

  # this new text will get a special tag for now
  $cur_c addtag mod_text withtag $id

  $cur_c icursor $id 0

  # Used to determine if modified
  set UNDO ""

  modify_text
}

proc mouse_text_cursor_move {x y} {
    global cur_c TEXT_CHIT

    set TEXT_CHIT 1

    set xval [expr $x + 10000]
    set yval [expr $y + 10000]
    $cur_c icursor mod_text @$xval,$yval
    $cur_c focus mod_text
    focus $cur_c
    $cur_c select from mod_text @$xval,$yval
}

proc modify_text {} {

  global cur_c cur_s scale
  global WIN WIN_DATA

  set WIN_DATA($WIN,display_msg) \
      "Left mouse button moves cursor, Tab for newline, Return ends, Shift/Control Button-x changes size/anchor, Escape aborts"

  # reset Button-1
  bind $cur_c <Button-1> "possibly_end_text_mode"

  # shift button-x changes the text size
  bind $cur_c <Shift-Button-1> "change_text_size mod_text small"
  bind $cur_c <Shift-Button-2> "change_text_size mod_text standard"
  bind $cur_c <Shift-Button-3> "change_text_size mod_text large"

  # control button-x changes the text anchor position
  bind $cur_c <Control-Button-1> "change_text_anchor mod_text w"
  bind $cur_c <Control-Button-2> "change_text_anchor mod_text c"
  bind $cur_c <Control-Button-3> "change_text_anchor mod_text e"
  
  $cur_c focus mod_text
  draw_anchor_marker mod_text
  
  $cur_c bind mod_text <Return> "end_text_mode"
  # insert a new line for tab
#  $cur_c bind mod_text <Tab> [list $cur_c insert mod_text insert \n]
#  $cur_c bind mod_text <Button-1> "mouse_text_cursor_move \[$cur_c canvasx %x\] \[$cur_c canvasy %y\]"

  $cur_c bind mod_text <Button-1> "textB1Press \[$cur_c canvasx %x\] \[$cur_c canvasy %y\]"
  $cur_c bind mod_text <B1-Motion> "textB1Move \[$cur_c canvasx %x\] \[$cur_c canvasy %y\]"
  $cur_c bind mod_text <Shift-1> "$cur_c select adjust current @%x,%y"
  $cur_c bind mod_text <KeyPress> "textInsert %A"
  $cur_c bind mod_text <Tab> "textInsert \\n"
  $cur_c bind mod_text <BackSpace> "textBs"
  $cur_c bind mod_text <Delete> "textDel"
  $cur_c bind mod_text <Button-2> "textPaste \[$cur_c canvasx %x\] \[$cur_c canvasy %y\]" 



  # some EMACS-like bindings
  $cur_c bind mod_text <Left> \
      "$cur_c icursor mod_text \[expr \[$cur_c index mod_text insert\] - 1\]"
  $cur_c bind mod_text <Right> \
      "$cur_c icursor mod_text \[expr \[$cur_c index mod_text insert\] + 1\]"
  $cur_c bind mod_text <Control-a> "$cur_c icursor mod_text \[bol\]"
  $cur_c bind mod_text <Control-e> "$cur_c icursor mod_text \[eol\]"
  $cur_c bind mod_text <Control-d> "$cur_c dchars mod_text insert"
  $cur_c bind mod_text <Control-k> "$cur_c dchars mod_text insert \[eol\]"
  $cur_c bind mod_text <Down> "next_line forward"
  $cur_c bind mod_text <Up> "next_line backward"

}

proc textInsert {string} {

    global cur_c
    if {$string == ""} {
	return
    }
    catch {$cur_c dchars mod_text sel.first sel.last}
    $cur_c insert mod_text insert $string
}

proc textPaste {x y} {

    global cur_c

    set xval [expr $x + 10000]
    set yval [expr $y + 10000]

    catch {
	$cur_c insert mod_text @$xval,$yval [selection get]
    }
    textB1Press $x $y
}

proc textB1Press {x y} {
    global cur_c TEXT_CHIT

    set TEXT_CHIT 1

    set xval [expr $x + 10000]
    set yval [expr $y + 10000]

    $cur_c select clear

    $cur_c icursor current @$xval,$yval
    $cur_c focus current
    focus $cur_c
    $cur_c select from current @$xval,$yval
}

proc textB1Move {x y} {
    global cur_c

    set xval [expr $x + 10000]
    set yval [expr $y + 10000]

    $cur_c select to current @$xval,$yval
}

proc textBs {} {
    global cur_c

    if {![catch {$cur_c dchars mod_text sel.first sel.last}]} {
	return
    }
    set char [expr {[$cur_c index mod_text insert] - 1}]
    if {$char >= 0} {$cur_c dchar mod_text $char}
}

proc textDel {} {
    global cur_c

    if {![catch {$cur_c dchars mod_text sel.first sel.last}]} {
	return
    }
    $cur_c dchars mod_text insert
}

proc change_text_size {id size} {

  global cur_c scale FONT

  # first update the tags
  set tags [$cur_c gettags $id]
  set old_size [lindex $tags [lsearch $tags size_*]]
  $cur_c dtag mod_text $old_size
  $cur_c addtag size_$size withtag $id 

  if {$old_size != $size} {
    # flag that this canvas has been modified
    is_modified

    # now actually change the size
    $cur_c itemconfigure $id -font $FONT($size,$scale)
  }
}


proc change_text_anchor {id anchor} {

  global cur_c scale FONT

  set old_anchor [string index [lindex [$cur_c itemconfigure $id -anchor] 4] 0]
  if {$old_anchor != $anchor} {
    # flag that this canvas has been modified
    is_modified

    set bbox [$cur_c bbox $id]
    set width [expr [lindex $bbox 2] - [lindex $bbox 0]]

    switch $old_anchor {
      "w" { set dx 0 }
      "e" { set dx [expr 0 - $width] }
      "c" { set dx [expr 0 - $width/2.0] }
    }

    switch $anchor {
      "w" { set dx [expr $dx + 0] }
      "e" { set dx [expr $dx + $width] }
      "c" { set dx [expr $dx + $width/2.0] }
    }

    # round to the nearest grid
    set dx [expr round(1.0 * $dx / $scale) * $scale]

    $cur_c move $id $dx 0

    # update the anchor
    $cur_c itemconfigure $id -anchor $anchor

    # update the anchor marker
    $cur_c delete anchor
    draw_anchor_marker mod_text
  }
}


proc show_anchors {} {

  global cur_c

  # waste any old anchors
  $cur_c delete anchor

  foreach id [$cur_c find withtag scaletext] {
    draw_anchor_marker $id
  }
}


proc draw_anchor_marker {id} {

  global cur_c scale COLORS

  set coords [$cur_c coords $id]
  set x [lindex $coords 0]
  set y [lindex $coords 1]
  set del [expr $scale/3.0]

  $cur_c create line $x $y [expr $x-$del] [expr $y-$del] [expr $x-$del] \
      [expr $y+$del] $x $y -tags "anchor tmp" -fill $COLORS(anchor) 
}


# returns the begining of the line index.  Complicated for multi-line text.

proc bol {} {

  global cur_c

  set cursor [$cur_c index mod_text insert]
  set text [lindex [$cur_c itemconfigure mod_text -text] 4]

  return [expr 1 + [string last "\n" [string range $text 0 [expr $cursor-1]]]]
}

# returns the end of the line index.  Complicated for multi-line text.

proc eol {} {

  global cur_c

  set cursor [$cur_c index mod_text insert]
  set text [lindex [$cur_c itemconfigure mod_text -text] 4]

  set next_lf [string first "\n" [string range $text $cursor end]]
  if {$next_lf == -1} {
    return end
  } else {
    return [expr $next_lf + $cursor]
  }
}

# moves the cursor forward or back a line.  Wraps.

proc next_line {dir} {

  global cur_c

  set cursor [$cur_c index mod_text insert]
  set text [lindex [$cur_c itemconfigure mod_text -text] 4]

  set prev_lf [string last "\n" [string range $text 0 [expr $cursor-1]]]

  if {$dir == "forward"} {
    set next_lf [string first "\n" [string range $text $cursor end]]
    if {$next_lf != -1} {
      incr next_lf $cursor
      set stop_lf [string first "\n" [string range $text [expr $next_lf+1] end]]
      if {$stop_lf == -1} {
	set stop_lf [string length $text]
      } else {
	incr stop_lf [expr $next_lf+1]
      }
    } else {
      set stop_lf [string first "\n" [string range $text 0 end]]
      if {$stop_lf == -1} {
	set stop_lf 100000
      }
    }
  } else {
    set next_lf [string last "\n" [string range $text 0 [expr $prev_lf-1]]]
    if {$next_lf != -1} {
      set stop_lf $prev_lf
    } else {
      if {$prev_lf == -1} {
	set next_lf [string last "\n" $text]
	set stop_lf 100000
      } else {
	set next_lf -1
	set stop_lf $prev_lf
      }
    }
  }
  $cur_c icursor mod_text [min [expr $next_lf + $cursor - $prev_lf] $stop_lf]
}


proc possibly_end_text_mode {} {

  global TEXT_CHIT

  if {[info exists TEXT_CHIT]} {
    unset TEXT_CHIT
    return
  }

  end_text_mode
}


proc abort_text_mode {} {

  global cur_c

  $cur_c delete mod_text
  $cur_c delete anchor

  end_text_mode
}

proc abort_modify_text_mode {} {

  global cur_c UNDO

  $cur_c itemconfigure mod_text -text $UNDO

  end_text_mode
}

proc end_text_mode {} {

  global cur_c UNDO

  $cur_c delete anchor
  $cur_c focus ""

  $cur_c select clear

  # throw away null strings
  set text [lindex [$cur_c itemconfigure mod_text -text] 4]
  if {$text == ""} {
    puts "Aborted text mode."
    $cur_c delete mod_text

  } else {
    select_id [lindex [$cur_c find withtag mod_text] 0]

    # get rid of special tag
    $cur_c dtag mod_text

    if {$text != $UNDO} {
      # flag that this canvas has been modified
      is_modified
    }
  }

  # clean up the bindings
  foreach binding [$cur_c bind mod_text] {
    $cur_c bind mod_text $binding ""
  }

  leave_mode text
}

