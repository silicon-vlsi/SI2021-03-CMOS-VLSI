#### Procedures for CppSim Library Manager


proc create_run_lib_manager_form {} {

global CPPSIM_HOME CPPSIMSHARED_HOME  \
       TOP_LIBMAN_WINDOW message_libman_log cur_s tcl_platform  \
       LIBMAN_LIB_LISTBOX LIBMAN_MOD_LISTBOX SCHEM_LIB ICON1_LIB \
       ICON2_LIB TOP_CREATE_ENTRY_WINDOW \
       LIBMAN_EXPORT_FOR_DELETE_RUN_FLAG MOD_EXPORT_FOR_DELETE_RUN_FLAG

catch {destroy $TOP_LIBMAN_WINDOW}

toplevel .cppLibMan 
set TOP_LIBMAN_WINDOW .cppLibMan
set TOP_CREATE_ENTRY_WINDOW .cppCreateEntry
set SCHEM_LIB ""
set ICON1_LIB ""
set ICON2_LIB ""
set LIBMAN_EXPORT_FOR_DELETE_RUN_FLAG 0
set MOD_EXPORT_FOR_DELETE_RUN_FLAG 0

set x [expr [winfo screenwidth $TOP_LIBMAN_WINDOW]/2 - [winfo reqwidth $TOP_LIBMAN_WINDOW]/2 \
	    - [winfo vrootx [winfo parent $TOP_LIBMAN_WINDOW]]]
set y [expr [winfo screenheight $TOP_LIBMAN_WINDOW]/2 - [winfo reqheight $TOP_LIBMAN_WINDOW]/2 \
	    - [winfo vrooty [winfo parent $TOP_LIBMAN_WINDOW]]]

wm geometry .cppLibMan "+$x+$y"
wm title .cppLibMan "CppSim Library Manager"
    
bind .cppLibMan <Escape> {catch {destroy .cppLibMan} ; return}

# Create a frame for buttons and entry.
frame .cppLibMan.button_frame -borderwidth 10

pack .cppLibMan.button_frame -side top -fill x
# Create the command buttons.
button .cppLibMan.button_frame.import -text "Import Library Tool" -width 20 -command "create_run_import_form"
button .cppLibMan.button_frame.export -text "Export Library Tool" -width 20 -command "create_run_export_form"
button .cppLibMan.button_frame.close -padx 5 -text "Close" -command cppLibManClose
pack .cppLibMan.button_frame.close  .cppLibMan.button_frame.import .cppLibMan.button_frame.export -side left -padx 2

frame .cppLibMan.sue_lib_frame
label .cppLibMan.sue_lib_frame.label -text "'sue.lib' Operations:" -width 15 -padx 1
button .cppLibMan.sue_lib_frame.add_button -text "Add Library" -width 13 -command create_run_lib_add_to_sue_lib_form
button .cppLibMan.sue_lib_frame.remove_button -text "Remove Library" -width 13 -command cppSueLibRemove
button .cppLibMan.sue_lib_frame.schem_button -text "schematic win." -width 13 -command cppSueLibSchem
button .cppLibMan.sue_lib_frame.icon1_button -text "icon1 win." -width 13 -command cppSueLibIcon1
button .cppLibMan.sue_lib_frame.icon2_button -text "icon2 win." -width 13 -command cppSueLibIcon2

pack .cppLibMan.sue_lib_frame.label -side left -padx 5
pack .cppLibMan.sue_lib_frame.add_button -side left -padx 2
pack .cppLibMan.sue_lib_frame.remove_button -side left -padx 2
pack .cppLibMan.sue_lib_frame.schem_button -side left -padx 2
pack .cppLibMan.sue_lib_frame.icon1_button -side left -padx 2
pack .cppLibMan.sue_lib_frame.icon2_button -side left -padx 2
pack .cppLibMan.sue_lib_frame -side top -fill x -pady 3



frame .cppLibMan.library_frame
label .cppLibMan.library_frame.label -text "Library Operations:" -width 15 -padx 1
button .cppLibMan.library_frame.create_button -text "Create" -width 13 -command cppLibraryCreate
button .cppLibMan.library_frame.rename_button -text "Rename" -width 13 -command cppLibraryRename
button .cppLibMan.library_frame.dependency_button -text "Dependencies" -width 13 -command cppLibraryDependencies
button .cppLibMan.library_frame.delete_button -text "Delete" -width 13 -command cppLibraryDelete

pack .cppLibMan.library_frame.label -side left -padx 5
pack .cppLibMan.library_frame.create_button -side left -padx 2
pack .cppLibMan.library_frame.rename_button -side left -padx 2
pack .cppLibMan.library_frame.dependency_button -side left -padx 2
pack .cppLibMan.library_frame.delete_button -side left -padx 2
pack .cppLibMan.library_frame -side top -fill x -pady 3


frame .cppLibMan.module_frame
label .cppLibMan.module_frame.label -text "Module Operations:" -width 15 -padx 1
button .cppLibMan.module_frame.move_button -text "Move" -width 13 -command cppModuleMove
button .cppLibMan.module_frame.dependency_button -text "Dependencies" -width 13 -command cppModuleDependencies
button .cppLibMan.module_frame.delete_button -text "Delete" -width 13 -command cppModuleDelete

pack .cppLibMan.module_frame.label -side left -padx 5
pack .cppLibMan.module_frame.move_button -side left -padx 2
pack .cppLibMan.module_frame.dependency_button -side left -padx 2
pack .cppLibMan.module_frame.delete_button -side left -padx 2
pack .cppLibMan.module_frame -side top -fill x -pady 3


# Create menu options for sim file

frame .cppLibMan.lib_mod_select_frame -width 60

frame .cppLibMan.lib_mod_select_frame.lib -width 30
label .cppLibMan.lib_mod_select_frame.lib.label -text "Library:    " -width 8 -padx 1 
set LIBMAN_LIB_LISTBOX [listbox .cppLibMan.lib_mod_select_frame.lib.listbox \
   -height 7 -width 33 \
   -yscrollcommand {.cppLibMan.lib_mod_select_frame.lib.yscroll set} \
   -xscrollcommand {.cppLibMan.lib_mod_select_frame.lib.xscroll set}] 

scrollbar .cppLibMan.lib_mod_select_frame.lib.yscroll -command {.cppLibMan.lib_mod_select_frame.lib.listbox yview} -orient vertical -relief sunken
scrollbar .cppLibMan.lib_mod_select_frame.lib.xscroll -command {.cppLibMan.lib_mod_select_frame.lib.listbox xview} -orient horizontal -relief sunken 

pack .cppLibMan.lib_mod_select_frame.lib.label -side left -anchor nw 
pack .cppLibMan.lib_mod_select_frame.lib.xscroll -side bottom -fill x
pack .cppLibMan.lib_mod_select_frame.lib.yscroll -side right -fill y
pack .cppLibMan.lib_mod_select_frame.lib.listbox -side left -fill both -expand true


frame .cppLibMan.lib_mod_select_frame.mod -width 30
label .cppLibMan.lib_mod_select_frame.mod.label -text "Module:" -width 10 -padx 1 
set LIBMAN_MOD_LISTBOX [listbox .cppLibMan.lib_mod_select_frame.mod.listbox \
   -height 7 -width 33 \
   -yscrollcommand {.cppLibMan.lib_mod_select_frame.mod.yscroll set} \
   -xscrollcommand {.cppLibMan.lib_mod_select_frame.mod.xscroll set}] 

scrollbar .cppLibMan.lib_mod_select_frame.mod.yscroll -command {.cppLibMan.lib_mod_select_frame.mod.listbox yview} -orient vertical -relief sunken
scrollbar .cppLibMan.lib_mod_select_frame.mod.xscroll -command {.cppLibMan.lib_mod_select_frame.mod.listbox xview} -orient horizontal -relief sunken 

pack .cppLibMan.lib_mod_select_frame.mod.label -side left -anchor nw 
pack .cppLibMan.lib_mod_select_frame.mod.xscroll -side bottom -fill x
pack .cppLibMan.lib_mod_select_frame.mod.yscroll -side right -fill y
pack .cppLibMan.lib_mod_select_frame.mod.listbox -side left -fill both -expand true

pack .cppLibMan.lib_mod_select_frame.lib -side left
pack .cppLibMan.lib_mod_select_frame.mod -side left


pack .cppLibMan.lib_mod_select_frame -side top -fill both -expand true -pady 5 -padx 5

bind $LIBMAN_LIB_LISTBOX <ButtonRelease-1> "cppUpdateModules"
focus $LIBMAN_LIB_LISTBOX

# Create a text widget to log the output
frame .cppLibMan.message_frame -pady 10
label .cppLibMan.message_frame.label -text "Result:     " -width 8 -padx 1
set message_libman_log [text .cppLibMan.message_frame.message -width 80 -height 14 \
-borderwidth 2 -relief raised -setgrid true \
-yscrollcommand {.cppLibMan.message_frame.yscroll set} \
-xscrollcommand {.cppLibMan.message_frame.xscroll set}]
scrollbar .cppLibMan.message_frame.yscroll -command {.cppLibMan.message_frame.message yview} -orient vertical -relief sunken
scrollbar .cppLibMan.message_frame.xscroll -command {.cppLibMan.message_frame.message xview} -orient horizontal -relief sunken

pack .cppLibMan.message_frame.label -anchor nw -side left
pack .cppLibMan.message_frame.xscroll -side bottom -fill x
pack .cppLibMan.message_frame.yscroll -side right -fill y
pack .cppLibMan.message_frame.message -side left -fill both -expand true
pack .cppLibMan.message_frame -side top -fill both -expand true -padx 5

update_sue_lib_file -add ""
cppLibManUpdateLibraries

$message_libman_log delete 1.0 end
$message_libman_log insert end "\n-------  NOTE:  YOU WILL NEED TO RESTART SUE2 ONCE YOU ARE  -------\n"
$message_libman_log insert end   "-------         FINISHED WITH LIBRARY MANAGER OPERATIONS    -------\n"
$message_libman_log see end

}


proc cppLibraryDependencies {} {
global CPPSIM_HOME CPPSIMSHARED_HOME tcl_platform message_libman_log

    set lib_name [cppLibManGetCurLib]
    if {$lib_name == ""} {
       tk_dialog_new .cppsim_message "Error:  Need to Select Library" \
	        "Error: you need to select a library\nin the 'Library:' listbox" "" 0 {OK} 
      return
    }

    set first_sue_file [cpp_first_cell_in_library_libman $lib_name]
    if {$first_sue_file != ""} {
       if {$tcl_platform(platform) == "windows"} {
         set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin/win32"
       } else {
         set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin"
       }
       set sue_lib_file $CPPSIM_HOME/Sue2/sue.lib
       $message_libman_log delete 1.0 end
       $message_libman_log insert end "\n---- Determining dependency of modules on library '$lib_name' ----\n"
       $message_libman_log see end

       catch {open "|$sue2_bin_directory/check_dependencies_or_name_clashes $sue_lib_file $first_sue_file all dependencies verbose |& cat"} fid
       set message_input [read $fid]
       $message_libman_log insert end $message_input
       $message_libman_log see end

       if {[catch {close $fid} err] != 0} {
          $message_libman_log insert end "\n**** Removal of this library will impact other libraries ****\n"
          $message_libman_log see end
          return
       } else {
          $message_libman_log insert end "\n**** Removal of this library will have no impact on other libraries ****\n"
          $message_libman_log see end
          return
       } 
    }
}


