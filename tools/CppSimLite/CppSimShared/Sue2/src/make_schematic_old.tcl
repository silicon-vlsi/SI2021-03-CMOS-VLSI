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

# called by other procedures to make a new canvas and pack it.

proc make_schematic {schematic {type S}} {

  global cur_c cur_s SUE WIN

#  set win [use_first win '.schematics]
#  if {[winfo exists $win] == 1} {
#     set cur_dir [$win.dir cget -text]

#     set filename "$SUE_DIR/current_cell.cppsimview"

#     if {[catch "set CUR_CELL_FILE_ID \[open $filename w\]"] != 0} {
#        puts "Aborting, could not create file $filename"
#        ready
#        return
#     }

#     puts $CUR_CELL_FILE_ID $cur_dir/$schematic

     ###### Close file
#     close $CUR_CELL_FILE_ID
#  }
################## Save current cell name for CppSimView #########
########## End of: Save current cell name for CppSimView #########

  if {[info exists cur_c]} {
    catch {unpack_canvas}
  }

  # never been loaded before, setup entry in global database
  set SUE($schematic) $schematic

  make_canvas $WIN $schematic $type
  pack_canvas $WIN

  setup_bindings

  # only icon schematics need origins
  if {$type == "I"} {
    make_icon_origin
  }

  toggle_grid

  return $cur_s
}


# displays a different schematic

proc goto_schematic {schematic {win ""}} {

  global cur_c cur_s scale WIN SUE COLORS STATE ICON_MENU

  if {$win != "ignore_win_for_selection" && $ICON_MENU == ""} {
     set active_ind [$win curselection]
     if {$active_ind != ""} {
        set active_val [$win get $active_ind]
        set ind [string last " " $active_val]
        set new_val [string range $active_val [expr ${ind} + 1] end]
        set schematic $new_val
     } else {
        return
     }
  }

  # are we already here?
  if {$cur_s == $schematic} {
    return
  } 

  # see if there already exists a canvas
  if {[info exists SUE($schematic)]} {
    # yes, unpack old canvas, enter the new canvas, and pack it in.

    catch {unpack_canvas}
    enter_canvas $schematic
    pack_canvas $WIN

    return $cur_c
  }

  # if it's an icon, it should have a procedure defined
  if {[is_icon $schematic]} {
    if {[info commands $schematic] != ""} {

      catch {unpack_canvas}

      # make a canvas and put the icon into it
      make_new_schematic $schematic I

      # generators have special generator arguments
      set iname [get_rootname $schematic]
      upvar #0 icon_$iname g_data
      icon_make $iname [use_first g_data(gargs)]

      setup_icon_colors

      if {[info exists STATE($schematic,exists)]} {
          scale_canvas $STATE($schematic,scale)
          $cur_c xview moveto $STATE($schematic,hscroll)
          $cur_c yview moveto $STATE($schematic,vscroll)
          unset STATE($schematic,exists)
      } else {
          zoom_to_fit
      }

      ready

      return $cur_c
      
    } else {
      # no lo existo
      return
    }
  } else {
    # must be a schematic

    # see if there is a schematic generator
    upvar #0 icon_$schematic g_data
    set genname [lindex [split_filename [use_first g_data(generator)]] 1]
    if {[info commands SCHEMATIC_$genname] != ""} {
      catch {unpack_canvas}

      # make a canvas and put the schematic into it
      make_new_schematic $schematic
      eval SCHEMATIC_$genname $g_data(gargs) -name $schematic

      # add a comment to alert user that this is a defined schematic
      set bbox [$cur_c bbox all]
      make_text -origin "[lindex $bbox 0] [expr [lindex $bbox 1] -50]" -text \
	  "Schematic generated from $g_data(generator) $g_data(gargs)" \
	  -size large

      zoom_to_fit

      # making this schematic may have defined some new icons, so...
      make_icon_listbox

      display_title

      # show connection info
      show_connects "" fast

      ready

      return $cur_c
    }

    # is there a schematic procedure for it
    if {[info commands SCHEMATIC_$schematic] != ""} {

      catch {unpack_canvas}

      # make a canvas and put the schematic into it
      make_new_schematic $schematic

#	puts [info commands SCHEMATIC_$schematic]

      SCHEMATIC_$schematic
      
      setup_schematic_colors

      if {[info exists STATE($schematic,exists)]} {
          scale_canvas $STATE($schematic,scale)
          $cur_c xview moveto $STATE($schematic,hscroll)
          $cur_c yview moveto $STATE($schematic,vscroll)
          unset STATE($schematic,exists)
      } else {
          zoom_to_fit
      }
      # making this schematic may have defined some new icons, so...
      make_icon_listbox

      display_title

      # show connection info
      show_connects "" fast

      ready

      return $cur_c
    }

    # try looking for the icon instead
    return [goto_schematic ICON_$schematic "ignore_win_for_selection"]
  }
}


# makes a new schematic

