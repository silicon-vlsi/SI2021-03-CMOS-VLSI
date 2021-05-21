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

#################################################################
# 
# Routines to write an icon procedure from a cell.
#
#################################################################

#################################################################
# 
# Top level routine to write an icon.  Assumes file is open
#
#################################################################
proc write_icon {check_string} {

  global cur_c cur_s scale
  global PROC

  set PROC ""

  # Loop through all instances in the canvas
  foreach id [$cur_c find withtag origin] {
    if {[is_tagged $id icon_input] || [is_tagged $id icon_output] || \
	    [is_tagged $id icon_inout]} {
      # good, it's a terminal

      # instance-specific data (like W=1.2u) are here
      upvar #0 ${cur_s}_inst$id i_data

      if {$check_string == "with_message"} {
         if {[string compare $i_data(_name) ""] == 0} {
           select_ids $id
           set button [tk_dialog_new .copy_error_message "Error" \
           "Error:  there is an unnamed pin or node label (currently highlighted)" \
	   "" 0 {OK}]
	   return
         } elseif {[string first " " $i_data(_name)] >= 0} {
           select_ids $id
           set button [tk_dialog_new .copy_error_message "Error" \
           "Error:  there is a space within a pin or node label (currently highlighted)" \
	   "" 0 {OK}]
           return
        }
      }
    }
  }

  # Scale everything to 10 for saving
  set old_scale $scale
  scale_canvas 10

  # Move so that origin is at 0,0
  set offset [get_origin_offset]
  $cur_c move all [expr -1.0*[lindex $offset 0]] [expr -1.0*[lindex $offset 1]]
  
  ###### Write instances and terminals
  icon_write_terminals

  ###### Write wires, lines, arcs, text
  icon_write_all_but_instances

  ###### Write icon_setup line
  set setup "  icon_setup \$args [list [icon_default]]"

  # define the icon procedure
  proc $cur_s {args} [join "[list $setup] $PROC" "\n"]

  # unset the global variable that held the body of the icon
  unset PROC

  # move the icon back (otherwise we have to move the scrollbars, etc.)
  eval $cur_c move all $offset

  scale_canvas $old_scale
}

################################################################
#
# This section deals with writing icon terminals (aka IOs)
#
# we can only have instances of terminals in icons
#
################################################################
proc icon_write_terminals {} {

  global cur_c cur_s scale
  global PROC

  # Loop through all instances in the canvas
  foreach id [$cur_c find withtag origin] {
    if {[is_tagged $id icon_input] || [is_tagged $id icon_output] || \
	    [is_tagged $id icon_inout]} {
      # good, it's a terminal

    } else {
      # not a terminal, waste it.
      set tags [$cur_c gettags $id]
      set deadmeat [string range [lindex $tags \
				      [lsearch -regexp $tags icon_]] 5 end]
      puts "Warning: Removed instance \"$deadmeat\" from icon [get_rootname $cur_s]."

      $cur_c delete inst$id
      global ${cur_s}_inst$id
      unset ${cur_s}_inst$id

      continue
    }

    # set up data structures to get icon data

    # instance-specific data (like W=1.2u) are here
    upvar #0 ${cur_s}_inst$id i_data

    set type $i_data(type)

    # instance-generic data (like default L=0.6u) are here
    upvar #0 icon_$type g_data
    
    # Make origin argument
    set origin [round_list [lrange [$cur_c coords $id] 0 1]]
    set creator "  icon_term -type $type -origin [list $origin]"

    if {$i_data(orient) != "R0"} {
      # Non default value, so print it out
      lappend creator "-orient" $i_data(orient)
    }

    # motor through the properties
    foreach name $g_data(prop_names) {
      if {$i_data(_$name) != $g_data(_$name,default)} {
	# Non default value, so print it out
	lappend creator "-$name" $i_data(_$name)
      }
    }
    lappend PROC $creator
  }
  return
}

################################################################
#
# This section deals with icon writing.
#
# Note that wires simply become lines.
################################################################
proc icon_write_all_but_instances {} {

  global cur_c cur_s scale PROC

  # walk through everything in the canvas except grid lines
  $cur_c addtag object all
  $cur_c dtag grid object

  foreach id [$cur_c find withtag object] {
    switch [$cur_c type $id] {

      "line" {
	if {[is_tagged $id origin_icon]} {
	  # ignore the origin, it gets automatically created
	  continue
	}

	# turn wires into lines
	if {[is_tagged $id draw_item] || [is_tagged $id wire]} {
	  lappend PROC "  icon_line [round_list [$cur_c coords $id]]"
	  continue
	}
      }

      "text" {
	# all text without the tag draw_item are part of instances
	if {[is_tagged $id draw_item]} {
	  set text [string trim [lindex [$cur_c itemconfigure $id -text] 4]]
	  set origin [list [round_list [$cur_c coords $id]]]

	  # see if there are any options
	  if {[string index $text 0] == "-"} {
	    lappend PROC "  icon_property -origin $origin[text_size $id] $text"
	  } else {
	    set line "  icon_property -origin $origin[text_size $id][text_anchor $id] -label"
	    lappend line $text
	    lappend PROC $line
	  }
	}
      }

      "arc" {
	# all arcs without draw_item are part of instances
	if {[is_tagged $id draw_item]} {
	  set extent [expr round([lindex [$cur_c itemconfigure $id -extent] 4])]
	  set start [expr round([lindex [$cur_c itemconfigure $id -start] 4])]

	  lappend PROC "  icon_arc [round_list [$cur_c coords $id]] -start $start -extent $extent"
	}
      }
      default {
      }
    }
  }
  $cur_c dtag object
}