proc cppModuleDependencies {} {
global CPPSIM_HOME CPPSIMSHARED_HOME tcl_platform message_libman_log

    set mod_name [cppLibManGetCurMod]
    if {$mod_name == ""} {
       tk_dialog_new .cppsim_message "Error:  Need to Select Module" \
	        "Error: you need to select a module\nin the 'Module:' listbox" "" 0 {OK} 
      return
    } else {
       if {$tcl_platform(platform) == "windows"} {
         set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin/win32"
       } else {
         set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin"
       }
       set sue_lib_file $CPPSIM_HOME/Sue2/sue.lib
       $message_libman_log delete 1.0 end
       $message_libman_log insert end "\n---- Determining dependency of other modules on module '$mod_name' ----\n"
       $message_libman_log see end

       catch {open "|$sue2_bin_directory/check_dependencies_or_name_clashes $sue_lib_file $mod_name single dependencies verbose |& cat"} fid
       set message_input [read $fid]
       $message_libman_log insert end $message_input
       $message_libman_log see end
       if {[catch {close $fid} err] != 0} {
          $message_libman_log insert end "\n**** Removal of this module will impact other modules ****\n"
          $message_libman_log see end
          return
       } else {
          $message_libman_log insert end "\n**** Removal of this module will have no impact on other modules ****\n"
          $message_libman_log see end
          return
       } 
    }
}


proc cppSueLibRemove {} {
global CPPSIM_HOME CPPSIMSHARED_HOME message_libman_log

set lib_name [cppLibManGetCurLib]
if {$lib_name == ""} {
   tk_dialog_new .cppsim_message "Error:  Need to Select Library" \
	        "Error: you need to select a library\nin the 'Library:' listbox" "" 0 {OK} 
   return
}
$message_libman_log delete 1.0 end
$message_libman_log insert end "\n------------------ Note: BEFORE removing library '$lib_name' --------------------\n"
$message_libman_log insert end "\n   Consider examining the dependency of other library modules on this library\n"
$message_libman_log insert end "\n   by clicking on the 'Dependencies' button in the 'Library Operations:' section\n"
$message_libman_log see end

set verify_flag [tk_dialog_new .cppsim_message "Verification of Remove Operation" \
		  "Are you sure that you want to remove library\n'$lib_name' from the sue.lib file?" "" 1 {Yes} {Cancel}]

if {$verify_flag == 0} {
    update_sue_lib_file -remove $lib_name

    if {[winfo exists .cppLibManAdd]} {
        cppLibManUpdateAddLibraries
    }
   $message_libman_log insert end "\n*****  Removal of library '$lib_name' from sue.lib file completed  *****\n"
   $message_libman_log see end

}

}

proc update_sue_lib_file {control_string add_or_remove_lib} {
global SCHEM_LIB ICON1_LIB ICON2_LIB CPPSIM_HOME CPPSIMSHARED_HOME


set sue_lib_filename $CPPSIM_HOME/Sue2/sue.lib
if {$control_string == "-remove"} {
   set remove_lib $add_or_remove_lib
   set add_lib ""
} else {
   set remove_lib ""
   set add_lib $add_or_remove_lib
}

catch {open "$sue_lib_filename"} fid
set add_flag 0
set sue_lib_list ""
while {[gets $fid line] >= 0} {
   if {$line == $add_lib} {
       set add_flag 1
   }
   set num_entries [llength $sue_lib_list]
   set cur_index 0
   while {$cur_index < $num_entries} {
      set cur_entry [lindex $sue_lib_list $cur_index]
      if {$cur_entry == $line} {
	  break
      }
      set cur_index [expr $cur_index + 1]
   }
   if {$cur_index == $num_entries} {
       set sue_lib_list [concat $sue_lib_list $line]
   }
}
catch {close $fid} err
if {$add_lib != "" && $add_flag == 0} {
   set sue_lib_list [concat $sue_lib_list $add_lib]
}
set num_sue_lib_entries [llength $sue_lib_list]

set cur_index 0
set schem_flag 0
set icon1_flag 0
set icon2_flag 0

if {$SCHEM_LIB == $ICON1_LIB} {
   set ICON1_LIB ""
}
if {$SCHEM_LIB == $ICON2_LIB} {
   set ICON2_LIB ""
}
if {$ICON1_LIB == $ICON2_LIB} {
   set ICON2_LIB ""
}

while {$cur_index < $num_sue_lib_entries} {
   set cur_entry [lindex $sue_lib_list $cur_index]
   if {$cur_entry == $SCHEM_LIB} {
       set schem_flag 1
   } elseif {$cur_entry == $ICON1_LIB} {
       set icon1_flag 1
   } elseif {$cur_entry == $ICON2_LIB} {
       set icon2_flag 1
   }
   set cur_index [expr $cur_index + 1]
}

catch {open "$sue_lib_filename" w} fid

if {$schem_flag == 0} {
   if {$num_sue_lib_entries > 0} {
       set SCHEM_LIB [lindex $sue_lib_list 0]
       if {$remove_lib != $SCHEM_LIB} {
          puts $fid $SCHEM_LIB
       }
   } else {
       set SCHEM_LIB ""
   }
} else {
   if {$remove_lib != $SCHEM_LIB} {
      puts $fid $SCHEM_LIB
   }
}
if {$icon1_flag == 0} {
   if {$num_sue_lib_entries > 1} {
       set ICON1_LIB [lindex $sue_lib_list 1]
       if {$remove_lib != $ICON1_LIB} {
           puts $fid $ICON1_LIB
       }
   } else {
       set ICON1_LIB ""
   }
} else {
    if {$remove_lib != $ICON1_LIB} {
       puts $fid $ICON1_LIB
    }
}
if {$icon2_flag == 0} {
   if {$num_sue_lib_entries > 2} {
       set ICON2_LIB [lindex $sue_lib_list 2]
       if {$remove_lib != $ICON2_LIB} {
          puts $fid $ICON2_LIB
       }
   } else {
       set ICON2_LIB ""
   }
} else {
   if {$remove_lib != $ICON2_LIB} {
      puts $fid $ICON2_LIB
   }
}

set sue_lib_list [lsort -dictionary $sue_lib_list]
set cur_index 0
while {$cur_index < $num_sue_lib_entries} {
   set cur_entry [lindex $sue_lib_list $cur_index]
   if {$cur_entry != $SCHEM_LIB} {
      if {$cur_entry != $ICON1_LIB} {
	  if {$cur_entry != $ICON2_LIB} {
	     if {$cur_entry != $remove_lib} {
                 puts $fid $cur_entry
	     }
	  }
      }
   }
   set cur_index [expr $cur_index + 1]
}
catch {close $fid} err
cppLibManUpdateLibraries
}

proc cppSueLibSchem {} {
global CPPSIM_HOME CPPSIMSHARED_HOME message_libman_log SCHEM_LIB 

set lib_name [cppLibManGetCurLib]
if {$lib_name == ""} {
   tk_dialog_new .cppsim_message "Error:  Need to Select Library" \
	        "Error: you need to select a library\nin the 'Library:' listbox" "" 0 {OK} 
   return
}
$message_libman_log delete 1.0 end
$message_libman_log insert end "\n------------------ Note:  --------------------\n"
$message_libman_log insert end "\n   Pushing this button places the associated library\n"
$message_libman_log insert end "\n   at the first line of the sue.lib file, which\n"
$message_libman_log insert end "\n   then causes this library to become the default library\n"
$message_libman_log insert end "\n   that appears in the 'schematic window' when Sue2 is started\n"
$message_libman_log see end

set verify_flag [tk_dialog_new .cppsim_message "Verification of 'schematic window' Operation" \
		  "Are you sure that you want to select library\n'$lib_name' to be the\ndefault 'schematic window' library?" "" 1 {Yes} {Cancel}]

if {$verify_flag == 0} {
    set SCHEM_LIB $lib_name
    update_sue_lib_file -add ""
}

}


proc cppSueLibIcon1 {} {
global CPPSIM_HOME CPPSIMSHARED_HOME message_libman_log ICON1_LIB

set lib_name [cppLibManGetCurLib]
if {$lib_name == ""} {
   tk_dialog_new .cppsim_message "Error:  Need to Select Library" \
	        "Error: you need to select a library\nin the 'Library:' listbox" "" 0 {OK} 
   return
}
$message_libman_log delete 1.0 end
$message_libman_log insert end "\n------------------ Note:  --------------------\n"
$message_libman_log insert end "\n   Pushing this button places the associated library\n"
$message_libman_log insert end "\n   at the second line of the sue.lib file, which\n"
$message_libman_log insert end "\n   then causes this library to become the default library\n"
$message_libman_log insert end "\n   that appears in the 'icon1 window' when Sue2 is started\n"
$message_libman_log see end

set verify_flag [tk_dialog_new .cppsim_message "Verification of 'icon1 window' Operation" \
		  "Are you sure that you want to select library\n'$lib_name' to be the\ndefault 'icon1 window' library?" "" 1 {Yes} {Cancel}]

if {$verify_flag == 0} {
    set ICON1_LIB $lib_name
    update_sue_lib_file -add ""
}

}

proc cppSueLibIcon2 {} {
global CPPSIM_HOME CPPSIMSHARED_HOME message_libman_log ICON2_LIB

set lib_name [cppLibManGetCurLib]
if {$lib_name == ""} {
   tk_dialog_new .cppsim_message "Error:  Need to Select Library" \
	        "Error: you need to select a library\nin the 'Library:' listbox" "" 0 {OK} 
   return
}
$message_libman_log delete 1.0 end
$message_libman_log insert end "\n------------------ Note:  --------------------\n"
$message_libman_log insert end "\n   Pushing this button places the associated library\n"
$message_libman_log insert end "\n   at the third line of the sue.lib file, which\n"
$message_libman_log insert end "\n   then causes this library to become the default library\n"
$message_libman_log insert end "\n   that appears in the 'icon2 window' when Sue2 is started\n"
$message_libman_log see end

set verify_flag [tk_dialog_new .cppsim_message "Verification of 'icon2 window' Operation" \
		  "Are you sure that you want to select library\n'$lib_name' to be the\ndefault 'icon2 window' library?" "" 1 {Yes} {Cancel}]

if {$verify_flag == 0} {
    set ICON2_LIB $lib_name
    update_sue_lib_file -add ""
}

}