proc make_new_schematic {{schematic ""} {type S}} {

  global SUE COPY_SCHEMATIC_FLAG

if {$schematic == ""} {
    # get the name of the file from the file selector box
    if {[info exists COPY_SCHEMATIC_FLAG]} {
        set filename [SueFileDialog "save" "Save As"]
        # if nil, file selector box cancelled -- do nothing
        if {$filename == ""} { 
           return "" 
        }

        set path_list [split_filename $filename]
        set schematic [lindex $path_list 1]
        set list_schem_names [regsub -all SCHEMATIC_ \
				[info commands SCHEMATIC_*] ""]
        foreach schem_name $list_schem_names {
           if {[string compare $schem_name $schematic] == 0} {
              set button [tk_dialog_new .copy_error_message "Error" \
              "New schematic not made - Schematic $schematic already exists" \
		    "" 0 {OK}]
	       if {[info exists COPY_SCHEMATIC_FLAG]} {
                  unset COPY_SCHEMATIC_FLAG
	       }
              return ""
	   }
        }
    } else {
      if {$type == "S"} {
        set filename [SueFileDialog "save" "New Schematic"]
        # if nil, file selector box cancelled -- do nothing
        if {$filename == ""} { 
           return "" 
        }

        set path_list [split_filename $filename]
        set schematic [lindex $path_list 1]
        set list_schem_names [regsub -all SCHEMATIC_ \
				[info commands SCHEMATIC_*] ""]
        foreach schem_name $list_schem_names {
           if {[string compare $schem_name $schematic] == 0} {
              set button [tk_dialog_new .copy_error_message "Error" \
              "New schematic not made - Schematic $schematic already exists" \
		    "" 0 {OK}]
	      if {[info exists COPY_SCHEMATIC_FLAG]} {
                  unset COPY_SCHEMATIC_FLAG
	      }
              return ""
	   }
        }

      } else {
        set filename [SueFileDialog "save" "New Icon"]
        # if nil, file selector box cancelled -- do nothing
        if {$filename == ""} { 
           return "" 
        }

        set path_list [split_filename $filename]
        set schematic [lindex $path_list 1]
        set list_schem_names [regsub -all ICON_ \
				[info commands ICON_*] ""]
        foreach schem_name $list_schem_names {
           if {[string compare $schem_name $schematic] == 0} {
              set button [tk_dialog_new .copy_error_message "Error" \
              "New icon not made - Icon $schematic already exists" \
		    "" 0 {OK}]
	       if {[info exists COPY_SCHEMATIC_FLAG]} {
                   unset COPY_SCHEMATIC_FLAG
	       }
              return ""
	   }
        }

      }
    }

    # if nil, file selector box cancelled -- do nothing
    if {$filename == ""} { 
	if {[info exists COPY_SCHEMATIC_FLAG]} {
            unset COPY_SCHEMATIC_FLAG
	}
        return 
    }

    add_auto_path $filename

    # break apart the directory, the fileroot name, and the suffix
    set path_list [split_filename $filename]
    set dir [lindex $path_list 0]
    set schematic [lindex $path_list 1]
    set suffix [lindex $path_list 2]

    if {[string range $schematic 0 4] == "ICON_"} {
      puts "Aborting.  Cannot start schematic name with the characters \"ICON_\""
      return
    }

    if {$type == "I"} {
      set schematic "ICON_$schematic"
    }

    # See if this schematic name has already been used.  If so, punt.
    if {[info exists SUE($schematic)]} {
      set button [tk_dialog_new .copy_error_message "Error" \
       "New schematic not made - Schematic $schematic already exists" \
		    "" 0 {OK}]

      puts "Aborting.  $schematic already exists."
      return
    }

} else {
    # break apart the directory, the fileroot name, and the suffix
    set path_list [split_filename $schematic]
    set dir [lindex $path_list 0]
    set schematic [lindex $path_list 1]
    set suffix [lindex $path_list 2]

    if {$type == "I"} {
      set schematic "ICON_[get_rootname $schematic]"
    }
  }

  if {[info exists SUE($schematic)]} {
    goto_schematic $schematic "ignore_win_for_selection"
    return
  }

  set value [make_schematic $schematic $type]

  make_filename $dir $suffix

  # if it's an icon, need to enter into icon listbox
  if {$type == "I" && [info commands $schematic] == ""} {


    # create a placeholder procedure for this icon (needed for
    # make_icon_listbox)
    write_icon NULL

    # need to do this again now that there is an icon
    make_filename $dir $suffix

    # update the icon listbox
    make_icon_listbox
  } 
  set i [string last "/" $dir]
  set libname [string range $dir 0 [expr ${i} - 1]]
  set i [string last "/" $libname]
  set libname [string range $libname [expr ${i} + 1] end]

  if {$libname != "" && $dir != ""} {

      set new_filename $dir$schematic.sue
      if {[file exists $new_filename] == 0} {
          if {$type == "S"} {
              if {[catch {set fp [open $new_filename w]} err] != 0} {
                 puts "Error: failed to create $new_filename in library $libname"
              } else {
                 puts $fp "proc SCHEMATIC_$schematic {} {"
	         puts $fp "}"
                 close $fp
	      }
          }
      }
#      add_auto_path $new_filename
#      add_to_auto_index $schematic $new_filename

      set fake_module_name "first_module_for_$libname"
      if {[string compare $schematic $fake_module_name] != 0} {
          set fake_module_filename $dir$fake_module_name.sue
          if {[file exists $fake_module_filename] == 1} {
              if {[catch {file delete -force $fake_module_filename} err] != 0} {
                 puts "Error: failed to delete $fake_module_name in library $libname"
	      }
	  }
      }
  }
  display_title

  return $value
}


