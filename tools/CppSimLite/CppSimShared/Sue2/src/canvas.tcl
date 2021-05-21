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

#############################################################################
# Global varibles associated with canvases:
#
# cur_s		name of the current schematic.  For icons it's ICON_<name>
# cur_c 	name of the current canvas: .$WIN.c_$cur_c
# scale		current canvas scale which is also the grid step
#############################################################################

# Make a new canvas for a schematic or icon

proc make_canvas {win schematic type} {

  global cur_c cur_s scale COLORS MODIFY_ICON HIERARCHY

  # default drawing is 10 points for minimum size
  set scale 10

  set cur_s $schematic

  if {[info exists cur_c] == 0} {
     set cur_c $win.editor
     # Create the canvas for this schematic
     canvas $cur_c -scrollregion {-10000 -10000 10000 10000} \
         -width 15c -height 10c \
         -xscrollcommand "$win.hscroll set" \
         -yscrollcommand "$win.vscroll set" \
         -closeenough 4 -background $COLORS(background) \
         -confine false

     pack $cur_c -expand yes -fill both

     set HIERARCHY ""

  }

  upvar #0 SUE_$cur_s data

  set data(canvas) $cur_c
  set data(scale) $scale
  set data(type) $type

  set data(modified) ""
  set data(modify_icon) ""

  set data(locked) ""
  set data(written) ""

  set data(hscroll) 0
  set data(vscroll) 0

  # all icons are up to date as of now
  set data(icon_index) $MODIFY_ICON(_index)
  set data(netlist_icon_index) $MODIFY_ICON(_index)

  add_schematic_to_listbox $schematic

}

proc setup_schematic_colors {} {
      global cur_c COLORS

      $cur_c itemconfigure icon_pinbox -fill $COLORS(icon_pinbox)
      $cur_c itemconfigure wire||dot -fill $COLORS(wire)
      $cur_c itemconfigure draw_item -fill $COLORS(draw_item)
      $cur_c itemconfigure icon -fill $COLORS(icon)
      $cur_c itemconfigure icon_text -fill $COLORS(icon_text)
      $cur_c itemconfigure icon_text_param -fill $COLORS(icon_text_param)
      $cur_c itemconfigure term -fill $COLORS(icon_pinbox)
      $cur_c itemconfigure icon&&arc -outline $COLORS(icon)
      $cur_c raise term

      show_connects "" fast
}

proc setup_icon_colors {} {
      global cur_c COLORS

      $cur_c itemconfigure icon_pinbox -fill $COLORS(icon_pinbox)
      $cur_c itemconfigure wire||dot -fill $COLORS(wire)
      $cur_c itemconfigure draw_item -fill $COLORS(icon)
      $cur_c itemconfigure icon_text_param -fill $COLORS(icon_text_param)
      $cur_c itemconfigure draw_item&&scaletext -fill $COLORS(icon_text)
      $cur_c itemconfigure arc -outline $COLORS(icon)
      $cur_c itemconfigure icon&&!scaletext -fill $COLORS(icon)
      $cur_c itemconfigure icon_text -fill $COLORS(icon_text)
      $cur_c itemconfigure term -fill $COLORS(icon_pinbox)
      $cur_c raise term

      show_connects "" fast
}

proc setup_print_colors {} {
      global cur_c COLORS

      $cur_c itemconfigure icon_pinbox -fill black
      $cur_c itemconfigure draw_item -fill black
      $cur_c itemconfigure icon -fill black
      $cur_c itemconfigure icon_text -fill red
      $cur_c itemconfigure icon_text_param -fill red
      $cur_c itemconfigure term -fill black
      $cur_c itemconfigure icon&&arc -outline black
      $cur_c itemconfigure wire||dot -fill black

#      $cur_c raise term
      $cur_c raise icon_text
      $cur_c raise icon_text_param

      show_connects "" fast
}

# pack the current canvas into the given window

proc pack_canvas {win} {

  global cur_c cur_s scale WIN WIN_DATA GRID_SPACING COLORS
  upvar #0 SUE_$cur_s data

#    $cur_c xview moveto 0.5
#    $cur_c yview moveto 0.5
#    $win.hscroll set [lindex [$cur_c xview] 0] [lindex [$cur_c xview] 1]
#    $win.vscroll set [lindex [$cur_c yview] 0] [lindex [$cur_c yview] 1]

    if { $data(type) == "I" } {
        setup_icon_colors
    } else {
        setup_schematic_colors
    }

    show_connects "" fast

    display_title $win

    display_selection
}


# unpack the current canvas from the given window