proc cppLibManGetCurLibDir {} {
global LIBMAN_LIB_LISTBOX CPPSIM_HOME CPPSIMSHARED_HOME

set cur_index [$LIBMAN_LIB_LISTBOX curselection]
if {$cur_index != ""} {
    set cur_library [$LIBMAN_LIB_LISTBOX get $cur_index]
    set base_index [string first :Private $cur_library]
    if {$base_index != -1} {
	set cur_library [string range $cur_library 0 [expr $base_index - 1]]
        set cur_lib_dir $CPPSIM_HOME/SueLib/$cur_library
    } else {
        if {[file isdirectory $CPPSIM_HOME/SueLib/$cur_library]} {
           set cur_lib_dir $CPPSIM_HOME/SueLib/$cur_library
        } else {
           set cur_lib_dir $CPPSIMSHARED_HOME/SueLib/$cur_library
	}
    }
return $cur_lib_dir
} else {
return ""
}
}

proc cppLibManGetCurLib {} {
global LIBMAN_LIB_LISTBOX

set cur_index [$LIBMAN_LIB_LISTBOX curselection]
if {$cur_index != ""} {
    set cur_library [$LIBMAN_LIB_LISTBOX get $cur_index]
    set base_index [string first :Private $cur_library]
    if {$base_index != -1} {
	set cur_library [string range $cur_library 0 [expr $base_index - 1]]
    } 
return $cur_library
} else {
return ""
}
}

proc cppLibManGetCurMod {} {
global LIBMAN_MOD_LISTBOX

set cur_index [$LIBMAN_MOD_LISTBOX curselection]
if {$cur_index != ""} {
    set cur_mod [$LIBMAN_MOD_LISTBOX get $cur_index]
    return $cur_mod
} else {
    return ""
}
}


proc cppUpdateModules {} {
global LIBMAN_MOD_LISTBOX message_libman_log CUR_LIBRARY_FOR_MOD \
       CUR_LIBRARY_DIR_FOR_MOD

    set cur_lib_dir [cppLibManGetCurLibDir]
    if {$cur_lib_dir == ""} {
       set cur_lib_dir $CUR_LIBRARY_DIR_FOR_MOD
    } else {
       set CUR_LIBRARY_DIR_FOR_MOD $cur_lib_dir
    }
    set cur_lib_name [cppLibManGetCurLib]
    if {$cur_lib_name == ""} {
        set cur_lib_name $CUR_LIBRARY_FOR_MOD
    } else {
        set CUR_LIBRARY_FOR_MOD $cur_lib_name
    }

    $LIBMAN_MOD_LISTBOX delete 0 end
    set module_list [glob -nocomplain -tails -directory $cur_lib_dir -types f *.sue]
    set num_files [llength $module_list]
    set cur_index 0
    while {$cur_index < $num_files} {
        set cur_entry_raw [lindex $module_list $cur_index]
        set base_index [string first .sue $cur_entry_raw]
	set cur_entry [string range $cur_entry_raw 0 [expr $base_index - 1]]
        $LIBMAN_MOD_LISTBOX insert end $cur_entry
	set cur_index [expr $cur_index + 1]
    }

#    set cur_lib [cppLibManGetCurLib]
#    $message_libman_log delete 1.0 end
#    $message_libman_log insert end "\nDirectory:  '$cur_lib_dir'\n"
#    $message_libman_log see end

}


# CppSim Fill In Library Entries
proc cppLibManUpdateLibraries {} {

global CPPSIM_HOME CPPSIMSHARED_HOME LIBMAN_LIB_LISTBOX

$LIBMAN_LIB_LISTBOX delete 0 end
set sue_lib_filename $CPPSIM_HOME/Sue2/sue.lib
set sue_lib_list ""

catch {open "$sue_lib_filename"} fid
set cur_index 0
while {[gets $fid line] >= 0} {
   set dir_name [cpp_libman_public_or_private_library $line] 
   if {$dir_name != ""} {
       set sue_lib_list [concat $sue_lib_list $line] 
       $LIBMAN_LIB_LISTBOX insert end $dir_name
       set cur_index [expr $cur_index + 1]
   }
}
catch {close $fid} err
if {$cur_index > 0} {
    set cur_index 0
    $LIBMAN_LIB_LISTBOX selection set $cur_index
    $LIBMAN_LIB_LISTBOX activate $cur_index
}

catch {open "$sue_lib_filename" w} fid
set num_sue_lib_entries [llength $sue_lib_list]
set cur_index 0
while {$cur_index < $num_sue_lib_entries} {
   set cur_entry [lindex $sue_lib_list $cur_index]
   puts $fid $cur_entry
   set cur_index [expr $cur_index + 1]
}
catch {close $fid} err

cppUpdateModules
}


proc cpp_first_cell_in_library_libman {lib_name} {

global CPPSIM_HOME CPPSIMSHARED_HOME message_libman_log

set private_suelib_dir $CPPSIM_HOME/SueLib
set shared_suelib_dir $CPPSIMSHARED_HOME/SueLib

set private_dir_exists [file isdirectory $private_suelib_dir/$lib_name]

set shared_dir_exists [file isdirectory $shared_suelib_dir/$lib_name]

if {$private_dir_exists} {
   set file_list [glob -nocomplain -tails -directory $private_suelib_dir/$lib_name *.sue]
   set num_files [llength $file_list]
   if {$num_files < 1} {
       if {$shared_dir_exists} {
           $message_libman_log insert end "\n------------------ Error: No .sue files in library '$lib_name:Private' --------------------\n"
       } else {
           $message_libman_log insert end "\n------------------ Error: No .sue files in library '$lib_name' --------------------\n"
       }
      $message_libman_log see end
      return ""
   }
   set first_sue_file [lindex $file_list 0]
   return $first_sue_file
} else {
   if {$shared_dir_exists} {
      set file_list [glob -nocomplain -tails -directory $shared_suelib_dir/$lib_name *.sue]
      set num_files [llength $file_list]
      if {$num_files < 1} {
         $message_libman_log insert end "\n------------------ Error: No .sue files in library '$lib_name' --------------------\n"
         $message_libman_log see end
         return ""
      }
      set first_sue_file [lindex $file_list 0]
      return $first_sue_file

   } else {
      $message_libman_log insert end "\n------------------ Error: Can't locate library '$lib_name' in the file system --------------------\n"
      $message_libman_log see end
      return ""
   }
}
}




#########################################################################################################


# Kill current CppSim run
proc cppLibManClose {} {

global TOP_LIBMAN_WINDOW

catch {destroy $TOP_LIBMAN_WINDOW}
}


proc cpp_libman_public_or_private_library {lib_name} {

global CPPSIM_HOME CPPSIMSHARED_HOME

set private_suelib_dir $CPPSIM_HOME/SueLib
set shared_suelib_dir $CPPSIMSHARED_HOME/SueLib

set private_dir_exists [file isdirectory $private_suelib_dir/$lib_name]

set shared_dir_exists [file isdirectory $shared_suelib_dir/$lib_name]

if {$private_dir_exists} {
   if {$shared_dir_exists} {
      return "$lib_name:Private"
   } else {
       return $lib_name
   }
} else {
   if {$shared_dir_exists} {
      return "$lib_name"
   } else {
      return ""
   }
}
}



