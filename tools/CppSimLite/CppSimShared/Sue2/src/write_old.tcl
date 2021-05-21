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
# Routines to write schematic/icon procedures into a file
#
#################################################################

#################################################################
# 
# Top level routine to write a schematic/icon pair into a file.
# If this was a generator, is isn't any more.
#
#################################################################
proc write_file {schematic} {

  global cur_c cur_s scale SUE WRITE_FILE_ID SUFFIX WIN 

  if {[check_icon_text_variables] == 0} {
      return
  }

  if {[string compare $cur_s "no_name"] == 0} {
       set button [tk_dialog_new .modify_exit "Must Rename Cell" \
		    "Rename and save cell using 'save as' command" \
		    "" 0 {OK}]
   return
  }
  # save the current canvas, schematic, and scale
  set save_cur_c $cur_c
  set save_cur_s $cur_s
  set save_scale $scale

  set schematic_name [get_rootname $schematic]

  remember_modified

  ####### Check for existance of a file to receive the cell
  if {[info exists SUE($schematic_name)]} {
    global SUE_$schematic_name
##    set filename [set SUE_${schematic_name}(filename)]
      set title_message [wm title $WIN]
      set first_ind [string first " ---  " $title_message]
      
      set filename [string range $title_message [expr $first_ind + 6] end]
  } else {
    # see if there is icon information, instead
    if {[info exists SUE(ICON_$schematic_name)]} {
      global SUE_ICON_$schematic_name
##      set filename [set SUE_ICON_${schematic_name}(filename)]
      set title_message [wm title $WIN]
      set first_ind [string first " ---  " $title_message]
      set filename [string range $title_message [expr $first_ind + 6] end]
    } else {
      # No lo existo
      return
    }
  }

  # make sure that we have permission to write this file
  if {[file isfile $filename] == 1 && [file writable $filename] != 1} {
    puts "Aborting, could not overwrite file $filename"
    return
  }

  # rename any existing file to be a backup
  if {[file isfile $filename] == 1} {
     file rename -force $filename $filename$SUFFIX(backup)
  }

  ####### Open file
  if {[catch "set WRITE_FILE_ID \[open $filename w\]"] != 0} {
    puts "Aborting, could not create file $filename"
    ready
    return
  }

  set written ""

  # write the schematic if it exists
  if {[info exists SUE_${schematic_name}(canvas)]} {
    set cur_c [set SUE_${schematic_name}(canvas)]
    set cur_s $schematic_name
    set scale [set SUE_${schematic_name}(scale)]
    write_schematic
    set SUE_${schematic_name}(written) W

    # unmodify schematic since it has been written
    not_modified $schematic_name

    lappend written SCHEMATIC_$schematic_name

    # remake the title_bar if it exists to display new date and
    # possibly new owner.
    set id [lindex [$cur_c find withtag icon_title_bar] 0]
    if {$id != ""} {
      remake $id $id dont_modify
    }
  } elseif {[info commands SCHEMATIC_$schematic_name] != ""} {
    # there is a schematic, we just haven't made a canvas for it.
    write_proc_to_file SCHEMATIC_$schematic_name

    lappend written SCHEMATIC_$schematic_name
  }

  # write the icon if it exists
  if {[info commands ICON_$schematic_name] != ""} {
    # has the canvas been modified
    global SUE_ICON_$schematic_name

    if {[info exists SUE_ICON_${schematic_name}(canvas)] &&
	[set SUE_ICON_${schematic_name}(modified)] != ""} {
      set cur_c [set SUE_ICON_${schematic_name}(canvas)]
      set cur_s ICON_$schematic_name
      set scale [set SUE_ICON_${schematic_name}(scale)]
      # this just recreates the procedure for the icon
      write_icon with_message
      set SUE_ICON_${schematic_name}(written) W

      # unmodify icon since it has been written
      not_modified ICON_$schematic_name
    }
    # now write it to the file
    write_proc_to_file ICON_$schematic_name

    lappend written ICON_$schematic_name
  }

  ###### Close file
  close $WRITE_FILE_ID

#  if {$written != ""} {
#    puts "Wrote $written to $filename"
#  } else {
#    puts "Nothing written."
#  }

  # restore the current canvas, schematic, and scale
  set cur_c $save_cur_c
  set cur_s $save_cur_s
  set scale $save_scale

  # fix up the title (could be changed by no_modified or unmodifying something)
  display_title

  ready
}

