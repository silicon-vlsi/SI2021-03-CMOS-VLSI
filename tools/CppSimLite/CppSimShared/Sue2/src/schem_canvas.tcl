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
# Utility procedures for highlighting the item under the pointer:
##############################################################################

##############################################################################
# item_enter handles highlighting what's under the cursor.
# 
# We have selected to use only those items which highlite the same way
##############################################################################
##############################################################################
# unhighlites what was under cursor
##############################################################################

proc item_enter_or_leave {enter_or_leave_mode} {

  global cur_c cur_s COLORS 
  global LAST_TAG LAST_LAST_TAG LAST_LAST_LAST_TAG

  set id [$cur_c find withtag current]

  upvar #0 SUE_$cur_s data

  set tag [find_origin_tag $id]

  if {$enter_or_leave_mode == "enter"} {
      if {[info exists LAST_LAST_LAST_TAG]} {

	  if {$LAST_LAST_TAG == $tag && $LAST_LAST_LAST_TAG == $LAST_TAG && $LAST_TAG != $tag} {
              $cur_c bind $tag <Any-Leave> {break}
              return
	  } else {
              $cur_c bind $tag <Any-Leave> {item_enter_or_leave "leave"}
	  }
       } else {
          $cur_c bind $tag <Any-Leave> {item_enter_or_leave "leave"}
       }
       if {[info exists LAST_LAST_TAG]} {
            set LAST_LAST_LAST_TAG $LAST_LAST_TAG
       }
       if {[info exists LAST_TAG]} {
            set LAST_LAST_TAG $LAST_TAG
       }
       set LAST_TAG $tag
     }

  if { $data(type) != "I" } {

     set cur_c_tags [$cur_c gettags $id]
     set check_tag [lsearch $cur_c_tags grid]
     if {$check_tag == -1} {
         set tag_grid 0
     } else {
         set tag_grid 1
     }
#     set tag_grid [is_tagged $id grid]

     if {$tag_grid == 1 && $enter_or_leave_mode == "leave"} {
       return
     }

     if {$tag != ""} {
#        set tag_selected [is_tagged $id selected]
        set cur_c_tags [$cur_c gettags $id]
        set check_tag [lsearch $cur_c_tags selected]

        if {$check_tag != -1} {
            $cur_c itemconfigure $tag -fill $COLORS(selected)
            $cur_c itemconfigure $tag&&arc -outline $COLORS(selected)
        } else {
	    if {$enter_or_leave_mode == "enter"} {
               $cur_c itemconfigure $tag -fill $COLORS(active)
               $cur_c itemconfigure $tag&&arc -outline $COLORS(active)
	    } else {
               set wire_check [is_wire $id]
               set dot_check [is_dot $id]
               set draw_check [is_draw_item $id]
               set term_check [is_term_open $id]
	       if {$wire_check == 1 || $dot_check == 1} {
                  $cur_c itemconfigure $tag -fill $COLORS(wire)
	       } elseif {$draw_check == 1} {
                  $cur_c itemconfigure $tag -fill $COLORS(draw_item)
	       } elseif {$term_check == 1} {
               $cur_c itemconfigure $tag -fill $COLORS(icon_pinbox)
	       } else {
               $cur_c itemconfigure $tag&&icon -fill $COLORS(icon)
               $cur_c itemconfigure $tag&&icon_text -fill $COLORS(icon_text)
               $cur_c itemconfigure $tag&&icon_text_param -fill $COLORS(icon_text_param)
               $cur_c itemconfigure $tag&&term -fill $COLORS(icon_pinbox)
               $cur_c itemconfigure $tag&&arc -outline $COLORS(icon)
               $cur_c raise $tag&&term
	       }
	    }
        }
     }
  } else {
      foreach ids $id {
        set tag [$cur_c gettags $ids]

        if {$tag != ""} {
           set cur_c_tags [$cur_c gettags $id]
           set check_tag [lsearch $cur_c_tags selected]
#           set tag_selected [is_tagged $ids selected]
           set arc_check [lsearch $tag arc]
           set grid_check [lsearch $tag grid]

	   if {$enter_or_leave_mode == "enter"} {
              if {$check_tag != -1} {
                 if {$arc_check != -1 } {
                    $cur_c itemconfigure $ids -outline $COLORS(selected)
                 } elseif {$grid_check == -1} {
                    $cur_c itemconfigure $ids -fill $COLORS(selected)
	         }
	      } else {
                 if {$arc_check != -1 } {
                    $cur_c itemconfigure $ids -outline $COLORS(active)
                 } elseif {$grid_check == -1} {
                    $cur_c itemconfigure $ids -fill $COLORS(active)
	         }
	      }
	   } else {
              set text_check [lsearch $tag scaletext]
              set draw_check [lsearch $tag draw_item]
              if {$check_tag != -1} {
                 if {$arc_check != -1 } {
                    $cur_c itemconfigure $ids -outline $COLORS(selected)
	         } elseif {$grid_check == -1} {
                    $cur_c itemconfigure $ids -fill $COLORS(selected)
	         }
	      } else {
                 if {$arc_check != -1 } {
                    $cur_c itemconfigure $ids -outline $COLORS(icon)
                 } elseif {$text_check != -1 } {
                    if {$draw_check != -1} {
                       $cur_c itemconfigure $ids -fill $COLORS(icon_text)
		    } else {
                       $cur_c itemconfigure $ids -fill $COLORS(icon_text_param)
		    }
                 } elseif {$grid_check == -1} {
		    $cur_c itemconfigure $ids -fill $COLORS(icon)
	         }
	      }
	   }
	}
     }
  }
}