proc create_run_lib_add_to_sue_lib_form {} {

global CPPSIM_HOME CPPSIMSHARED_HOME TOP_LIBMAN_ADD_WINDOW \
       LIBMAN_ADD_PLIB_LISTBOX LIBMAN_ADD_SLIB_LISTBOX \
       CUR_SLIB_NAME CUR_PLIB_NAME message_libman_add_log \
       LIBMAN_EXPORT_RUN_FLAG

catch {destroy $TOP_LIBMAN_ADD_WINDOW}

toplevel .cppLibManAdd
set TOP_LIBMAN_ADD_WINDOW .cppLibManAdd

set x [expr [winfo screenwidth $TOP_LIBMAN_ADD_WINDOW]/2 - [winfo reqwidth $TOP_LIBMAN_ADD_WINDOW]/2 \
	    - [winfo vrootx [winfo parent $TOP_LIBMAN_ADD_WINDOW]]]
set y [expr [winfo screenheight $TOP_LIBMAN_ADD_WINDOW]/2 - [winfo reqheight $TOP_LIBMAN_ADD_WINDOW]/2 \
	    - [winfo vrooty [winfo parent $TOP_LIBMAN_ADD_WINDOW]]]

wm geometry .cppLibManAdd "+$x+$y"
wm title .cppLibManAdd "Add Library to 'sue.lib' File"
    
bind .cppLibManAdd <Escape> {catch {destroy .cppLibManAdd} ; return}

set CUR_SLIB_NAME ""
set CUR_PLIB_NAME ""
set LIBMAN_EXPORT_RUN_FLAG 0

# Create a frame for buttons and entry.
frame .cppLibManAdd.button_frame -borderwidth 10

pack .cppLibManAdd.button_frame -side top -fill x
# Create the command buttons.
button .cppLibManAdd.button_frame.add -text "Add Library" -width 20 -command "cpp_add_to_sue_lib"
button .cppLibManAdd.button_frame.export -text "Export Library Modules" -width 20 -command "cpp_export_libman_library"
button .cppLibManAdd.button_frame.close -padx 5 -text "Close" -command cppLibManAddClose
pack .cppLibManAdd.button_frame.close  .cppLibManAdd.button_frame.add .cppLibManAdd.button_frame.export -side left -padx 2




frame .cppLibManAdd.lib_mod_select_frame -width 60

frame .cppLibManAdd.lib_mod_select_frame.lib -width 30
label .cppLibManAdd.lib_mod_select_frame.lib.label -text "Private:    " -width 8 -padx 1 
set LIBMAN_ADD_PLIB_LISTBOX [listbox .cppLibManAdd.lib_mod_select_frame.lib.listbox \
   -height 7 -width 33 \
   -yscrollcommand {.cppLibManAdd.lib_mod_select_frame.lib.yscroll set} \
   -xscrollcommand {.cppLibManAdd.lib_mod_select_frame.lib.xscroll set}] 

scrollbar .cppLibManAdd.lib_mod_select_frame.lib.yscroll -command {.cppLibManAdd.lib_mod_select_frame.lib.listbox yview} -orient vertical -relief sunken
scrollbar .cppLibManAdd.lib_mod_select_frame.lib.xscroll -command {.cppLibManAdd.lib_mod_select_frame.lib.listbox xview} -orient horizontal -relief sunken 

pack .cppLibManAdd.lib_mod_select_frame.lib.label -side left -anchor nw 
pack .cppLibManAdd.lib_mod_select_frame.lib.xscroll -side bottom -fill x
pack .cppLibManAdd.lib_mod_select_frame.lib.yscroll -side right -fill y
pack .cppLibManAdd.lib_mod_select_frame.lib.listbox -side left -fill both -expand true


frame .cppLibManAdd.lib_mod_select_frame.mod -width 30
label .cppLibManAdd.lib_mod_select_frame.mod.label -text "Shared:" -width 10 -padx 1 
set LIBMAN_ADD_SLIB_LISTBOX [listbox .cppLibManAdd.lib_mod_select_frame.mod.listbox \
   -height 7 -width 33 \
   -yscrollcommand {.cppLibManAdd.lib_mod_select_frame.mod.yscroll set} \
   -xscrollcommand {.cppLibManAdd.lib_mod_select_frame.mod.xscroll set}] 

scrollbar .cppLibManAdd.lib_mod_select_frame.mod.yscroll -command {.cppLibManAdd.lib_mod_select_frame.mod.listbox yview} -orient vertical -relief sunken
scrollbar .cppLibManAdd.lib_mod_select_frame.mod.xscroll -command {.cppLibManAdd.lib_mod_select_frame.mod.listbox xview} -orient horizontal -relief sunken 

pack .cppLibManAdd.lib_mod_select_frame.mod.label -side left -anchor nw 
pack .cppLibManAdd.lib_mod_select_frame.mod.xscroll -side bottom -fill x
pack .cppLibManAdd.lib_mod_select_frame.mod.yscroll -side right -fill y
pack .cppLibManAdd.lib_mod_select_frame.mod.listbox -side left -fill both -expand true

pack .cppLibManAdd.lib_mod_select_frame.lib -side left
pack .cppLibManAdd.lib_mod_select_frame.mod -side left


pack .cppLibManAdd.lib_mod_select_frame -side top -fill both -expand true -pady 5 -padx 5

bind $LIBMAN_ADD_SLIB_LISTBOX <ButtonRelease-1> "cppUpdateSLIB"
bind $LIBMAN_ADD_PLIB_LISTBOX <ButtonRelease-1> "cppUpdatePLIB"
focus $LIBMAN_ADD_PLIB_LISTBOX

frame .cppLibManAdd.text_frame -width 60

label .cppLibManAdd.text_frame.label1 -text "----------------------------------------------------------------- Note: -----------------------------------------------------------------" -padx 1 
label .cppLibManAdd.text_frame.label2 -text "Private libraries are in '$CPPSIM_HOME/SueLib'" -padx 1 
label .cppLibManAdd.text_frame.label3 -text "Shared libraries are in '$CPPSIMSHARED_HOME/SueLib'" -padx 1 

pack .cppLibManAdd.text_frame.label1 -side top -pady 1
pack .cppLibManAdd.text_frame.label2 -side top -pady 1
pack .cppLibManAdd.text_frame.label3 -side top -pady 1
pack .cppLibManAdd.text_frame -side top -fill both -expand true -pady 5 -padx 5

# Create a text widget to log the output
frame .cppLibManAdd.message_frame -pady 10
label .cppLibManAdd.message_frame.label -text "Result:     " -width 8 -padx 1
set message_libman_add_log [text .cppLibManAdd.message_frame.message -width 80 -height 14 \
-borderwidth 2 -relief raised -setgrid true \
-yscrollcommand {.cppLibManAdd.message_frame.yscroll set} \
-xscrollcommand {.cppLibManAdd.message_frame.xscroll set}]
scrollbar .cppLibManAdd.message_frame.yscroll -command {.cppLibManAdd.message_frame.message yview} -orient vertical -relief sunken
scrollbar .cppLibManAdd.message_frame.xscroll -command {.cppLibManAdd.message_frame.message xview} -orient horizontal -relief sunken

pack .cppLibManAdd.message_frame.label -anchor nw -side left
pack .cppLibManAdd.message_frame.xscroll -side bottom -fill x
pack .cppLibManAdd.message_frame.yscroll -side right -fill y
pack .cppLibManAdd.message_frame.message -side left -fill both -expand true
pack .cppLibManAdd.message_frame -side top -fill both -expand true -padx 5

cppLibManUpdateAddLibraries
}

proc cpp_export_libman_library {} {

global message_libman_add_log CPPSIMSHARED_HOME CPPSIM_HOME tcl_platform \
       LIBMAN_EXPORT_RUN_FLAG CUR_SLIB_NAME CUR_PLIB_NAME \

if {$LIBMAN_EXPORT_RUN_FLAG == 1} {
    return
}
set LIBMAN_EXPORT_RUN_FLAG 1

if {$CUR_SLIB_NAME == "" && $CUR_PLIB_NAME == ""} {
   tk_dialog_new .cppsim_message "Error:  Need to Select Library" \
	        "Error: you need to select a library\nin one of the listboxes" "" 0 {OK} 
   set LIBMAN_EXPORT_RUN_FLAG 0
   return
}

set change_private_lib_flag 0
if {$CUR_SLIB_NAME != ""} {
    set export_library $CUR_SLIB_NAME
    set private_dir $CPPSIM_HOME/SueLib/$CUR_SLIB_NAME
    if {[file isdirectory $private_dir]} {
       set change_private_lib_flag 1
       file rename -force $private_dir $private_dir\__Private__
    }
} elseif {$CUR_PLIB_NAME != ""} {
    set export_library $CUR_PLIB_NAME

}

set export_destination_file [string tolower $export_library]
set first_sue_file [cpp_first_cell_in_library_libman_add $export_library]
if {$first_sue_file == ""} {
    if {$change_private_lib_flag == 1} {
       file rename -force $private_dir\__Private__ $private_dir
    }
    set LIBMAN_EXPORT_RUN_FLAG 0
    $message_libman_add_log delete 1.0 end
    $message_libman_add_log insert end "\n------------------ 'Export Library Modules' Exited with Errors --------------------\n"
    $message_libman_add_log see end

    return
}

set sue_lib_filename $CPPSIM_HOME/Sue2/sue.lib_temp_
catch {open "$sue_lib_filename" w} fid
puts $fid $export_library
catch {close $fid} err

$message_libman_add_log delete 1.0 end
$message_libman_add_log insert end "\n----------------- Running Export of Library '$export_library' -----------------\n\n"
$message_libman_add_log see end

if {$tcl_platform(platform) == "windows"} {
   set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin/win32"
} else {
   set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin"
}


catch {open "|$sue2_bin_directory/export_cppsim_library $first_sue_file $sue_lib_filename all lite |& cat"} fid
set message_input [read $fid]
$message_libman_add_log insert end $message_input
$message_libman_add_log see end

if {[catch {close $fid} err] != 0} {
    $message_libman_add_log insert end "\n------------------ Exporting Exited with Errors --------------------\n"
    $message_libman_add_log see end

    if {$change_private_lib_flag == 1} {
       file rename -force $private_dir\__Private__ $private_dir
    }
    set LIBMAN_EXPORT_RUN_FLAG 0
    return
}

if {$change_private_lib_flag == 1} {
    file rename -force $private_dir\__Private__ $private_dir
}
 

if {$tcl_platform(platform) == "windows"} {
   set tar_bin_file "$CPPSIMSHARED_HOME/Msys/bin/tar"
} else {
   set tar_bin_file "tar"
}

set cur_directory [pwd]
cd $CPPSIM_HOME/Import_Export

$message_libman_add_log insert end "\n... Creating file '$export_destination_file.tar.gz ...\n\n"
$message_libman_add_log see end

catch {open "|$tar_bin_file cfz $export_destination_file.tar.gz ./SueLib ./CadenceLib ./SimRuns  |& cat"} fid
set message_input [read $fid]
$message_libman_add_log insert end $message_input
$message_libman_add_log see end

cd $cur_directory

if {[catch {close $fid} err] != 0} {
      $message_libman_add_log insert end "\n------------------ Exporting Exited with Errors --------------------\n"
      $message_libman_add_log see end
} else {
      $message_libman_add_log insert end "\n------------------ File '$export_destination_file.tar.gz' was successfully created --------------------\n"
      $message_libman_add_log insert end "------------------ in directory '$CPPSIM_HOME/Import_Export' --------------------\n"
      $message_libman_add_log see end
} 
set LIBMAN_EXPORT_RUN_FLAG 0
return
}



proc cpp_export_libman_library_for_delete {lib_name} {

global message_libman_log CPPSIMSHARED_HOME CPPSIM_HOME tcl_platform \
       LIBMAN_EXPORT_FOR_DELETE_RUN_FLAG

if {$LIBMAN_EXPORT_FOR_DELETE_RUN_FLAG == 1} {
    return
}
set LIBMAN_EXPORT_FOR_DELETE_RUN_FLAG 1

set export_destination_file [string tolower $lib_name]


set first_sue_file [cpp_first_cell_in_library_libman $lib_name]
if {$first_sue_file == ""} {
    set LIBMAN_EXPORT_FOR_DELETE_RUN_FLAG 0
    $message_libman_log insert end "\n-------------- Note:  directory '$lib_name' is empty, so no need to export its contents  ----------------\n"
    $message_libman_log see end
    return
}

$message_libman_log insert end "\n... First exporting '$lib_name' to: \n"
$message_libman_log insert end "\n    '$CPPSIM_HOME/Import_Export/$export_destination_file.tar.gz' ...\n"
$message_libman_log see end


set sue_lib_filename $CPPSIM_HOME/Sue2/sue.lib_temp_
catch {open "$sue_lib_filename" w} fid
puts $fid $lib_name
catch {close $fid} err

#$message_libman_log delete 1.0 end
$message_libman_log insert end "\n----------------- Running Export of Library '$lib_name' -----------------\n\n"
$message_libman_log see end

if {$tcl_platform(platform) == "windows"} {
   set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin/win32"
} else {
   set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin"
}


catch {open "|$sue2_bin_directory/export_cppsim_library $first_sue_file $sue_lib_filename all lite |& cat"} fid
set message_input [read $fid]
$message_libman_log insert end $message_input
$message_libman_log see end

if {[catch {close $fid} err] != 0} {
    $message_libman_log insert end "\n------------------ Exporting Exited with Errors --------------------\n"
    $message_libman_log see end
    set LIBMAN_EXPORT_FOR_DELETE_RUN_FLAG 0
    return
}

if {$tcl_platform(platform) == "windows"} {
   set tar_bin_file "$CPPSIMSHARED_HOME/Msys/bin/tar"
} else {
   set tar_bin_file "tar"
}

set cur_directory [pwd]
cd $CPPSIM_HOME/Import_Export

$message_libman_log insert end "\n... Creating file '$export_destination_file.tar.gz ...\n\n"
$message_libman_log see end

catch {open "|$tar_bin_file cfz $export_destination_file.tar.gz ./SueLib ./CadenceLib ./SimRuns  |& cat"} fid
set message_input [read $fid]
$message_libman_log insert end $message_input
$message_libman_log see end

cd $cur_directory

if {[catch {close $fid} err] != 0} {
      $message_libman_log insert end "\n------------------ Exporting Exited with Errors --------------------\n"
      $message_libman_log see end
} else {
      $message_libman_log insert end "\n------------------ File '$export_destination_file.tar.gz' was successfully created --------------------\n"
      $message_libman_log insert end "------------------ in directory '$CPPSIM_HOME/Import_Export' --------------------\n"
      $message_libman_log see end
} 
set LIBMAN_EXPORT_FOR_DELETE_RUN_FLAG 0
return
}


