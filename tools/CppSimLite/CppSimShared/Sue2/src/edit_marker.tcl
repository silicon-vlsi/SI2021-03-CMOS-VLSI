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

# Edit markers are rectangles that are drawn during the editing of
# the draw_items lines and arcs.  Inside these edit_markers are procs
# which tell how to resize the draw_items.

proc edit_draw_item {x y} {

  global cur_c DISABLE_CANVAS_EVENT TEXT_CHIT

  # stop general canvas binding events
  set DISABLE_CANVAS_EVENT 1

  set id [$cur_c find withtag current]
  switch [$cur_c type $id] {

    "text" {
      # if it's text send if off
      if {![is_tagged $id selected]} {
	select_id $id
      }

      setup_modify_text_mode
      mouse_text_cursor_move [$cur_c canvasx $x] [$cur_c canvasy $y]
      
      set TEXT_CHIT 1

      return
    }

    "line" {
      create_line_edit_markers $id
    }

    "arc" {
      create_arc_edit_markers $id
    }

    default {
      puts "Unknown type [$cur_c type $id]"
    }
  }
}




# These move procedures copy pretty directly from the icon routines
##############################################################################
# Sets up a move for a drawing item edit marker
##############################################################################
proc marker_press {x y} {

  global cur_c
  global DISABLE_CANVAS_EVENT SAVE

  # stop general canvas binding events
  set DISABLE_CANVAS_EVENT 1
 
  set SAVE(x) $x
  set SAVE(y) $y

  set SAVE(undo,x) $x
  set SAVE(undo,y) $y
 
  # Tag the edit marker so that we can easily find it later
  $cur_c addtag selected_marker withtag current

}
 
##############################################################################
# Handles mouse drag on a drawing item edit marker
##############################################################################
proc marker_drag {x y} {

  global cur_c
  global SAVE
 
  $cur_c move selected_marker [expr $x - $SAVE(x)] [expr $y - $SAVE(y)]

  # get the procedure out of the edit_marker tags
  set tags [$cur_c gettags selected_marker]
  set proc [lrange [lindex $tags [lsearch $tags proc*]] 1 end]

  # execute it
  eval $proc

  set SAVE(x) $x
  set SAVE(y) $y
}
 
##############################################################################
# Handles button 1 release on a drawing item edit marker
##############################################################################
proc marker_release {} {

  global cur_c
  global SAVE

  # unselect the edit marker
  $cur_c dtag selected_marker

  if {[nearby $SAVE(x) $SAVE(y) $SAVE(undo,x) $SAVE(undo,y)]} {

    # nothing happened
    return
  }

  # flag that this canvas has been modified
  is_modified
}


proc create_edit_mark {x y} {

  global cur_c scale COLORS

  set del [expr $scale/3.0]
  $cur_c create rectangle [expr $x - $del] [expr $y - $del] \
                          [expr $x + $del] [expr $y + $del] \
      -tags "edit_marker tmp selected" -outline "" -fill $COLORS(selected)
}