proc is_wire {id} {

  global cur_c

  set search_result [lsearch [$cur_c gettags $id] wire]
  if {$search_result == -1} {
    return 0
  } else {
    return 1
  } 
}

proc is_term_open {id} {

  global cur_c

  set search_result [lsearch [$cur_c gettags $id] open]
  if {$search_result == -1} {
     return 0
  } else {
     return 1
  }
}

proc is_dot {id} {

  global cur_c

  set search_result [lsearch [$cur_c gettags $id] dot]
  if {$search_result == -1} {
     return 0
  } else {
     return 1
  }
}

proc is_draw_item {id} {

  global cur_c

  set search_result [lsearch [$cur_c gettags $id] draw_item]
  if {$search_result == -1} {
     return 0
  } else {
     return 1
  }
}


# unhighlites selected stuff unless it's current

proc unhighlite_selected {} {

  global cur_c COLORS

  set current_id [$cur_c find withtag current]
  set tag [find_origin_tag $current_id]

  foreach id [$cur_c find withtag selected] {
    set tag_check [is_tagged $id $tag]
    if {$tag_check == 1} {
      $cur_c itemconfigure $id -fill $COLORS(selected,active)

    } else {
      $cur_c itemconfigure $id -fill $COLORS(selected)
    } 
  }
}


##############################################################################
# Utility procedures to aid in entering/leaving modified states where
# the canvas bindings are changed so that the user can do something like
# create a wire or a graphical thingy
##############################################################################

##############################################################################
# save_bindings saves out the mouse and keyboard bindings for the
# current schematic. It is used when a new set of temporary bindings is
# going to replace the default bindings so that the user can do something
# useful like copy stuff, or draw a wire, or draw line, rects, oval, etc.
# It works in conjunction with restore_bindings which restores the
# mouse and keyboard bindings
##############################################################################
proc save_bindings {} {

  global cur_c WIN
  global ${WIN}_bindings
  global TAGS_TO_SAVE ;# global list of canvas item tags which should be saved

  # first save general canvas bindings
  set bind_list [bind $cur_c]
  foreach binding $bind_list {
    set ${WIN}_bindings($binding) [bind $cur_c $binding]
  }

  # now save away tag specifi bindings
  foreach tag $TAGS_TO_SAVE {

    # save away all canvas bindings for this tag
    set bind_list [$cur_c bind $tag]
    foreach binding $bind_list {
      set ${WIN}_bindings($tag,$binding) [$cur_c bind $tag $binding]
    }
  }
}


##############################################################################
# restore_bindings restores the default mouse and keyboard bindings
# for the current schematic, which were saved by save_bindings
##############################################################################
proc restore_bindings {} {

  global cur_c WIN
  global ${WIN}_bindings
  global TAGS_TO_SAVE ;# global list of canvas item tags which should be saved

  # restore everying in the bindings array
  foreach binding [array names ${WIN}_bindings] {

    set binding [split $binding ,]

    if {[llength $binding] == 1} {
      # a generic binding
      bind $cur_c $binding [set ${WIN}_bindings($binding)]

    } else {
      # a tag specific bindings
      set tag [lindex $binding 0]
      set bind [lindex $binding 1]
      $cur_c bind $tag $bind [set ${WIN}_bindings($tag,$bind)]
    }
  }
}