# Loads a file by sourcing the file.  If there is a procedure of name
# SCHEMATIC_$schematic where $schematic is the filename minus directory
# and suffix, then displays the schematic.  Also updates the icon
# listbox to show any icons that may have been loaded.

# Note: any schematic other procedures in this file will get their
# procedures defined but will be otherwise unknown.

proc load_schematic {{filename ""}} {

  global SUE COLORS cur_c cur_s

  if {$filename == ""} {
    # get the name of the file from the file selector box
    set filename [SueFileDialog "open" "Load"]

    # if nil, file selector box cancelled -- do nothing
    if {$filename == ""} { 
      return 
    }

  }

  # break apart the directory, the fileroot name, and the suffix
  set path_list [split_filename $filename]
  set dir [lindex $path_list 0]
  set schematic [lindex $path_list 1]
  set suffix [lindex $path_list 2]

  # This is only done to fix up the suffix if it is gone
  set filename "$dir$schematic$suffix"

  global SUE_$schematic STATE

  # special case for no_name schematic
  if {$schematic == "no_name"} {
    if {[delete_schematic $schematic 1] == -1} {
      return
    }
    if {[delete_schematic ICON_$schematic] == -1} {
      return
    }
  }

  if {$schematic == $cur_s} {
      return
  }
  # See if there is already a schematic or icon of the same name.  If so
  # just goto it.
  if {[info exists STATE($schematic,exists)]} {
    puts "Schematic $schematic already exists.  Switching to $schematic."
    goto_schematic $schematic "ignore_win_for_selection"
    return
  }
  if {[info exists STATE(ICON_$schematic,exists)]} {
    puts "Icon $schematic already exists, aborting load."
    return
  }

  add_auto_path $filename
  
  # Load the file (don't know if it contains a schematic, an icon, or both)
  puts "Loading $filename ..."
  if {[catch "source $filename" error]} {
    # failed, file probably doesn't exist
    puts $error

    # this gets confusing with no_name.
    if {$schematic == "no_name"} {
      make_new_schematic no_name
    }
    ready
    return
  }

  # if there is a schematic, bring it up in a canvas
  if {[info commands SCHEMATIC_${schematic}] != ""} {
    # make a canvas and display it

    make_schematic $schematic

    make_filename $dir $suffix

    # execute the schematic proc into the new canvas
    SCHEMATIC_${schematic}

    zoom_to_fit

    display_title

    # show connection info
    show_connects "" fast
} elseif {[info commands ICON_${schematic}] != ""} {
    goto_schematic ICON_$schematic "ignore_win_for_selection"
}

  # force wish to load in all of the tclIndexes
  catch perrott

  # update the icon listbox.  Change the directory (file dir removes /)
#  make_icon_listbox [file dir $dir]
  make_icon_listbox
  ready
}


# if the current canvas is a schematic, switch to the icon.  If no
# icon exists, punt.  If the current canvas is an icon, switch to 
# the schematic if one exists.

proc change_views {} {

  global cur_s

  goto_schematic [corresponding_cell $cur_s] "ignore_win_for_selection"
}


# returns the name of the corresponding cell: the icon for a schematic
# of the schematic for an icon.

proc corresponding_cell {cell} {

  # is this an icon name
  if {[is_icon $cell]} {
    # the rootname is the schematic name
    return [get_rootname $cell]
  } else {
    # make the icon name
    return ICON_$cell
  }
}


# makes an icon for a schematic if none exists and puts the i/o's and
# the properies "name" and "M" into it.