proc cpp_export_libman_module_for_delete {lib_name mod_name} {

global message_libman_log CPPSIMSHARED_HOME CPPSIM_HOME tcl_platform \
       MOD_EXPORT_FOR_DELETE_RUN_FLAG

if {$MOD_EXPORT_FOR_DELETE_RUN_FLAG == 1} {
    return
}
set MOD_EXPORT_FOR_DELETE_RUN_FLAG 1

set export_destination_file [string tolower $mod_name]



$message_libman_log insert end "\n... First exporting '$mod_name' to: \n"
$message_libman_log insert end "\n    '$CPPSIM_HOME/Import_Export/$export_destination_file.tar.gz' ...\n"
$message_libman_log see end


set sue_lib_filename $CPPSIM_HOME/Sue2/sue.lib_temp_
catch {open "$sue_lib_filename" w} fid
puts $fid $lib_name
catch {close $fid} err

#$message_libman_log delete 1.0 end
$message_libman_log insert end "\n----------------- Running Export of Module '$mod_name' -----------------\n\n"
$message_libman_log see end

if {$tcl_platform(platform) == "windows"} {
   set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin/win32"
} else {
   set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin"
}


catch {open "|$sue2_bin_directory/export_cppsim_library $mod_name $sue_lib_filename single lite |& cat"} fid
set message_input [read $fid]
$message_libman_log insert end $message_input
$message_libman_log see end

if {[catch {close $fid} err] != 0} {
    $message_libman_log insert end "\n------------------ Exporting Exited with Errors --------------------\n"
    $message_libman_log see end
    set MOD_EXPORT_FOR_DELETE_RUN_FLAG 0
    return
}

if {$tcl_platform(platform) == "windows"} {
   set tar_bin_file "$CPPSIMSHARED_HOME/Msys/bin/tar"
} else {
   set tar_bin_file "tar"
}

set cur_directory [pwd]
cd $CPPSIM_HOME/Import_Export

$message_libman_log insert end "\n... Creating file '$export_destination_file.tar.gz ...\n\n"
$message_libman_log see end

catch {open "|$tar_bin_file cfz $export_destination_file.tar.gz ./SueLib ./CadenceLib ./SimRuns  |& cat"} fid
set message_input [read $fid]
$message_libman_log insert end $message_input
$message_libman_log see end

cd $cur_directory

if {[catch {close $fid} err] != 0} {
      $message_libman_log insert end "\n------------------ Exporting Exited with Errors --------------------\n"
      $message_libman_log see end
} else {
      $message_libman_log insert end "\n------------------ File '$export_destination_file.tar.gz' was successfully created --------------------\n"
      $message_libman_log insert end "------------------ in directory '$CPPSIM_HOME/Import_Export' --------------------\n"
      $message_libman_log see end
} 
set MOD_EXPORT_FOR_DELETE_RUN_FLAG 0
return
}


# Kill current CppSim run
proc cppLibManAddClose {} {

global TOP_LIBMAN_ADD_WINDOW

catch {destroy $TOP_LIBMAN_ADD_WINDOW}
}

proc cppUpdateSLIB {} {

global LIBMAN_ADD_SLIB_LISTBOX CUR_SLIB_NAME CUR_PLIB_NAME

set cur_index [$LIBMAN_ADD_SLIB_LISTBOX curselection]
if {$cur_index != ""} {
    set CUR_SLIB_NAME  [$LIBMAN_ADD_SLIB_LISTBOX get $cur_index]
    if {$CUR_SLIB_NAME == "*** No Shared Libraries to Add ***"} {
       set CUR_SLIB_NAME ""
    }
} else {
    set CUR_SLIB_NAME ""
}
set CUR_PLIB_NAME ""
}

proc cppUpdatePLIB {} {

global LIBMAN_ADD_PLIB_LISTBOX CUR_SLIB_NAME CUR_PLIB_NAME

set cur_index [$LIBMAN_ADD_PLIB_LISTBOX curselection]
if {$cur_index != ""} {
    set CUR_PLIB_NAME  [$LIBMAN_ADD_PLIB_LISTBOX get $cur_index]
    if {$CUR_PLIB_NAME == "*** No Private Libraries to Add ***"} {
       set CUR_PLIB_NAME ""
    }
} else {
    set CUR_PLIB_NAME ""
}
set CUR_SLIB_NAME ""
}

proc cppLibManUpdateAddLibraries {} {

global CPPSIM_HOME CPPSIMSHARED_HOME LIBMAN_ADD_SLIB_LISTBOX \
       LIBMAN_ADD_PLIB_LISTBOX


set sue_lib_filename $CPPSIM_HOME/Sue2/sue.lib

catch {open "$sue_lib_filename"} fid
set sue_lib_list ""
while {[gets $fid line] >= 0} {
    set sue_lib_list [concat $sue_lib_list $line] 
}
catch {close $fid} err
set num_sue_lib_entries [llength $sue_lib_list]

set cur_lib_dir $CPPSIM_HOME/SueLib
$LIBMAN_ADD_PLIB_LISTBOX delete 0 end
set lib_list [glob -nocomplain -tails -directory $cur_lib_dir -types d *]
set num_files [llength $lib_list]
set cur_index 0
set count 0

while {$cur_index < $num_files} {
   set cur_entry [lindex $lib_list $cur_index]
   set cur_index2 0
   while {$cur_index2 < $num_sue_lib_entries} {
     set cur_sue_lib_entry [lindex $sue_lib_list $cur_index2]
       if {$cur_entry == $cur_sue_lib_entry} {
           break
       }
     set cur_index2 [expr $cur_index2 + 1]
   }
   if {$cur_index2 == $num_sue_lib_entries} {
      set base_index [string first __Private__ $cur_entry]
      if {$base_index != -1} {
	  set cur_entry [string range $cur_entry 0 [expr $base_index - 1]]
      } 
      $LIBMAN_ADD_PLIB_LISTBOX insert end $cur_entry
      set count [expr $count + 1]
   }
   set cur_index [expr $cur_index + 1]
}
if {$count == 0} {
  $LIBMAN_ADD_PLIB_LISTBOX insert end "*** No Private Libraries to Add ***"
}

set cur_lib_dir $CPPSIMSHARED_HOME/SueLib

$LIBMAN_ADD_SLIB_LISTBOX delete 0 end
set lib_list [glob -nocomplain -tails -directory $cur_lib_dir -types d *]
set num_files [llength $lib_list]
set cur_index 0
set count 0

while {$cur_index < $num_files} {
   set cur_entry [lindex $lib_list $cur_index]
   set cur_index2 0
   while {$cur_index2 < $num_sue_lib_entries} {
     set cur_sue_lib_entry_raw [lindex $sue_lib_list $cur_index2]
       set cur_sue_lib_entry [cpp_libman_public_or_private_library $cur_sue_lib_entry_raw]
       if {$cur_entry == $cur_sue_lib_entry} {
           break
       }
     set cur_index2 [expr $cur_index2 + 1]
   }
   if {$cur_index2 == $num_sue_lib_entries} {
      $LIBMAN_ADD_SLIB_LISTBOX insert end $cur_entry
      set count [expr $count + 1]
   }
   set cur_index [expr $cur_index + 1]
}
if {$count == 0} {
  $LIBMAN_ADD_SLIB_LISTBOX insert end "*** No Shared Libraries to Add ***"
}

}