################################################################
#
# This section checks icon variables.
#
#
################################################################
proc check_icon_text_variables {} {

  global cur_c

  # walk through everything in the canvas except grid lines
  $cur_c addtag object all
  $cur_c dtag grid object

  set name_list ""

  foreach id [$cur_c find withtag object] {
      if {[$cur_c type $id] == "text"} {
	# all text without the tag draw_item are part of instances
	if {[is_tagged $id draw_item]} {
	    set text [string trim [lindex [$cur_c itemconfigure $id -text] 4]]
	    if {[lindex [split $text " "] 0] == "-type"} {
               continue
	    }
	    set index_val [string first \$ $text]
            if {$index_val != -1} {
	        set subtext [string range $text [expr $index_val + 1] end] 
		set index_val [string first \} $subtext]
                if {$index_val != -1} {
                   set subtext [string range $subtext 0 [expr $index_val - 1]] 
		}
		set index_val [string first \{ $subtext]
                if {$index_val != -1} {
                   set subtext [string range $subtext 0 [expr $index_val - 1]] 
		}
		set index_val [string first \[ $subtext]
                if {$index_val != -1} {
                   set subtext [string range $subtext 0 [expr $index_val - 1]] 
		}
		set index_val [string first \] $subtext]
                if {$index_val != -1} {
                   set subtext [string range $subtext 0 [expr $index_val - 1]] 
		}
		set index_val [string first " " $subtext]
		if {$index_val != -1} {
	           set subtext [string range $subtext 0 [expr $index_val - 1]]
		}
	    } else {
               set subtext ""
	    }
	    if {$subtext != ""} {
               lappend name_list $subtext
	    }
	}
    }
  }

  foreach label_name $name_list {
     set found_flag 0
     foreach id [$cur_c find withtag object] {
         if {[$cur_c type $id] == "text"} {
	    # all text without the tag draw_item are part of instances
   	    if {[is_tagged $id draw_item]} {
	       set text [string trim [lindex [$cur_c itemconfigure $id -text] 4]]
	       set text_list [split $text " "]
	       set name_label_index [lsearch $text_list "-name"]
		if {$name_label_index != -1} {
		    if {[lindex $text_list [expr $name_label_index + 1]] \
                         == $label_name} {
                        set found_flag 1
                        continue
		    } 
		}
	    }
	}
     }
      if {$found_flag == 0} {
         foreach id [$cur_c find withtag object] {
           if {[$cur_c type $id] == "text"} {
  	      # all text without the tag draw_item are part of instances
	      if {[is_tagged $id draw_item]} {
	         set text [string trim [lindex [$cur_c itemconfigure $id -text] 4]]
	         set index_val [string first \$ $text]
                 if {$index_val != -1} {
	            set subtext [string range $text [expr $index_val + 1] end] 
		    set index_val [string first \} $subtext]
                    if {$index_val != -1} {
                       set subtext [string range $subtext 0 [expr $index_val - 1]] 
		    }
		    set index_val [string first \{ $subtext]
                    if {$index_val != -1} {
                       set subtext [string range $subtext 0 [expr $index_val - 1]] 
		    }
		    set index_val [string first " " $subtext]
		    if {$index_val != -1} {
	               set subtext [string range $subtext 0 [expr $index_val - 1]]
		    }
	        } else {
                   set subtext ""
	        }
	        if {$subtext == $label_name} {
                   select_id $id
		}
	      }
	  }
	 }

         set button [tk_dialog_new .save_error "Error" \
	   "Can't save file since parameter '$label_name' is not declared.  To fix this issue, create the statement:   -type user -name $label_name -default default_value" "" 0 {OK}]
         return 0
      }
  }

  $cur_c dtag object

  return 1
}


# gets the list of arguments and default values of icon

proc icon_default {} {

  global PROC

  set default_list {{origin {0 0}} {orient R0}}
  foreach line $PROC {
    if {[lindex $line 0] == "icon_property"} {
      set default [eval icon_name_from_property [lrange $line 1 end]]
      if {$default != ""} {
	lappend default_list $default
      }
    }
  }
  return $default_list
}


proc icon_name_from_property {args} {
  call_by_keyword $args {{origin {0 0}} {type text} {name ""} {label ""} \
			     {text ""} {default ""} {size standard} {anchor w}}

  if {$type == "user"} {
    return [list $name $default]
  }
}


proc get_origin_offset {} {

  global cur_c cur_s scale

  # doesn't break on multiple origins (should be only one)
  set id [lindex [$cur_c find withtag origin_icon] 0]

  if {$id == ""} {
    # oops, no origin
    return "0 0"
  }

  return [round_list [lrange [$cur_c coords $id] 0 1]]
}