proc make_icon {} {

  global cur_c cur_s scale FONT DEFAULT_PROPERTIES COLORS

  # if we already are an icon, punt.
  if {[is_icon $cur_s]} {
    return
  }

  # save the old current schematic, we need it later
  set schematic $cur_s

  set icon ICON_$cur_s

  # if the icon already exists, just go to it.
  if {[info commands $icon] != ""} {
    change_views
    return
  }

  # make a new icon

  # first get all the io's from the schematic
  set ios ""

  set count 0
  foreach id [$cur_c find withtag icon_input] {
      set count [incr count 1]
  }
  set count2 0
  foreach id [$cur_c find withtag icon_output] {
      set count2 [incr count2 1]
  }
  foreach id [$cur_c find withtag icon_inout] {
      set count2 [incr count2 1]
  }
  if {$count2 > $count} {
     set count $count2
  }

  set x_tl -100
  set y_tl [expr -10*$count]

  set x $x_tl
  set y $y_tl

  foreach id [$cur_c find withtag icon_input] {
    global ${cur_s}_inst$id
    set name [set ${cur_s}_inst${id}(_name)]
    regsub -all {\[|\]} $name \\\\& name

    lappend ios "make input -origin [list [list $x [incr y 20]]] -name $name"
    lappend ios "make_text -origin \"[expr $x + 10] $y\" -text $name"
  }

  set y_inputs [incr y 20]

  set x [expr $x_tl + 200]
  set y $y_tl
  foreach id [$cur_c find withtag icon_output] {
    global ${cur_s}_inst$id
    set name [set ${cur_s}_inst${id}(_name)]
    regsub -all {\[|\]} $name \\\\& name

    lappend ios "make output -origin [list [list $x [incr y 20]]] -name $name"
    lappend ios "make_text -origin \"[expr $x - 10] $y\" -anchor e -text $name"
  }
#  set x 0
#  set y -100

  set y [incr y 20]

  set flag 0
  foreach id [$cur_c find withtag icon_inout] {
    set flag 1
    global ${cur_s}_inst$id
    set name [set ${cur_s}_inst${id}(_name)]
    regsub -all {\[|\]} $name \\\\& name

    lappend ios "make inout -origin [list [list $x [incr y 20]]] -name $name"
    lappend ios "make_text -origin \"[expr $x - 15] $y\" -anchor e -text $name"
  }

  if {$flag == 0} {
      set y_rb $y
  } else {
      set y_rb [incr y 20]
  }

  if {$y_inputs > $y_rb} {
    set y_rb $y_inputs
  }

  set x [expr $x_tl + 10]
  set y [expr $y_tl - 30]
  lappend ios "make_text -origin \"$x $y\" -anchor w -text {\$name}"
  set y [incr y 20]
  set module_name $cur_s
  lappend ios "make_text -origin \"$x $y\" -text $module_name"

  set x $x_tl
  set x_rb [expr $x_tl + 200]
  set y [expr $y_tl - 20]
  lappend ios "$cur_c create line $x $y $x_rb $y $x_rb $y_rb $x $y_rb $x $y -tags \"draw_item\" -fill $COLORS(icon)"

  set x [expr $x_tl + 10]
  set y [expr $y_rb + 10]
  lappend ios "make_text -origin \"$x $y\" -anchor w -text {example_param=\$example_param}"
  set y [incr y 60]
  lappend ios "make_text -origin \"$x $y\" -anchor w -text {-type user -name name -default x}"
  set y [incr y 20]
  lappend ios "make_text -origin \"$x $y\" -anchor w -text {-type user -name example_param -default 1.0}"

  # now make a "schematic" of the name ICON_foo of type icon 
  upvar #0 SUE_$cur_s schem
  make_new_schematic $schem(filename) I

  # put the io's in the new icon
  foreach command $ios {
    eval $command
  }

  # put the default properties into it
#  add_properties_to_icon

  # make this modified
  is_modified

  zoom_to_fit
}


proc add_properties_to_icon {} {

  global cur_c cur_s DEFAULT_PROPERTIES scale FONT

  set incy [expr $scale * 2]
  set x [expr 0 - $scale * 5]
  set y [expr $scale * 5]

  foreach prop $DEFAULT_PROPERTIES {
    make_text -origin "$x [incr y $incy]" -text $prop
  }
}


# pushes into a schematic or, if there is only an icon (say for a primitive),
# into an icon, using the selection.  Save state in HIERARCHY.

proc push_into_schematic {} {

  global cur_c cur_s
  global HIERARCHY

  set current_id [lindex [$cur_c find withtag selected] 0]
  set id [find_origin $current_id]
  if {[is_tagged $id origin] != 1} {

    # not an icon, can't push into
    return
  }

  set HIERARCHY [use_first HIERARCHY]

  set tags [$cur_c gettags $id]
  set schematic [string range [lindex $tags [lsearch $tags "icon_*"]] 5 end]

  # don't push into an icon recursively, show icon instead of schematic
  if {$schematic == $cur_s} {
    change_views
    return
  }

  set HIERARCHY "$cur_s,$id $HIERARCHY"

# puts "push into hierarchy: $HIERARCHY"

  goto_schematic $schematic "ignore_win_for_selection"
}


# pop out the current schematic using the HIERARCHY global

proc pop_out_of_schematic {} {

  global HIERARCHY

  if {[info exists HIERARCHY] != 1} {
    return
  }

  if {$HIERARCHY == ""} {
    # no where to go
    return
  }

  set schematic [lindex $HIERARCHY 0]
  set HIERARCHY [lrange $HIERARCHY 1 end]

# puts "push out of hierarchy: $HIERARCHY"

  goto_schematic [lindex [split $schematic ,] 0] "ignore_win_for_selection"
}


# Copies an entire schematic to a new canvas with a new name