proc cpp_add_to_sue_lib {} {
global CPPSIM_HOME CPPSIMSHARED_HOME CUR_SLIB_NAME CUR_PLIB_NAME \
       LIBMAN_ADD_PLIB_LISTBOX LIBMAN_ADD_SLIB_LISTBOX \
       message_libman_add_log tcl_platform

if {$CUR_SLIB_NAME == "" && $CUR_PLIB_NAME == ""} {
   tk_dialog_new .cppsim_message "Error:  Need to Select Library" \
	        "Error: you need to select a library\nin one of the listboxes" "" 0 {OK} 
   return
}


set sue_lib_filename $CPPSIM_HOME/Sue2/sue.lib

set private_shared_collision_flag 0

if {$CUR_SLIB_NAME != ""} {
    set cur_lib_dir $CPPSIM_HOME/SueLib
    set lib_list [glob -nocomplain -tails -directory $cur_lib_dir -types d *]
    set num_libs [llength $lib_list]
    set cur_index 0

    while {$cur_index < $num_libs} {
       set cur_entry [lindex $lib_list $cur_index]
       set ALT_SLIB_NAME $CUR_SLIB_NAME\__Private__
       if {$cur_entry == $CUR_SLIB_NAME || $cur_entry == $ALT_SLIB_NAME} {
            break
	}
	set cur_index [expr $cur_index + 1]
    }
    if {$cur_index == $num_libs} {
       set verify_flag [tk_dialog_new .cppsim_message "Verification of Add Operation" \
		  "Are you sure that you want to add shared library\n'$CUR_SLIB_NAME' to the sue.lib file?" "" 1 {Yes} {Cancel}]
    } else {
       set lib_list [$LIBMAN_ADD_PLIB_LISTBOX get 0 end]
       set num_libs [llength $lib_list]
       set cur_index 0

	while {$cur_index < $num_libs} {
	   set cur_entry [lindex $lib_list $cur_index]
	    if {$cur_entry == $CUR_SLIB_NAME} {
		break;
	    }
	    set cur_index [expr $cur_index + 1] 
	}
       if {$cur_index == $num_libs} {
           set private_shared_collision_flag 2
           set verify_flag [tk_dialog_new .cppsim_message "Verification of Add Operation" \
		  "Note:  PRIVATE library '$CUR_SLIB_NAME'\nhas already been added to the sue.lib file\nAre you sure that you want to REPLACE this PRIVATE library\n with SHARED library '$CUR_SLIB_NAME'?" "" 1 {Yes} {Cancel}]
       } else {
           set private_shared_collision_flag 1
           set verify_flag [tk_dialog_new .cppsim_message "Verification of Add Operation" \
		  "Note:  there is also a PRIVATE library '$CUR_SLIB_NAME'\nAre you sure that you want to add SHARED library\n'$CUR_SLIB_NAME' to the sue.lib file?" "" 1 {Yes} {Cancel}]
       }
    }
    

    if {$verify_flag == 0} {

       if {$private_shared_collision_flag != 0} {
           set private_dir $CPPSIM_HOME/SueLib/$CUR_SLIB_NAME
	   if {[file isdirectory $private_dir]} {
	       file rename -force $private_dir $private_dir\__Private__
	   }
       }
       if {$private_shared_collision_flag < 2} {
           update_sue_lib_file -add $CUR_SLIB_NAME
       }
       update_sue_lib_file -add ""
       cppLibManUpdateAddLibraries

       set first_sue_file [cpp_first_cell_in_library_libman_add $CUR_SLIB_NAME]
       if {$first_sue_file == ""} {
           $message_libman_add_log insert end "\n**** Library was successfully added ****\n"
           $message_libman_add_log see end
       } else {
          if {$tcl_platform(platform) == "windows"} {
             set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin/win32"
          } else {
             set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin"
          }
          set sue_lib_file $CPPSIM_HOME/Sue2/sue.lib
          $message_libman_add_log delete 1.0 end
          $message_libman_add_log insert end "\n---- Determining module name clashes for library '$CUR_SLIB_NAME' ----\n"
          $message_libman_add_log see end

          catch {open "|$sue2_bin_directory/check_dependencies_or_name_clashes $sue_lib_file $first_sue_file all name_clashes verbose |& cat"} fid
          set message_input [read $fid]
          $message_libman_add_log insert end $message_input
          $message_libman_add_log see end

          if {[catch {close $fid} err] != 0} {
             $message_libman_add_log insert end "\n**************************  Operation aborted due to module name clashes  **************************\n"
             $message_libman_add_log insert end "\n**** Consider exporting this library instead by pushing 'Export Library Modules' button above and then ****\n"
             $message_libman_add_log insert end "**** importing the library by pressing the 'Import Library' button in the Library Manager window ****\n"
             $message_libman_add_log see end
             update_sue_lib_file -remove $CUR_SLIB_NAME
             cppLibManUpdateAddLibraries
             return
          } else {
             $message_libman_add_log insert end "\n**** Library was successfully added ****\n"
             $message_libman_add_log see end
          } 
       }

   }
}

if {$CUR_PLIB_NAME != ""} {
    set cur_lib_dir $CPPSIMSHARED_HOME/SueLib
    set lib_list [glob -nocomplain -tails -directory $cur_lib_dir -types d *]
    set num_libs [llength $lib_list]
    set cur_index 0

    while {$cur_index < $num_libs} {
        set cur_entry [lindex $lib_list $cur_index]
	if {$cur_entry == $CUR_PLIB_NAME} {
            break
	}
	set cur_index [expr $cur_index + 1]
    }
    if {$cur_index == $num_libs} {
       set verify_flag [tk_dialog_new .cppsim_message "Verification of Add Operation" \
		  "Are you sure that you want to add private library\n'$CUR_PLIB_NAME' to the sue.lib file?" "" 1 {Yes} {Cancel}]
    } else {
       set lib_list [$LIBMAN_ADD_SLIB_LISTBOX get 0 end]
       set num_libs [llength $lib_list]
       set cur_index 0

	while {$cur_index < $num_libs} {
	   set cur_entry [lindex $lib_list $cur_index]
	    if {$cur_entry == $CUR_PLIB_NAME} {
		break;
	    }
	    set cur_index [expr $cur_index + 1] 
	}
       if {$cur_index == $num_libs} {
           set private_shared_collision_flag 2
           set verify_flag [tk_dialog_new .cppsim_message "Verification of Add Operation" \
		  "Note:  SHARED library '$CUR_PLIB_NAME'\nhas already been added to the sue.lib file\nAre you sure that you want to REPLACE this SHARED library\n with PRIVATE library '$CUR_PLIB_NAME'?" "" 1 {Yes} {Cancel}]
       } else {
           set private_shared_collision_flag 1
           set verify_flag [tk_dialog_new .cppsim_message "Verification of Add Operation" \
		  "Note:  there is also a SHARED library '$CUR_PLIB_NAME'\nAre you sure that you want to add PRIVATE library\n'$CUR_PLIB_NAME' to the sue.lib file?" "" 1 {Yes} {Cancel}]
       }
    }
    if {$verify_flag == 0} {
	if {$private_shared_collision_flag != 0} {
           set private_dir $CPPSIM_HOME/SueLib/$CUR_PLIB_NAME
	    if {[file isdirectory $private_dir\__Private__]} {
	       file rename -force $private_dir\__Private__ $private_dir
	    }
	}
	if {$private_shared_collision_flag < 2} {
           update_sue_lib_file -add $CUR_PLIB_NAME
	}
       update_sue_lib_file -add ""
       cppLibManUpdateAddLibraries

       set first_sue_file [cpp_first_cell_in_library_libman_add $CUR_PLIB_NAME]

       if {$first_sue_file == ""} {
           $message_libman_add_log insert end "\n**** Library was successfully added ****\n"
           $message_libman_add_log see end
       } else {
          if {$tcl_platform(platform) == "windows"} {
             set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin/win32"
          } else {
             set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin"
          }
          set sue_lib_file $CPPSIM_HOME/Sue2/sue.lib
          $message_libman_add_log delete 1.0 end
          $message_libman_add_log insert end "\n---- Determining module name clashes for library '$CUR_PLIB_NAME' ----\n"
          $message_libman_add_log see end

          catch {open "|$sue2_bin_directory/check_dependencies_or_name_clashes $sue_lib_file $first_sue_file all name_clashes verbose |& cat"} fid
          set message_input [read $fid]
          $message_libman_add_log insert end $message_input
          $message_libman_add_log see end

          if {[catch {close $fid} err] != 0} {
             $message_libman_add_log insert end "\n**************************  Operation aborted due to module name clashes  **************************\n"
             $message_libman_add_log insert end "\n**** Consider exporting this library instead by pushing 'Export Library' button above and then ****\n"
             $message_libman_add_log insert end "**** importing the library by pressing the 'Import Library' button in the Library Manager window ****\n"
             $message_libman_add_log see end
             update_sue_lib_file -remove $CUR_PLIB_NAME
             cppLibManUpdateAddLibraries
             return
          } else {
             $message_libman_add_log insert end "\n**** Library was successfully added ****\n"
             $message_libman_add_log see end
          } 
       }

    }
}
}

proc cpp_create_entry_form {title description suffix initial_text_value} {

global TOP_CREATE_ENTRY_WINDOW cpp_text_entry_value

catch {destroy $TOP_CREATE_ENTRY_WINDOW}
set cpp_text_entry_value $initial_text_value

toplevel .cppCreateEntry
set TOP_CREATE_ENTRY_WINDOW .cppCreateEntry

set x [expr [winfo screenwidth $TOP_CREATE_ENTRY_WINDOW]/2 - [winfo reqwidth $TOP_CREATE_ENTRY_WINDOW]/2 \
	    - [winfo vrootx [winfo parent $TOP_CREATE_ENTRY_WINDOW]]]
set y [expr [winfo screenheight $TOP_CREATE_ENTRY_WINDOW]/2 - [winfo reqheight $TOP_CREATE_ENTRY_WINDOW]/2 \
	    - [winfo vrooty [winfo parent $TOP_CREATE_ENTRY_WINDOW]]]

wm geometry .cppCreateEntry "+$x+$y"
wm title .cppCreateEntry $title
    
bind .cppCreateEntry <Escape> {catch {destroy .cppCreateEntry} ; return ""}

# Create a frame for buttons and entry.
frame .cppCreateEntry.button_frame -borderwidth 10

pack .cppCreateEntry.button_frame -side top -fill x
# Create the command buttons.
button .cppCreateEntry.button_frame.ok -text "OK" -width 15 -command "cppCreateEntryOK"
button .cppCreateEntry.button_frame.cancel -text "Cancel" -command cppCreateEntryCancel
pack .cppCreateEntry.button_frame.cancel  .cppCreateEntry.button_frame.ok -side left -padx 2

frame .cppCreateEntry.text_entry_frame
label .cppCreateEntry.text_entry_frame.label -text $description -padx 1
entry .cppCreateEntry.text_entry_frame.destination_library_entry -width 58 -relief sunken \
-textvariable cpp_text_entry_value
label .cppCreateEntry.text_entry_frame.label2 -text $suffix -padx 1

pack .cppCreateEntry.text_entry_frame.label -side left -padx 5
pack .cppCreateEntry.text_entry_frame.destination_library_entry -side left -fill x
pack .cppCreateEntry.text_entry_frame.label2 -side left -padx 1
pack .cppCreateEntry.text_entry_frame -side top -fill x -pady 15

frame .cppCreateEntry.spacer_frame
label .cppCreateEntry.spacer_frame.label -text "" -padx 1
pack .cppCreateEntry.spacer_frame.label -side left -padx 5
pack .cppCreateEntry.spacer_frame -side top -fill x -pady 1
}

proc cppCreateEntryCancel {} {
global TOP_CREATE_ENTRY_WINDOW cpp_text_entry_value

set cpp_text_entry_value "Cancelled_Operation_Occurred"
catch {destroy $TOP_CREATE_ENTRY_WINDOW}
}

proc cppCreateEntryOK {} {
global TOP_CREATE_ENTRY_WINDOW 

catch {destroy $TOP_CREATE_ENTRY_WINDOW}
}