proc write_schematic {} {

  global cur_c cur_s scale
  global PROC

  set ids [$cur_c find withtag origin]

  ####### Scale the canvas to "10" for saving
  set old_scale $scale
  scale_canvas 10

  ####### Center the canvas on the origin for saving
  #set offset [round_list_scale [center_bbox [get_schematic_bbox]] 10]
  #$cur_c move all [expr 0-[lindex $offset 0]] [expr 0-[lindex $offset 1]]

  set PROC ""

  ###### Write instances and terminals
  write_instances

  ###### Write wires
  write_wires

  ###### Write draw_items: lines, arcs, text
  write_draw_items

  # define the schematic procedure
  proc SCHEMATIC_$cur_s {} [join $PROC "\n"]

  # now write it to the file
  write_proc_to_file SCHEMATIC_$cur_s

  unset PROC

  ####### Restore the origin
  #eval $cur_c move all $offset

  ####### Restore the scale
  scale_canvas $old_scale

  set ids [$cur_c find withtag origin]

  # Loop through all instances to create list of instance names
  set instance_list ""
  foreach id $ids {
     upvar #0 ${cur_s}_inst$id i_data
     set type $i_data(type)
     upvar #0 icon_$type g_data
     foreach name $g_data(prop_names) {
	if {[string compare $name "name"] == 0} {
	    if {[string compare $i_data(_$name) ""] != 0} {
		if {[string compare $type "input"] != 0 && \
		    [string compare $type "output"] != 0 && \
		    [string compare $type "inout"] != 0 && \
		    [string compare $type "global"] != 0 && \
		    [string compare $type "name_net"] != 0 && \
		    [string compare $type "name_net_s"] != 0} {
                    lappend instance_list $i_data(_$name)
		} elseif {[string first " " $i_data(_$name)] >= 0} {
                    select_ids $id
                    set button [tk_dialog_new .copy_error_message "Error" \
                    "Error:  there is a space within a pin or node label (currently highlighted)" \
		    "" 0 {OK}]
		}
	    } else {
		if {[string compare $type "input"] == 0 || \
		    [string compare $type "output"] == 0 || \
		    [string compare $type "inout"] == 0 || \
		    [string compare $type "global"] == 0 || \
		    [string compare $type "name_net"] == 0 || \
		    [string compare $type "name_net_s"] == 0} {
                    select_ids $id
                    set button [tk_dialog_new .copy_error_message "Error" \
                    "Error:  there is an unnamed pin or node label (currently highlighted)" \
		    "" 0 {OK}]
		}
	    }
	}
     }
  }

  set sorted_instance_list [lsort $instance_list]

  set prev_instance ""
  foreach instance $sorted_instance_list {
      if {$instance == $prev_instance} {
         set selected_instance_list ""
         foreach id $ids {
            upvar #0 ${cur_s}_inst$id i_data
            set type $i_data(type)
            upvar #0 icon_$type g_data
            foreach name $g_data(prop_names) {
	       if {[string compare $name "name"] == 0} {
	          if {[string compare $i_data(_$name) $instance] == 0} {
		     if {[string compare $type "input"] != 0 && \
		        [string compare $type "output"] != 0 && \
		        [string compare $type "inout"] != 0 && \
		        [string compare $type "global"] != 0 && \
		        [string compare $type "name_net"] != 0 && \
		        [string compare $type "name_net_s"] != 0} {
                           lappend selected_instance_list $id
		     }
	          }
	       }
            }
         }
         select_ids $selected_instance_list
         set button [tk_dialog_new .copy_error_message "Error" \
           "Error:  there is more than one instance named $instance" \
		    "" 0 {OK}]
         break
      }
     set prev_instance $instance
  }



}