proc unpack_canvas {} {
  
  global cur_c cur_s scale WIN WIN_DATA SUE
  global STATE COPY_SCHEMATIC_FLAG HIERARCHY
  upvar #0 SUE_$cur_s data

  # If there is a cancellation command, execute it now
  eval $WIN_DATA($WIN,abort_cmd)
  set WIN_DATA($WIN,abort_cmd) ""

if {[info exists STATE($cur_s,exists)] != 1} {

      # if the canvas is modified, ask the user for confirmation before deleting
      if {$data(modified) == "M" && [info exists COPY_SCHEMATIC_FLAG] != 1} {
	  if {[string compare $cur_s "no_name"] == 0} {
              copy_schematic

              set HIERARCHY ""

              set current_id [lindex [$cur_c find withtag selected] 0]
              set id [find_origin $current_id]

              upvar #0 SUE_$cur_s data
	  } else {
             set button [tk_dialog_new .modify_exit "Cell Modified" \
		    "Save changes?" \
			     "" 0 {yes} {no}]

             if {$button == 0} {
               # user hit the yes key

                write_file $cur_s
                remember_modified
	     } else {
                not_modified $cur_s
	     } 
	  }
      } elseif {[info exists COPY_SCHEMATIC_FLAG] == 1} {
            not_modified $cur_s
      }

      set STATE($cur_s,exists) 1
      set STATE($cur_s,scale) $scale
      set STATE($cur_s,hscroll) [lindex [$cur_c xview] 0]
      set STATE($cur_s,vscroll) [lindex [$cur_c yview] 0]

      unset SUE($cur_s)
      unset data
  }


  ready

#  pack forget $cur_c
  $cur_c delete all

  leave_canvas
}


# if icon has been edited, remember that it has so we can propagate
# into schematics that contains an instance of this icon

proc remember_modified {} {
  
  global cur_c cur_s MODIFY_ICON
  upvar #0 SUE_$cur_s data

  if {$data(type) == "I"} {
    if {$data(modify_icon) == "M"} {
      set schematic [get_rootname $cur_s]

      # remakes procedure for this icon
      write_icon NULL
      # get rid of old compiled versions
      catch {rename _MAKE_$schematic ""}
      catch {rename _MAKE90_$schematic ""}

      # now make the new icon to recompile and setup data structures
      set bogus_id [make $schematic]
      # delete bogus icon and data structures
      $cur_c delete inst$bogus_id
      upvar #0 ${cur_s}_inst$bogus_id i_data
#      unset i_data

      # store this icon so it will propagate 
      set MODIFY_ICON([incr MODIFY_ICON(_index)]) $schematic

      # unmodify this icon - for this task only
      set data(modify_icon) ""
    }
  }
}


# when entering a canvas set up the current canvas, schematic, scale
# global variables and set the focus to it.

proc enter_canvas {schematic} {

  global cur_c cur_s scale
  upvar #0 SUE_$schematic data

  set cur_c $data(canvas)
  set cur_s $schematic
  set scale $data(scale)

  # we are going into this canvas, need to propagate and modified icons now.
  propagate_modified_icons

  set cur_focus_win [focus]
  set win_focus_check [string compare -length 5 $cur_focus_win ".win1"]
  if {$win_focus_check == 0} {
      focus $cur_c
  }
}


# check to see if we need to propagate any modified icons into the canvas
# of this schematic and if needed, do so.

proc propagate_modified_icons {} {

  global cur_s cur_c MODIFY_ICON
  upvar #0 SUE_$cur_s data

  set updated("_dummy") ""
  if {$MODIFY_ICON(_index) > $data(icon_index)} {
    # propagate through all icons in MODIFY_ICON array
    for {set i $data(icon_index)} {$i < $MODIFY_ICON(_index)} {} {
      set icon $MODIFY_ICON([incr i])

      if {[info exists updated($icon)]} {
	# already propagated
	continue
      }
	  
      # remember so we don't have to repropagate
      set updated($icon) 1
    
      # don't propagate back into icon
      if {$cur_s == "ICON_$icon"} {
	continue
      }

      foreach id [$cur_c find withtag icon_$icon] {
	remake $id $id dont_modify
      }
    }
    
    # update this canvas to know which icons have been propagated
    set data(icon_index) $MODIFY_ICON(_index)
  }
}


# when leaving a canvas that contains a modified icon, we need to update
# the world

proc leave_canvas {} {

  global cur_c cur_s

  # unhilites item
  item_enter_or_leave "leave"
}


# called every time you enter a mode like text mode or duplication mode

proc enter_mode {mode {abort_cmd ""}} {

  global WIN WIN_DATA
  global DISABLE_CANVAS_EVENT

  # If there is a cancellation command, execute it
  eval $WIN_DATA($WIN,abort_cmd)

  # stop general canvas binding events
  set DISABLE_CANVAS_EVENT 1

  # set up the abort command for this mode
  set WIN_DATA($WIN,abort_cmd) $abort_cmd

  # cleans up the screen
  item_enter_or_leave "leave"

  # Save and clear out all default canvas bindings
  save_bindings
  clear_bindings
}

# called every time you leave a mode

proc leave_mode {{mode ""}} {

  global WIN WIN_DATA DISABLE_CANVAS_EVENT

  # since we are leaving normally we can cancel the abort_cmd
  set WIN_DATA($WIN,abort_cmd) ""

  # reset the display message
  set WIN_DATA($WIN,display_msg) ""

  # restore the default canvas bindings
  clear_bindings
  restore_bindings

  # enable general canvas events
  set DISABLE_CANVAS_EVENT 0
}