proc cppLibraryCreate {} {
global CPPSIM_HOME message_libman_log TOP_CREATE_ENTRY_WINDOW \
       cpp_text_entry_value

set title "Create New Library"
set description "Library Name:"
set suffix ""

set initial_text_value "NewLib"

cpp_create_entry_form $title $description $suffix $initial_text_value
tkwait window $TOP_CREATE_ENTRY_WINDOW
set new_lib_name $cpp_text_entry_value
if {$new_lib_name == "Cancelled_Operation_Occurred"} {
   $message_libman_log delete 1.0 end
   $message_libman_log insert end "\n---- Library creation cancelled  ----\n"
   $message_libman_log see end
   return
} elseif {$new_lib_name == ""} {
   $message_libman_log delete 1.0 end
   $message_libman_log insert end "\n---- Error in Creating New Library  ----\n"
   $message_libman_log insert end "\n     Library name was blank!\n"
   $message_libman_log insert end "\n***** Please try again with a non-blank library name *****\n"
   $message_libman_log see end
   return
}
set new_lib_name [string map {" " "_" ":" "_" "." "_" "," "_" ";" "_" "-" "_" "*" "_"} $new_lib_name]
set new_lib_dir $CPPSIM_HOME/SueLib/$new_lib_name

if {[file isdirectory $new_lib_dir]} {
   $message_libman_log delete 1.0 end
   $message_libman_log insert end "\n---- Error in Creating New Library  ----\n"
   $message_libman_log insert end "\n     Library '$new_lib_name' already exists!\n"
   $message_libman_log insert end "\n***** Please try again with a different library name *****\n"
   $message_libman_log see end
   return
}

$message_libman_log delete 1.0 end
$message_libman_log insert end "\n---- Creating New Library '$new_lib_name' ----\n"
$message_libman_log see end

if {[catch {file mkdir $new_lib_dir} err] != 0} {
    $message_libman_log insert end "\n**** Error: failed to make new directory '$new_lib_dir' ****\n"
    $message_libman_log insert end "\n     error message:\n"
    $message_libman_log insert end "\n     $err:\n"
    $message_libman_log see end
    return
}

## create fake first module to allow indexing of library
set fake_module_name "first_module_for_$new_lib_name"

if {[catch {set fp [open $new_lib_dir/$fake_module_name.sue w]} err] != 0} {
    $message_libman_log insert end "\n**** Error: failed to create new module '$fake_module_name' in directory '$new_lib_dir' ****\n"
    $message_libman_log insert end "\n     error message:\n"
    $message_libman_log insert end "\n     $err:\n"
    $message_libman_log see end
    return
}
puts $fp "proc SCHEMATIC_$fake_module_name {} {"
puts $fp "make output -name out -origin {530 160}"
puts $fp " make_text -origin {410 120} -text {NOTE:  THIS MODULE WILL BE AUTOMATICALLY"
puts $fp "            DELETED ONCE YOU CREATE YOUR OWN"
puts $fp "            MODULE WITHIN THIS LIBRARY"
puts $fp "            (DELETION OCCURS UPON RESTARTING SUE2)}"
puts $fp "}"
close $fp

if {[catch {set fp [open $new_lib_dir/tclIndex w]} err] != 0} {
    $message_libman_log insert end "\n**** Error: failed to create file 'tclIndex' in directory '$new_lib_dir' ****\n"
    $message_libman_log insert end "\n     error message:\n"
    $message_libman_log insert end "\n     $err:\n"
    $message_libman_log see end
    return
}
puts $fp "# Tcl autoload index file, version 2.0"
puts $fp "# This file is generated by the \"auto_mkindex\" command"
puts $fp "# and sourced to set up indexing information for one or"
puts $fp "# more commands.  Typically each line is a command that"
puts $fp "# sets an element in the auto_index array, where the"
puts $fp "# element name is the name of a command and the value is"
puts $fp "# a script that loads the command."
puts $fp " "
puts $fp "set auto_index(SCHEMATIC_$fake_module_name) [list source [file join \$dir $fake_module_name.sue]]"
close $fp

update_sue_lib_file -add $new_lib_name

$message_libman_log insert end "\n*****  Private library '$new_lib_name' successfully created  *****\n"
$message_libman_log see end

tk_dialog_new .cppsim_message "Warning: need to restart Sue2 to see this new library" \
	       "Warning:  Sue2 must be restarted in order to access this new library '$new_lib_name'" "" 0 {OK}
            return ""

}


proc cppLibraryRename {} {
global CPPSIM_HOME message_libman_log TOP_CREATE_ENTRY_WINDOW \
       cpp_text_entry_value

set old_lib_name [cppLibManGetCurLib]
if {$old_lib_name == ""} {
    tk_dialog_new .cppsim_message "Error:  Need to Select Library" \
	        "Error: you need to select a library\nin the 'Library:' listbox" "" 0 {OK} 
    return
}
set old_lib_dir [cppLibManGetCurLibDir]
set private_old_lib_dir $CPPSIM_HOME/SueLib/$old_lib_name
if {$old_lib_dir != $private_old_lib_dir} {
    tk_dialog_new .cppsim_message "Error:  Need to Select a PRIVATE Library" \
	        "Error: renaming of libraries can only be applied to PRIVATE libraries\n      Library '$old_lib_name' is a SHARED library\n     You can see which libraries are private by clicking on the\n    'Add Library' button of 'sue.lib' Operations:\n\n    *****  Please select a PRIVATE library in the 'Library:' listbox  *****" "" 0 {OK} 
    return
}

set title "Rename Library '$old_lib_name'"
set description "Rename Library '$old_lib_name' as:"
set suffix ""

set initial_text_value "NewLib"

cpp_create_entry_form $title $description $suffix $initial_text_value
tkwait window $TOP_CREATE_ENTRY_WINDOW
set new_lib_name $cpp_text_entry_value
if {$new_lib_name == "Cancelled_Operation_Occurred"} {
   $message_libman_log delete 1.0 end
   $message_libman_log insert end "\n---- Library rename cancelled  ----\n"
   $message_libman_log see end
   return
} elseif {$new_lib_name == ""} {
   $message_libman_log delete 1.0 end
   $message_libman_log insert end "\n---- Error in Renaming Library  ----\n"
   $message_libman_log insert end "\n     New library name was blank!\n"
   $message_libman_log insert end "\n***** Please try again with a non-blank library name *****\n"
   $message_libman_log see end
   return
}
set new_lib_dir $CPPSIM_HOME/SueLib/$new_lib_name

if {[file isdirectory $new_lib_dir]} {
   $message_libman_log delete 1.0 end
   $message_libman_log insert end "\n---- Error in Renaming Library  ----\n"
   $message_libman_log insert end "\n     Library '$new_lib_name' already exists!\n"
   $message_libman_log insert end "\n***** Please try again with a different library name *****\n"
   $message_libman_log see end
   return
}

$message_libman_log delete 1.0 end
$message_libman_log insert end "\n---- Renaming Library '$old_lib_name' as '$new_lib_name' ----\n"
$message_libman_log see end

if {[catch {file rename -force $old_lib_dir $new_lib_dir} err] != 0} {
    $message_libman_log insert end "\n**** Error: failed to rename directory '$old_lib_dir' ****\n"
    $message_libman_log insert end "**** to '$new_lib_dir' ****\n"
    $message_libman_log insert end "\n     error message:\n"
    $message_libman_log insert end "\n     $err:\n"
    $message_libman_log see end
    return
}

set old_lib_dir $CPPSIM_HOME/CadenceLib/$old_lib_name
if {[file isdirectory $old_lib_dir]} {
    set new_lib_dir $CPPSIM_HOME/CadenceLib/$new_lib_name

    if {[catch {file rename -force $old_lib_dir $new_lib_dir} err] != 0} {
       $message_libman_log insert end "\n**** Error: failed to rename directory '$old_lib_dir' ****\n"
       $message_libman_log insert end "**** to '$new_lib_dir' ****\n"
       $message_libman_log insert end "\n     error message:\n"
       $message_libman_log insert end "\n     $err:\n"
       $message_libman_log see end
       return
    }
}

set old_lib_dir $CPPSIM_HOME/SimRuns/$old_lib_name
if {[file isdirectory $old_lib_dir]} {
    set new_lib_dir $CPPSIM_HOME/SimRuns/$new_lib_name

    if {[catch {file rename -force $old_lib_dir $new_lib_dir} err] != 0} {
       $message_libman_log insert end "\n**** Error: failed to rename directory '$old_lib_dir' ****\n"
       $message_libman_log insert end "**** to '$new_lib_dir' ****\n"
       $message_libman_log insert end "\n     error message:\n"
       $message_libman_log insert end "\n     $err:\n"
       $message_libman_log see end
       return
    }
}

update_sue_lib_file -remove $old_lib_name
update_sue_lib_file -add $new_lib_name

$message_libman_log insert end "\n*****  Renaming of library '$old_lib_name' to\n        private library '$new_lib_name' successfully completed  *****\n"
$message_libman_log see end

}



proc cppModuleDelete {} {
global CPPSIM_HOME message_libman_log \
       CUR_LIBRARY_FOR_MOD CUR_LIBRARY_DIR_FOR_MOD


set mod_name [cppLibManGetCurMod]
if {$mod_name == ""} {
   tk_dialog_new .cppsim_message "Error:  Need to Select Module" \
      "Error: you need to select a module\nin the 'Module:' listbox" "" 0 {OK} 
   return
}
set lib_name $CUR_LIBRARY_FOR_MOD
set lib_dir $CUR_LIBRARY_DIR_FOR_MOD
set private_lib_dir $CPPSIM_HOME/SueLib/$lib_name
if {$lib_dir != $private_lib_dir} {
    tk_dialog_new .cppsim_message "Error:  Need to Select a PRIVATE Library" \
	        "Error: deletion of modules can only be applied within PRIVATE libraries\n      Library '$lib_name' is a SHARED library\n     You can see which libraries are private by clicking on the\n    'Add Library' button of 'sue.lib' Operations:\n\n    *****  Please select a PRIVATE library in the 'Library:' listbox  *****" "" 0 {OK} 
    return
}

set verify_flag [tk_dialog_new .cppsim_message "Verification of Delete Operation" \
		  "Are you sure that you want to delete module\n'$mod_name'?" "" 1 {Yes} {Cancel}]

if {$verify_flag != 0} {
   return
}

$message_libman_log delete 1.0 end
$message_libman_log insert end "\n---- Deleting Module '$mod_name' ----\n"
$message_libman_log see end

cpp_export_libman_module_for_delete $lib_name $mod_name

set lib_file $CUR_LIBRARY_DIR_FOR_MOD/$mod_name.sue
if {[catch {file delete -force $lib_file} err] != 0} {
    $message_libman_log insert end "\n**** Error: failed to delete Sue2 file ****\n"
    $message_libman_log insert end "**** '$lib_file' ****\n"
    $message_libman_log insert end "\n     error message:\n"
    $message_libman_log insert end "\n     $err:\n"
    $message_libman_log see end
    return
}

set lib_dir $CPPSIM_HOME/CadenceLib/$lib_name/$mod_name
if {[file isdirectory $lib_dir]} {
    if {[catch {file delete -force $lib_dir} err] != 0} {
       $message_libman_log insert end "\n**** Error: failed to delete directory ****\n"
       $message_libman_log insert end "**** '$lib_dir' ****\n"
       $message_libman_log insert end "\n     error message:\n"
       $message_libman_log insert end "\n     $err:\n"
       $message_libman_log see end
       return
    }
}

set lib_dir $CPPSIM_HOME/SimRuns/$lib_name/$mod_name
if {[file isdirectory $lib_dir]} {
    if {[catch {file delete -force $lib_dir} err] != 0} {
       $message_libman_log insert end "\n**** Error: failed to delete directory ****\n"
       $message_libman_log insert end "**** '$lib_dir' ****\n"
       $message_libman_log insert end "\n     error message:\n"
       $message_libman_log insert end "\n     $err:\n"
       $message_libman_log see end
       return
    }
}

cppUpdateModules

$message_libman_log insert end "\n*****  Deletion of module '$mod_name' successfully completed  *****\n"
$message_libman_log see end

}