##############################################################################
# clear_bindings removes all of the bindings associated with the
# canvas
##############################################################################
proc clear_bindings {} {

  global cur_c
  global TAGS_TO_SAVE ;# global list of canvas item tags which should be saved

  # first clear general canvas bindings
  set bind_list [bind $cur_c]
  foreach binding $bind_list {
    bind $cur_c $binding ""
  }

  # now clear tag specific bindings
  foreach tag $TAGS_TO_SAVE {

    # save away all canvas bindings for this tag
    set bind_list [$cur_c bind $tag]
    foreach binding $bind_list {
      $cur_c bind $tag $binding ""
    }
  }
}

##############################################################################
# info_bindings lists the bindings which are currently saved,
# cleared, restored by (save/clear/restore)_bindings
##############################################################################
proc info_bindings {} {

  global cur_c
  global WIN
  global TAGS_TO_SAVE ;# global list of canvas item tags which should be saved

  # first general canvas bindings
  set bind_list [bind $cur_c]
  foreach binding $bind_list {
    puts "$WIN $binding bound to: [bind $cur_c $binding]"
  }

  # now restore tag specific bindings
  foreach tag $TAGS_TO_SAVE {
    # save away all canvas bindings for this tag
    set bind_list [$cur_c bind $tag]
    foreach binding $bind_list {
      puts "$WIN $tag $binding bound to: [$cur_c bind $tag $binding]"
    }
  }
}


# this procedure will change the state of the current schematic to
# be modified.

proc is_modified {{unmodify ""}} {

  global cur_s MODIFY NETLIST_CACHE

  upvar #0 SUE_$cur_s data

  set data(modified) M
  set data(modify_icon) M

  change_listbox_prefix $cur_s "M "

  if {[is_icon $cur_s]} {
    set data(netlist_modify_icon) M
  } else {
    # forget cached netlist information if any on schematics
    catch {unset NETLIST_CACHE($cur_s)}
  }

  # is this used???  What does it do???
  if {$unmodify != ""} {
    if {[info exists MODIFY($cur_s)] == 1 || $MODIFY($cur_s) == ""} {
      set MODIFY($cur_s) [list $unmodify $MODIFY($cur_s)]
    
    } else {
      set MODIFY($cur_s) [list $unmodify]
    }
  }
  display_title
}


# this procedure will reset the modification of either a schematic or an icon

proc not_modified {name} {

  global SUE_$name

  change_listbox_prefix $name "  "

  set SUE_${name}(modified) ""

  display_title
}


# tries to undo the last command to the current schematic

proc undo_last {} {

  global cur_s MODIFY

  if {[info exists MODIFY($cur_s)] == 1 && $MODIFY($cur_s) != ""} {

    set unmodify [lindex $MODIFY($cur_s) 0] 
    set MODIFY($cur_s) [lrange $MODIFY($cur_s) 1 end]

    puts "Undoing..."
    $unmodify

    # now undefine the procedure
    rename $unmodify ""

    # undoing modifies the cell, too
    is_modified

    ready

  } else {
    puts "Nothing to undo"
  }
}


# renames the command called undo that was just created to undo the last
# modification and then remembers it for undo_last.
# Only remembers up to UNDO_LEVEL things per schematic.

proc save_undo {} {

  global cur_s MODIFY undo_index UNDO_LEVEL

  if {[info exists undo_index] != 1} {
    set undo_index 0
  }

  set new_undo_name SUE_UNDO_[incr undo_index]
  rename undo $new_undo_name

  if {[info exists MODIFY($cur_s)]} {
    set MODIFY($cur_s) "$new_undo_name $MODIFY($cur_s)"
  } else {
    set MODIFY($cur_s) $new_undo_name
  }

  if {[llength $MODIFY($cur_s)] > $UNDO_LEVEL} {
    # undefine the oldest undo proc and remove from undo list
    rename [lindex $MODIFY($cur_s) $UNDO_LEVEL] ""
    set MODIFY($cur_s) [lrange $MODIFY($cur_s) 0 [expr ${UNDO_LEVEL} - 1]]
  }
}