proc copy_schematic {{schematic ""}} {

  global cur_c cur_s scale SUE
  global SUE_${cur_s}

  global COPY_SCHEMATIC_FLAG
  set COPY_SCHEMATIC_FLAG 1

  set copy_schematic $cur_s

  # if this is an icon, only copy the icon, not the schematic
  if {[is_icon $cur_s]} {

    set button [tk_dialog_new .copy_warning_message "Warning" \
       "Save As on an Icon view only saves the icon.  To save both \
        schematic and icon, perform Save As on the corresponding 
        schematic view" \
		    "" 0 {Continue} {Cancel}]

    if {$button == 1} {
      # user hit the cancel key
      return
    }

    # copy the icon and show the new icon in a canvas

    if {[info exists SUE($schematic)]} {
      set button [tk_dialog_new .copy_error_message "Error" \
       "Copy not allowed - Icon [get_rootname $schematic] already exists" \
		    "" 0 {OK}]

      puts "Aborting.  Icon [get_rootname $schematic] already exists"
      if {[info exists COPY_SCHEMATIC_FLAG]} {
         unset COPY_SCHEMATIC_FLAG
      }
      return -1
    }

    set save_scale $scale
    # make a new one.  this will ask for the new name
    set id [make_new_schematic $schematic I]
    if {$id == ""} {
      # cancelled
      if {[info exists COPY_SCHEMATIC_FLAG]} {
      unset COPY_SCHEMATIC_FLAG
      }
      return -1
    }

    puts "Copying $copy_schematic to $cur_s"

    # now put the old icon in it
    icon_make [get_rootname $copy_schematic] ""

    # make this modified
    is_modified

#    zoom_to_fit
    scale_canvas $save_scale

    ready

    write_file $cur_s

    if {[info exists COPY_SCHEMATIC_FLAG]} {
       unset COPY_SCHEMATIC_FLAG
    }
    return
  }

  # duplicate the schematic
  if {$schematic != "" && [info exists SUE($schematic)]} {
      set button [tk_dialog_new .copy_error_message "Error" \
       "Copy not allowed - Schematic $schematic already exists" \
		    "" 0 {OK}]

    puts "Aborting.  Schematic $schematic already exists"
    return -1
  }

  global PROC
  set PROC ""

  set save_scale $scale
  scale_canvas 10

  set icon ICON_$cur_s

  write_instances
  write_wires
  write_draw_items

  scale_canvas $save_scale

  # the PROC global can get squashed
  set save_proc $PROC
  unset PROC

  ready


  # make a new one.  this will ask for the new name
  set id [make_new_schematic $schematic]
  if {$id == ""} {
    # cancelled
    return -1
  }

  # put everything from the first one into it.
  foreach line $save_proc {
    eval $line
  }

  if {$save_proc != ""} {
    # flag that this canvas has been modified
    is_modified
  }

#  zoom_to_fit
  scale_canvas $save_scale

  # show connection info
  show_connects "" fast

  # does this have an icon with it? (if you are already in the icon
  # then you don't get a copy of the schematic, even if there is one).
  if {[info commands $icon] != ""} {
    # copy the icon procedure
    proc ICON_$cur_s [info args $icon] [info body $icon]

    # need to enter this icon into the auto index
    global auto_index SUE_auto_index SUE_$cur_s
    set auto_index(ICON_$cur_s) "source [set SUE_${cur_s}(filename)]"
    set SUE_auto_index(ICON_$cur_s) "source [set SUE_${cur_s}(filename)]"
    # update the icon listbox
    make_icon_listbox
} 

  # replace recursive icons if there are any
  set ids [$cur_c find withtag icon_$copy_schematic]
  if {$ids != ""} {
    # need to make a bogus icon first
    set bogus_id [make $cur_s]

    # now replace all recursive icons
    foreach id $ids {
      remake $id $bogus_id
    }

    # delete bogus icon and data structures
    $cur_c delete inst$bogus_id
    upvar #0 ${cur_s}_inst$bogus_id i_data
    unset i_data
  }

  ready

  write_file $cur_s

  global SUE_$cur_s
  add_to_auto_index $cur_s [set SUE_${cur_s}(filename)]

  if {[info exists COPY_SCHEMATIC_FLAG]} {
     unset COPY_SCHEMATIC_FLAG
  }
update_listboxes
}


# Walk down the hierachy and save modified cells.

proc modified_save_and_leaves {schematic} {

  global SCHEMS

  catch {unset SCHEMS}

  modified_save_and_leaves_int [get_rootname $schematic]

  if {![info exists SCHEMS(written)]} {
    puts "No cells need to be saved."
  }
}


proc modified_save_and_leaves_int {schematic} {

  global cur_c cur_s SCHEMS SUE

  if {[info exists SCHEMS($schematic)]} {
    # already been here
    return
  }

  set SCHEMS($schematic) traced

  upvar #0 SUE_$schematic schem_array
  upvar #0 SUE_ICON_$schematic icon_array

  if {([info exists SUE($schematic)] && $schem_array(modified) != "") || \
	  ([info exists SUE(ICON_$schematic)] && $icon_array(modified) != "")} {
    write_file $schematic
    set SCHEMS(written) 1
  }
  
  if {![info exists SUE($schematic)]} {
    return
  }

  set canvas $schem_array(canvas)

  foreach id [$canvas find withtag origin] {
    upvar #0 ${schematic}_inst${id} i_data
    # the type is really the instance name
    set type $i_data(type)

    # if this has a schematic, trace down through it's hierarchy
    if {[info exists SUE($type)] || [info exists SUE(ICON_$type)]} {
      modified_save_and_leaves_int $type
    }
  }
}


# Changes the path of a given schematic and it's icon

proc change_path {} {

  global cur_c cur_s SUE SUFFIX

  set schematic [get_rootname $cur_s]
  global SUE_${schematic}

  # if there is no schematic, then look in the icon
  if {[info exists SUE_${schematic}(canvas)] != 1} {
    set schematic $cur_s
    global SUE_${schematic}
  }

  set dir [set SUE_${schematic}(dir)]
  if {$dir == ""} {
    set dir "[pwd]/"
  }

  # Prompt for a file name
  #   The following is now invalid - there is no FSBox function anymore
#  set filename [FSBox "Enter new path:" "$dir${schematic}.sue" "New Path"]

  # if nil, file selector box cancelled -- do nothing
  if {$filename == ""} { 
    return 
  }

  set path_list [split_filename $filename]
  set dir [lindex $path_list 0]
  set new_schematic [lindex $path_list 1]
  set suffix [lindex $path_list 2]

  if {$new_schematic != "" && $new_schematic != $schematic} {
    # the user is trying to change the name also.
    puts "Aborting. Can't change file name, only path."
    return
  }

  if {[set SUE_${schematic}(dir)] != $dir} {
    # remake the filename stuff
    make_filename $dir $suffix

    # remake the title_bar if it exists to display new date and
    # possibly new owner.
    set id [lindex [$cur_c find withtag icon_title_bar] 0]
    if {$id != ""} {
      remake $id $id dont_modify
    }

    puts "Changing path of cell \"$schematic\" to $dir"

    # possibly rearranged some icons
    make_icon_listbox
  }

  display_title
}


# deletes a schematic or icon.  Doesn't delete associated schematic/icon.

proc delete_schematic {schematic {even_no_name ""}} {

  global cur_s SUE MODIFY SUE_DIR SUE_auto_index

  # is the thing to be deleted in any canvas
  if {[info exists SUE($schematic)] != 1 } {
    # not in a canvas
    if {[is_icon $schematic]} {
      if {[info commands $schematic] != ""} {
	# lose the icon procedure
	set schematic_name [get_rootname $schematic]
	puts "Icon $schematic_name deleted."
	rename $schematic ""

	# removed the compiled versions, if they exist
	catch {rename _MAKE_$schematic_name ""}
	catch {rename _MAKE90_$schematic_name ""}

	catch {unset SUE_auto_index($schematic)}

	# update the icon listbox
	make_icon_listbox

	return
      }
    } else {
      # must be a schematic
      if {[info commands SCHEMATIC_$schematic] != ""} {
	# lose the schematic procedure
	puts "Schematic $schematic deleted."
	rename SCHEMATIC_$schematic ""

	catch {unset SUE_auto_index(SCHEMATIC_$schematic)}
	
	# don't need to update schematic listbox since it isn't there.
	return
      }
    }

    # no lo existo
    return
  }

  global SUE_$schematic

  # if the canvas is modified, ask the user for confirmation before deleting
  set modified_cells ""
  if {[set SUE_${schematic}(modified)] != ""} {
    if {[is_icon $schematic]} {
      set icon [get_rootname $schematic]
      lappend modified_cells "Icon \"$icon\" is modified."
    } else {
      lappend modified_cells "Schematic \"$schematic\" is modified."
    }
  }

  if {$modified_cells != ""} {
    set button [tk_dialog_new .modify_exit "Cells Modified" \
		    [join $modified_cells "\n"] \
		    "" 0 {delete} {cancel}]

    if {$button == 1} {
      # user hit the cancel key
      return -1
    }
  }

  if {$cur_s == $schematic} {
    catch {unpack_canvas}
    # goto bogus scratch schematic
    if {$even_no_name == ""} {
      make_new_schematic no_name
    }
  }

  # Now delete the canvas
#  set canvas [set SUE_${schematic}(canvas)]
#  destroy $canvas

  unset SUE($schematic)
  unset SUE_$schematic

  # remove modify history if it exists
  catch "unset MODIFY($schematic)"

  # remove any instance data
  catch [list uplevel #0 \
	     "unset [uplevel #0 [list info vars ${schematic}_inst*]]"]

  set schematic_name [get_rootname $schematic]

  # remove netlist cached data
  global NETLIST_CACHE
  catch {unset NETLIST_CACHE($schematic_name)}

  # remove associated procs
  if {[is_icon $schematic]} {
    # lose the icon
    puts "Icon $schematic_name deleted."

    rename ICON_$schematic_name ""
    catch {rename _MAKE_$schematic_name ""}
    catch {unset SUE_auto_index($schematic)}

    # update the icon listbox
    make_icon_listbox

  } else {
    puts "Schematic $schematic_name deleted."

    # lose the schematic procedure if it exists
    catch {rename SCHEMATIC_$schematic_name {}}
    catch {unset SUE_auto_index(SCHEMATIC_$schematic)}

    # take out of schematic listbox
    remove_schematic_from_listbox $schematic
  }

  # special case for no_name schematic
  if {$schematic_name == "no_name" && $even_no_name == ""} {
    # make a new empty schematic
    make_new_schematic no_name
  }
}


# To erase the grid and make hightlighted things appear normally
set COLORMAP($COLORS(background)) {1.0 1.0 1.0 setrgbcolor}
set COLORMAP($COLORS(fore)) {0.0 0.0 0.0 setrgbcolor}
set COLORMAP($COLORS(grid)) {1.0 1.0 1.0 setrgbcolor}
set COLORMAP($COLORS(active)) {0.0 0.0 0.0 setrgbcolor}
set COLORMAP($COLORS(selected,active)) {0.0 0.0 0.0 setrgbcolor}
set COLORMAP($COLORS(selected)) {0.0 0.0 0.0 setrgbcolor}

# setup the page sizes for normal 8.5x11 in2 paper
set PRINT(WIDTH) 8.3
set PRINT(HEIGHT) 10.7


proc make_ps_simplified {} {
    
  global cur_c cur_s scale WIN COLORS env COLORMAP PRINT SUE_DIR FONT SUFFIX

  global SUE_$cur_s

  set ps_file [SuePSFileDialog "save" "Postscript Filename"]
  if {$ps_file == ""} {
     return
  } else {
     set ps_file $ps_file.eps
  }


  set save_scale $scale
  set xview [lindex [$cur_c xview] 0]
  set yview [lindex [$cur_c yview] 0]


  $cur_c delete tmp

  # center for the camera
  zoom_to_fit
  turn_off_grid
  setup_print_colors

  eval $cur_c postscript -file $ps_file -rotate $PRINT(ORIENT) \
      -colormode mono -pagewidth 8.0i 


  # restore icons/wires to normal color, and reselect to get the color back

  upvar #0 SUE_$cur_s data
  if { $data(type) != "I" } {
     setup_schematic_colors
  } else {
     setup_icon_colors
  }


  # restore
  scale_canvas [expr 41 - $save_scale]

  scale_canvas $save_scale
  $cur_c xview moveto $xview
  $cur_c yview moveto $yview
  turn_on_grid

  if {[is_icon $cur_s]} {
    puts "Wrote postscript for icon \"[get_rootname $cur_s]\" to file $ps_file"
  } else {
    puts "Wrote postscript for schematic \"$cur_s\" to file $ps_file"
  }

  ready
}


# utilities

# Slightly different than file rootname because foo.bar.baz needs to
# have a rootname of foo and a suffix of bar.baz since we can't have
# "." in a windowname.  Also, directory comes complete with trailing /

proc split_filename {filename} {

  global SUFFIX

  # is this a directory
  if {[file isdir $filename]} {
    set dir [clean_dir $filename]/
    set fileroot ""
    set suffix ""
    
  } else {
    if {[file dirname $filename] == "."} {
      set dir ""
    } else {
      set dir [clean_dir [file dirname $filename]]/
    }

    set fileparts [split [file tail $filename] .]
    set fileroot [lindex $fileparts 0]
    set suffix [join [lrange $fileparts 1 end] .]
    if {$suffix != ""} {
      set suffix .$suffix
    }
  }

  if {$suffix == ""} {
    set suffix $SUFFIX(default)
  }

  # need to use "list" here
  if {$fileroot == ""} {
      set fileroot $filename
  }
  return [list $dir $fileroot $suffix]
}

# icon's are always of the form ICON_<name>

proc is_icon {name} {

  if {[string range $name 0 4] == "ICON_"} {
    return 1
  } else {
    return 0
  }
}


# Checks to see if the path of the filename is already in the auto_path
# and if not adds it and recomputes the tclindex.

proc add_auto_path {filename} {

  global auto_path SUFFIX auto_index SUE_auto_index

  set date 0
  # is the filename a directory
  if {[file isdir $filename]} {
    set dir [clean_dir $filename]
  } else {
    # no, it must be a file
    set dir [clean_dir [file dirname $filename]]
    catch "set date \[file mtime $filename\]"
  }

  # figure out the appropriate suffix
  set suffix [file extension $filename]
  if {$suffix == $SUFFIX(backup)} {
    # strip off the backup suffix
    set suffix [file extension [string range $filename 0 \
				    [expr [string last . $filename] - 1]]]
  }
  if {$suffix == "" || $suffix == "."} {
    set suffix $SUFFIX(default)
  }

  if {[catch "set index_date \[file mtime $dir/tclIndex\]"] != 0} {
    set index_date -10
  }

  # compute a new tcltags if needed 
  set SUFFIX($dir) [use_first SUFFIX($dir) SUFFIX(tclindex)]
  if {[lsearch $auto_path $dir] == -1 || $date > $index_date || \
	  [lsearch $SUFFIX($dir) "*$suffix"] == -1} {

    # remember this suffix for the next time
    if {[lsearch $SUFFIX($dir) "*$suffix"] == -1} {
      lappend SUFFIX($dir) *$suffix
    }

    # add directory to auto path if needed.
    if {[lsearch $auto_path $dir] == -1} {
#      puts "Adding $dir to auto_path."
#     set auto_path ". $dir [lremove $auto_path .]"
      set auto_path "$dir $auto_path"
    }

#    puts "Computing tcltags ($SUFFIX($dir)) for $dir"
    # auto_mkindex seems to change the pwd
    set cwd [pwd]
    # always include *.tcl
    if {[catch "eval auto_mkindex $dir $SUFFIX($dir)" msg]} {
      puts $msg
    }
    cd $cwd
  }

  # make sure all tclIndex's are loaded
  catch perrott

  # Now add in any of SUE's own special auto_path things -- they get
  # wasted when you do an auto_mkindex.
  if {[info exists SUE_auto_index]} {
    foreach cell [array names SUE_auto_index] {
      if {$SUE_auto_index($cell) != [use_first auto_index($cell)]} {
	set auto_index($cell) $SUE_auto_index($cell)
      } else {
	# this is redundant.  Destroy ???
	#unset SUE_auto_index($cell)
      }
    }
  }
}


# This is SUE's own addendum to the auto_index for things that haven't
# been saved yet.

proc add_to_auto_index {schematic filename} {

  global SUE_auto_index auto_index

  # Now put this directory into the auto_index variable
  if {[info commands SCHEMATIC_$schematic] != ""} {
    set SUE_auto_index(SCHEMATIC_$schematic) "source $filename"
    set auto_index(SCHEMATIC_$schematic) "source $filename"
  }
  if {[info commands ICON_$schematic] != ""} {
    set SUE_auto_index(ICON_$schematic) "source $filename"
    set auto_index(ICON_$schematic) "source $filename"
  }
}


proc lremove {list what} {

  while {[set pos [lsearch $list $what]] != -1} {
    set list [lreplace $list $pos $pos]
  }
  return $list
}

# Tries to create the filename field in the SUE data structure
# if no directory is given, uses the directory from the auto_load path
# Tries to keep the schematic and icon (if there are both) filenames
# in sync.

proc make_filename {{dir ""} {suffix ""}} {

  global cur_s auto_index SUFFIX

  upvar #0 SUE_$cur_s data

  set schematic [get_rootname $cur_s]

  if {[string index $dir 0] == "."} {
    set dir [pwd][string range $dir 1 end]
  }

  if {$dir == ""} {
    if {![info exists data(dir)]} {
      set data(dir) ""
    }

    if {[string index $data(dir) 0] == "."} {
      set data(dir) [pwd][string range $data(dir) 1 end]
    }

    if {$data(dir) == ""} {
      # use the auto-load directory if it exists
      if {[info exists auto_index(SCHEMATIC_$schematic)]} {
	set dir [file dirname [lindex $auto_index(SCHEMATIC_$schematic) 1]]/

      } elseif {[info exists auto_index(ICON_$schematic)]} {
	set dir [file dirname [lindex $auto_index(ICON_$schematic) 1]]/

      } else {
	set dir [pwd]/
      }

      if {[string index $dir 0] == "."} {
	set dir [pwd][string range $dir 1 end]
      }
      set data(dir) $dir

    } else {
      set dir $data(dir)
    }
  } else {
    set data(dir) $dir
  }

  if {$suffix == ""} {
    if {[info exists data(suffix)]} {
      set suffix $data(suffix)
    } else {
      set suffix $SUFFIX(default)
      set data(suffix) $suffix
    }
  } else {
    # remove backup extension if it exists
    if {[file extension $suffix] == $SUFFIX(backup)} {
      set suffix [string range $suffix 0 [expr [string length $suffix] - \
					      [string length $SUFFIX(backup)] - 1]]
    }
    set data(suffix) $suffix
  }

  set data(filename) "$dir$schematic$suffix"

  # If there is another view, then change that one also.
  upvar #0 SUE_[corresponding_cell $cur_s] c_data
  if {[info exists c_data(dir)]} {
    set c_data(dir) $data(dir)
    set c_data(suffix) $data(suffix)
    set c_data(filename) $data(filename)
  }

  add_to_auto_index $schematic $data(filename)
}


# displays modified cells in a dialog box and querries the user
# to exit or cancel exit command.

proc modify_exit {} {

  global SUE SUE_DIR

  set modified_cells ""

  foreach cell [array names SUE] {

    global SUE_$cell

    if {[set SUE_${cell}(modified)] != ""} {
      if {[is_icon $cell]} {
	set icon [get_rootname $cell]
	lappend modified_cells "Icon \"$icon\" is modified."
      } else {
	lappend modified_cells "Schematic \"$cell\" is modified."
      }
    }
  }

  if {$modified_cells != ""} {
    set button [tk_dialog_new .modify_exit "Cells Modified" \
		    [join $modified_cells "\n"] \
		    "" 0 {exit} {cancel}]

    if {$button == 1} {
      # user hit the cancel key
      return
    }
  }

  # close any probes that might be around
#  global PROBE_TYPE
#  if {$PROBE_TYPE != ""} {
#    ${PROBE_TYPE}_close_probe
#  }

  puts "Exiting SUE."
  # bye-bye world
  exit
}