################################################################
#
# Write all instances (includes terminals which are instances)
#
################################################################
proc write_instances_for_duplication {{tag ""}} {

  global cur_c cur_s scale
  global PROC

  if {$tag == ""} {
    set ids [$cur_c find withtag origin]
  } else {
    set ids [get_intersect_tag $tag origin]
  }


  # Loop through all instances
  foreach id $ids {
    # instance-specific data (like W=1.2u) are here
    upvar #0 ${cur_s}_inst$id i_data

    set type $i_data(type)

      # build up the creator 
      set creator "  make $type"

      if {$i_data(orient) != "R0"} {
	# Non default value, so include it
	lappend creator "-orient" $i_data(orient)
      }

      # instance-generic data (like default L=0.6u) are here
      upvar #0 icon_$type g_data

      # motor through the properties
      foreach name $g_data(prop_names) {
	if {$i_data(_$name) != $g_data(_$name,default)} {
	  # Non default value, so include it
	  lappend creator "-$name" $i_data(_$name)
         }
      }
    
      # the origin has to be last and is never defaulted
      lappend creator "-origin"
      
    if {[info exists repeat($type)] != 1} {
      # instance-generic data (like default L=0.6u) are here
      upvar #0 icon_$type g_data

      # note that this does a generate, not a regenerate, so an existing
      # generator will not be replaced but will be used.
      if {[info exists g_data(generator)] && \
	      ($g_data(gargs) != "" || $g_data(generator) != $type)} {
	lappend PROC "  generate $g_data(generator) $type $g_data(gargs)"
      }
      set repeat($type) 1
    }

    # add the origin (the only thing that changes)
    lappend creator [round_list [lrange [$cur_c coords $id] 0 1]]

    # append it to the others
    lappend PROC $creator

    }
}


################################################################
#
# Write all instances (includes terminals which are instances)
#
################################################################
proc write_instances {{tag ""}} {

  global cur_c cur_s scale
  global PROC
  global DELETE_FLAG

  if {$tag == ""} {
    set ids [$cur_c find withtag origin]
  } else {
    set ids [get_intersect_tag $tag origin]
  }

  # Loop through all instances to create list of instance names
  set instance_list ""
  foreach id $ids {
     upvar #0 ${cur_s}_inst$id i_data
     set type $i_data(type)
     upvar #0 icon_$type g_data
     foreach name $g_data(prop_names) {
	if {[string compare $name "name"] == 0} {
	    if {[string compare $i_data(_$name) ""] != 0} {
		if {[string compare $type "input"] != 0 && \
		    [string compare $type "output"] != 0 && \
		    [string compare $type "inout"] != 0 && \
		    [string compare $type "global"] != 0 && \
		    [string compare $type "name_net"] != 0 && \
		    [string compare $type "name_net_s"] != 0} {
                    lappend instance_list $i_data(_$name)
		}
	    }
	}
     }
  }


  # Loop through all instances
  set flag 0
  foreach id $ids {
    # instance-specific data (like W=1.2u) are here
    upvar #0 ${cur_s}_inst$id i_data

    set type $i_data(type)

      # build up the creator 
      set creator "  make $type"

      if {$i_data(orient) != "R0"} {
	# Non default value, so include it
	lappend creator "-orient" $i_data(orient)
      }

      # instance-generic data (like default L=0.6u) are here
      upvar #0 icon_$type g_data

      # motor through the properties
      foreach name $g_data(prop_names) {
	if {[string compare $name "name"] == 0 && \
		[info exists DELETE_FLAG] != 1} {
            set pre [string trim $i_data(_$name)]
	    if {[string length $pre] == 1} {
		if {[string compare $type "input"] != 0 && \
		    [string compare $type "output"] != 0 && \
		    [string compare $type "inout"] != 0 && \
		    [string compare $type "global"] != 0 && \
		    [string compare $type "name_net"] != 0 && \
		    [string compare $type "name_net_s"] != 0} {
                    if {[string compare $pre "x"] == 0} {
                       set count 0
	               while {1} {
                          if {[lsearch $instance_list xi$count] == -1} {
                              set i_data(_$name) xi$count
                              lappend instance_list $i_data(_$name)
                              break
	                  }
		          set count [expr $count+1]
		       }
		    } else {
                       set count 0
	               while {1} {
                          if {[lsearch $instance_list $pre$count] == -1} {
                              set i_data(_$name) $pre$count
                              lappend instance_list $i_data(_$name)
                              break
	                  }
		          set count [expr $count+1]
		       }
		    }
                    set flag 1
	        }
	    } 
        }

	if {$i_data(_$name) != $g_data(_$name,default)} {
	   # Non default value, so include it
	   lappend creator "-$name" $i_data(_$name)
         }
      }
    
      # the origin has to be last and is never defaulted
      lappend creator "-origin"
      
    if {[info exists repeat($type)] != 1} {
      # instance-generic data (like default L=0.6u) are here
      upvar #0 icon_$type g_data

      # note that this does a generate, not a regenerate, so an existing
      # generator will not be replaced but will be used.
      if {[info exists g_data(generator)] && \
	      ($g_data(gargs) != "" || $g_data(generator) != $type)} {
	lappend PROC "  generate $g_data(generator) $type $g_data(gargs)"
      }
      set repeat($type) 1
    }

    # add the origin (the only thing that changes)
    lappend creator [round_list [lrange [$cur_c coords $id] 0 1]]

    # append it to the others
    lappend PROC $creator

    if {$flag == 1 && [info exists DELETE_FLAG] != 1} {
        remake $id $id
        set flag 0
    }

  }

}