proc cppLibraryDelete {} {
global CPPSIM_HOME message_libman_log

set lib_name [cppLibManGetCurLib]
if {$lib_name == ""} {
    tk_dialog_new .cppsim_message "Error:  Need to Select Library" \
	        "Error: you need to select a library\nin the 'Library:' listbox" "" 0 {OK} 
    return
}
set lib_dir [cppLibManGetCurLibDir]
set private_lib_dir $CPPSIM_HOME/SueLib/$lib_name
if {$lib_dir != $private_lib_dir} {
    tk_dialog_new .cppsim_message "Error:  Need to Select a PRIVATE Library" \
	        "Error: deletion of libraries can only be applied to PRIVATE libraries\n      Library '$lib_name' is a SHARED library\n     You can see which libraries are private by clicking on the\n    'Add Library' button of 'sue.lib' Operations:\n\n    *****  Please select a PRIVATE library in the 'Library:' listbox  *****" "" 0 {OK} 
    return
}

set verify_flag [tk_dialog_new .cppsim_message "Verification of Delete Operation" \
		  "Are you sure that you want to delete library\n'$lib_name'?" "" 1 {Yes} {Cancel}]

if {$verify_flag != 0} {
   return
}

$message_libman_log delete 1.0 end
$message_libman_log insert end "\n---- Deleting Library '$lib_name' ----\n"
$message_libman_log see end


cpp_export_libman_library_for_delete $lib_name

if {[catch {file delete -force $lib_dir} err] != 0} {
    $message_libman_log insert end "\n**** Error: failed to delete directory ****\n"
    $message_libman_log insert end "**** '$lib_dir' ****\n"
    $message_libman_log insert end "\n     error message:\n"
    $message_libman_log insert end "\n     $err:\n"
    $message_libman_log see end
    return
}

set lib_dir $CPPSIM_HOME/CadenceLib/$lib_name
if {[file isdirectory $lib_dir]} {
    if {[catch {file delete -force $lib_dir} err] != 0} {
       $message_libman_log insert end "\n**** Error: failed to delete directory ****\n"
       $message_libman_log insert end "**** '$lib_dir' ****\n"
       $message_libman_log insert end "\n     error message:\n"
       $message_libman_log insert end "\n     $err:\n"
       $message_libman_log see end
       return
    }
}

set lib_dir $CPPSIM_HOME/SimRuns/$lib_name
if {[file isdirectory $lib_dir]} {
    if {[catch {file delete -force $lib_dir} err] != 0} {
       $message_libman_log insert end "\n**** Error: failed to delete directory ****\n"
       $message_libman_log insert end "**** '$lib_dir' ****\n"
       $message_libman_log insert end "\n     error message:\n"
       $message_libman_log insert end "\n     $err:\n"
       $message_libman_log see end
       return
    }
}

update_sue_lib_file -remove $lib_name

$message_libman_log insert end "\n*****  Deletion of library '$lib_name' successfully completed  *****\n"
$message_libman_log see end

}




proc cppModuleMove {} {

global CPPSIM_HOME message_libman_log TOP_CREATE_ENTRY_WINDOW \
       cpp_text_entry_value CUR_LIBRARY_FOR_MOD CUR_LIBRARY_DIR_FOR_MOD


set mod_name [cppLibManGetCurMod]
if {$mod_name == ""} {
   tk_dialog_new .cppsim_message "Error:  Need to Select Module" \
      "Error: you need to select a module\nin the 'Module:' listbox" "" 0 {OK} 
   return
}
set old_lib_name $CUR_LIBRARY_FOR_MOD
set old_lib_dir $CUR_LIBRARY_DIR_FOR_MOD
set private_old_lib_dir $CPPSIM_HOME/SueLib/$old_lib_name
if {$old_lib_dir != $private_old_lib_dir} {
    tk_dialog_new .cppsim_message "Error:  Need to Select a PRIVATE Library" \
	        "Error: moving of modules can only be applied between PRIVATE libraries\n      Library '$old_lib_name' is a SHARED library\n     You can see which libraries are private by clicking on the\n    'Add Library' button of 'sue.lib' Operations:\n\n    *****  Please select a PRIVATE library in the 'Library:' listbox  *****" "" 0 {OK} 
    return
}

set title "Move Module '$mod_name'"
set description "Move from Library '$old_lib_name' to:"
set suffix ""

set initial_text_value "NewLib"

cpp_create_entry_form $title $description $suffix $initial_text_value
tkwait window $TOP_CREATE_ENTRY_WINDOW
set new_lib_name $cpp_text_entry_value
if {$new_lib_name == "Cancelled_Operation_Occurred"} {
   $message_libman_log delete 1.0 end
   $message_libman_log insert end "\n---- Module move cancelled  ----\n"
   $message_libman_log see end
   return
} elseif {$new_lib_name == ""} {
   $message_libman_log delete 1.0 end
   $message_libman_log insert end "\n---- Error in Moving Module  ----\n"
   $message_libman_log insert end "\n     New library name was blank!\n"
   $message_libman_log insert end "\n***** Please try again with a non-blank library name *****\n"
   $message_libman_log see end
   return
}
set new_lib_dir $CPPSIM_HOME/SueLib/$new_lib_name

if {[file isdirectory $new_lib_dir] != 1} {
   $message_libman_log delete 1.0 end
   $message_libman_log insert end "\n---- Error in Moving Module  ----\n"
   $message_libman_log insert end "\n     Library '$new_lib_name' does not exist as a PRIVATE library!\n"
   $message_libman_log insert end "\n     You can see which libraries are private by clicking on the\n    'Add Library' button of 'sue.lib' Operations:\n"
   $message_libman_log insert end "\n***** Please try again with an existing PRIVATE library name *****\n"
   $message_libman_log see end
   return
}
set sue_lib_filename $CPPSIM_HOME/Sue2/sue.lib

set lib_in_sue_lib_flag 0
catch {open "$sue_lib_filename"} fid
while {[gets $fid line] >= 0} {
   if {$line == $new_lib_name} {
       set lib_in_sue_lib_flag 1
       break
   }
}

catch {close $fid} err
if {$lib_in_sue_lib_flag == 0} {
   $message_libman_log delete 1.0 end
   $message_libman_log insert end "\n---- Error in Moving Module  ----\n"
   $message_libman_log insert end "\n     Library '$new_lib_name' has not been added to the 'sue.lib' file!\n"
   $message_libman_log insert end "\n***** Please try again with a library name that has already been added to the 'sue.lib' file *****\n"
   $message_libman_log see end
   return
}


$message_libman_log delete 1.0 end
$message_libman_log insert end "\n---- Moving module '$mod_name ----'\n"
$message_libman_log insert end "\n     from Library '$old_lib_name' to Library '$new_lib_name' ----\n"
$message_libman_log see end

set old_file_name $old_lib_dir/$mod_name.sue
set new_file_name $new_lib_dir/$mod_name.sue

if {[file exists $old_file_name] != 1} {
    $message_libman_log insert end "\n**** Error: failed to locate sue module file:****\n"
    $message_libman_log insert end "    '$old_file_name'\n"
    $message_libman_log see end
    return
}
if {[file exists $new_file_name] == 1} {
    $message_libman_log insert end "\n**** Error:  sue module file already exists in new library:****\n"
    $message_libman_log insert end "    '$new_file_name'\n"
    $message_libman_log see end
    return
}

if {[catch {file rename -force $old_file_name $new_file_name} err] != 0} {
    $message_libman_log insert end "\n**** Error: failed to move module '$mod_name] *****\n"
    $message_libman_log insert end "      from '$old_lib_name' to '$new_lib_name'\n"
    $message_libman_log insert end "\n     error message:\n"
    $message_libman_log insert end "\n     $err:\n"
    $message_libman_log see end
    return
}

set old_lib_dir $CPPSIM_HOME/CadenceLib/$old_lib_name/$mod_name
if {[file isdirectory $old_lib_dir]} {
    set new_lib_dir $CPPSIM_HOME/CadenceLib/$new_lib_name
    if {[file isdirectory $new_lib_dir] != 1} {
        file mkdir -force $new_lib_dir
    }
    set new_lib_dir $CPPSIM_HOME/CadenceLib/$new_lib_name/$mod_name

    if {[catch {file rename -force $old_lib_dir $new_lib_dir} err] != 0} {
       $message_libman_log insert end "\n**** Error: failed to move module '$mod_name' CadenceLib directory ****\n"
       $message_libman_log insert end "**** from '$old_lib_dir' ****\n"
       $message_libman_log insert end "**** to '$new_lib_dir' ****\n"
       $message_libman_log insert end "\n     error message:\n"
       $message_libman_log insert end "\n     $err:\n"
       $message_libman_log see end
       return
    }
}

set old_lib_dir $CPPSIM_HOME/SimRuns/$old_lib_name/$mod_name
if {[file isdirectory $old_lib_dir]} {

    set new_lib_dir $CPPSIM_HOME/SimRuns/$new_lib_name
    if {[file isdirectory $new_lib_dir] != 1} {
        file mkdir -force $new_lib_dir
    }
    set new_lib_dir $CPPSIM_HOME/SimRuns/$new_lib_name/$mod_name

    if {[catch {file rename -force $old_lib_dir $new_lib_dir} err] != 0} {
       $message_libman_log insert end "\n**** Error: failed to move module '$mod_name' SimRuns directory ****\n"
       $message_libman_log insert end "**** from '$old_lib_dir' ****\n"
       $message_libman_log insert end "**** to '$new_lib_dir' ****\n"
       $message_libman_log insert end "\n     error message:\n"
       $message_libman_log insert end "\n     $err:\n"
       $message_libman_log see end
       return
    }
}

cppUpdateModules

$message_libman_log insert end "\n*****  Moving of module 'mod_name'  ***** \n"
$message_libman_log insert end "\n*****  from library to '$old_lib_name' to '$new_lib_name' was successful! *****\n"
$message_libman_log see end
}


proc cpp_first_cell_in_library_libman_add {lib_name} {

global CPPSIM_HOME CPPSIMSHARED_HOME message_libman_add_log

set private_suelib_dir $CPPSIM_HOME/SueLib
set shared_suelib_dir $CPPSIMSHARED_HOME/SueLib

set private_dir_exists [file isdirectory $private_suelib_dir/$lib_name]

set shared_dir_exists [file isdirectory $shared_suelib_dir/$lib_name]

if {$private_dir_exists} {
   set file_list [glob -nocomplain -tails -directory $private_suelib_dir/$lib_name *.sue]
   set num_files [llength $file_list]
   if {$num_files < 1} {
       if {$shared_dir_exists} {
           $message_libman_add_log insert end "\n------------------ Warning: No .sue files in library '$lib_name:Private' --------------------\n"
       } else {
           $message_libman_add_log insert end "\n------------------ Warning: No .sue files in library '$lib_name' --------------------\n"
       }
      $message_libman_add_log see end
      return ""
   }
   set first_sue_file [lindex $file_list 0]
   return $first_sue_file
} else {
   if {$shared_dir_exists} {
      set file_list [glob -nocomplain -tails -directory $shared_suelib_dir/$lib_name *.sue]
      set num_files [llength $file_list]
      if {$num_files < 1} {
         $message_libman_add_log insert end "\n------------------ Warning: No .sue files in library '$lib_name' --------------------\n"
         $message_libman_add_log see end
         return ""
      }
      set first_sue_file [lindex $file_list 0]
      return $first_sue_file

   } else {
      $message_libman_add_log insert end "\n------------------ Warning: Can't locate library '$lib_name' in the file system --------------------\n"
      $message_libman_add_log see end
      return ""
   }
}
}