################################################################
#
# Writes all wires.
#
################################################################
proc write_wires {{tag ""}} {

  global cur_c PROC

  if {$tag == ""} {
    set ids [$cur_c find withtag wire]
  } else {
    set ids [get_intersect_tag $tag wire]
  }

  # walk through all wires in the canvas
  foreach id $ids {
    lappend PROC "  make_wire [round_list [$cur_c coords $id]]"
  }
}

################################################################
#
# Writes all draw_items: lines, arcs, and text.
#
################################################################
proc write_draw_items {{tag ""}} {

  global cur_c PROC

  if {$tag == ""} {
    set ids [$cur_c find withtag draw_item]
  } else {
    set ids [get_intersect_tag $tag draw_item]
  }

  # walk through all draw_items
  foreach id $ids {
    switch [$cur_c type $id] {

      "line" {
	# don't save icon origins
	if {[is_tagged $id origin_icon] != 1} {
	  lappend PROC "  make_line [round_list [$cur_c coords $id]]"
	}
      }

      "text" {
	set text [lindex [$cur_c itemconfigure $id -text] 4]
	if {$text == ""} {
	  # toss turds
	  continue
	}
	set origin [round_list [$cur_c coords $id]]
	lappend PROC "  make_text -origin [list $origin] -text [list $text][text_size $id][text_anchor $id]"
      }

      "arc" {
	set extent \
	    [expr round([lindex [$cur_c itemconfigure $id -extent] 4])]
	set start [expr round([lindex [$cur_c itemconfigure $id -start] 4])]
	lappend PROC "  make_arc [round_list [$cur_c coords $id]] -start $start -extent $extent"
      }
      default {
	puts "Unknown canvas type [$cur_c type $id] during writing."
	continue
      }
    }
  }
}
  
proc write_proc_to_file {proc} {

  global WRITE_FILE_ID

  puts $WRITE_FILE_ID "proc $proc [list [info args $proc]] \{"

  foreach line [split [info body $proc] "\n"] {
    puts $WRITE_FILE_ID "$line"
  }
  puts $WRITE_FILE_ID "\}\n"
}


proc get_rootname {schematic} {

  if {[lsearch $schematic "ICON_*"] != -1} {
    return [string range $schematic 5 end]
  } else {
    return $schematic
  }
}
