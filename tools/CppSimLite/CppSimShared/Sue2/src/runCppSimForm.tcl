#### Procedures for CppSim/VppSim Simulation Window

proc create_run_cppsim_form {cppsim_or_vppsim_mode} {

global CPPSIM_HOME CPPSIMSHARED_HOME \
       CUR_SIM_FILE SIM_FILE_MENU TOP_LIBRARY TOP_CELL EDITOR \
       TOP_SIM_WINDOW message_log cur_s tcl_platform CPPSIM_RUN_FLAG

if {$CPPSIM_RUN_FLAG == 1} {
   tk_dialog_new .cppsim_message "Error: Current Simulation Run Not Completed" \
	  "Can't open new CppSim/VppSim Run Menu or switch between CppSim/VppSim modes while simulation is running.  Hit Kill Run button to end current simulation if desired." "" 0 {OK} 
   return
}

catch {destroy $TOP_SIM_WINDOW}

set CPPSIM_RUN_FLAG 0
toplevel .cppRunMenu 
set TOP_SIM_WINDOW .cppRunMenu

set x [expr [winfo screenwidth $TOP_SIM_WINDOW]/2 - [winfo reqwidth $TOP_SIM_WINDOW]/2 \
	    - [winfo vrootx [winfo parent $TOP_SIM_WINDOW]]]
set y [expr [winfo screenheight $TOP_SIM_WINDOW]/2 - [winfo reqheight $TOP_SIM_WINDOW]/2 \
	    - [winfo vrooty [winfo parent $TOP_SIM_WINDOW]]]

wm geometry .cppRunMenu "+$x+$y"
# wm title .cppRunMenu "CppSim Run Menu --- cell: $TOP_CELL, library: $TOP_LIBRARY"
    
# bind .cppRunMenu <Escape> {catch {destroy .cppRunMenu} ; return}

# Create a frame for buttons and entry.
frame .cppRunMenu.button_frame -borderwidth 10

pack .cppRunMenu.button_frame -side top -fill x
# Create the command buttons.
button .cppRunMenu.button_frame.compile_run -padx 5 -text "Compile/Run" -command cppRunRoutine
button .cppRunMenu.button_frame.netlist -padx 5 -text "Netlist Only" -command "cppNetlistOnlyRoutine full"
button .cppRunMenu.button_frame.edit_sim_file -padx 5 -text "Edit Sim File" -command cppEditRoutine
button .cppRunMenu.button_frame.synchronize -padx 5 -text "Synchronize" -command "cppSynchronize full"
button .cppRunMenu.button_frame.kill_run -padx 5 -text "Kill Run" -command cppKillCppSimCID
button .cppRunMenu.button_frame.close -padx 5 -text "Close" -command cppClose


pack .cppRunMenu.button_frame.close .cppRunMenu.button_frame.kill_run .cppRunMenu.button_frame.synchronize .cppRunMenu.button_frame.edit_sim_file .cppRunMenu.button_frame.netlist .cppRunMenu.button_frame.compile_run -side left -padx 1

# Create a label entry for run mode dialog options
global cppRunMode prev_cppRunMode 
if {$cppsim_or_vppsim_mode == "CppSim"} {
    set cppRunMode CppSim
} else {
    set cppRunMode Verilog
}
set prev_cppRunMode None
set CUR_SIM_FILE "None"

if {$cppsim_or_vppsim_mode == "CppSim"} {
   frame .cppRunMenu.run_mode_frame
   label .cppRunMenu.run_mode_frame.label -text "Sim Mode:" -padx 1
   set SIM_MODE_MENU [tk_optionMenu .cppRunMenu.run_mode_frame.options cppRunMode None]
   $SIM_MODE_MENU delete 0 end
   $SIM_MODE_MENU add radiobutton -label "CppSim"
   $SIM_MODE_MENU entryconfigure 0 -command "set cppRunMode CppSim; cppUpdateRunMode"

   # Note that Verilog functionality has been implemented in VppSim mode
   # if {$tcl_platform(platform) != "windows"} {
   #  $SIM_MODE_MENU add radiobutton -label "Verilog (with CppSim modules)"
   #  $SIM_MODE_MENU entryconfigure 1 -command "set cppRunMode Verilog; cppUpdateRunMode"
   # }
   $SIM_MODE_MENU add radiobutton -label "Matlab Mex File Creation"
   $SIM_MODE_MENU entryconfigure 2 -command "set cppRunMode Matlab; cppUpdateRunMode"
   $SIM_MODE_MENU add radiobutton -label "Simulink S-Function Creation"
   $SIM_MODE_MENU entryconfigure 3 -command "set cppRunMode Simulink; cppUpdateRunMode"
} else {
   frame .cppRunMenu.run_mode_frame
   label .cppRunMenu.run_mode_frame.label -text "Sim Mode:" -padx 1
   set SIM_MODE_MENU [tk_optionMenu .cppRunMenu.run_mode_frame.options cppRunMode None]
   $SIM_MODE_MENU delete 0 end
   $SIM_MODE_MENU add radiobutton -label "Verilog (with CppSim modules)"
   $SIM_MODE_MENU entryconfigure 0 -command "set cppRunMode Verilog; cppUpdateRunMode"
}

pack .cppRunMenu.run_mode_frame.label -side left
pack .cppRunMenu.run_mode_frame.options -side left -fill x
pack .cppRunMenu.run_mode_frame -side top -fill x


# Create a labeled entry for the Ts: command
#frame .cppRunMenu.ts_frame
#label .cppRunMenu.ts_frame.ts_label -text "Ts:         " -padx 1
#entry .cppRunMenu.ts_frame.ts_entry -width 20 -relief sunken \
#-textvariable ts_value
#pack .cppRunMenu.ts_frame.ts_label -side left
#pack .cppRunMenu.ts_frame.ts_entry -side left -fill x -expand true
#pack .cppRunMenu.ts_frame -side top -fill x

# Create menu options for sim file
frame .cppRunMenu.sim_file_frame
label .cppRunMenu.sim_file_frame.file_label -text "Sim File:   " -padx 1
set SIM_FILE_MENU [tk_optionMenu .cppRunMenu.sim_file_frame.file_options CUR_SIM_FILE "None"]
pack .cppRunMenu.sim_file_frame.file_label -side left
pack .cppRunMenu.sim_file_frame.file_options -side left -fill x
pack .cppRunMenu.sim_file_frame -side top -fill x

# Set up key binding equivalents to the buttons
# bind .cppRunMenu.top.cmd <Return> Run
# bind .cppRunMenu.top.cmd <Control-c> Stop
# focus .cppRunMenu.top.cmd

# Create a text widget to log the output
frame .cppRunMenu.message_frame -pady 10
label .cppRunMenu.message_frame.label -text "Result:   " -padx 1
set message_log [text .cppRunMenu.message_frame.message -width 80 -height 14 \
-borderwidth 2 -relief raised -setgrid true \
-yscrollcommand {.cppRunMenu.message_frame.yscroll set} \
-xscrollcommand {.cppRunMenu.message_frame.xscroll set}]
scrollbar .cppRunMenu.message_frame.yscroll -command {.cppRunMenu.message_frame.message yview} -orient vertical -relief sunken
scrollbar .cppRunMenu.message_frame.xscroll -command {.cppRunMenu.message_frame.message xview} -orient horizontal -relief sunken

pack .cppRunMenu.message_frame.label -anchor nw -side left
pack .cppRunMenu.message_frame.xscroll -side bottom -fill x
pack .cppRunMenu.message_frame.yscroll -side right -fill y
pack .cppRunMenu.message_frame.message -side left -fill both -expand true
pack .cppRunMenu.message_frame -side top -fill both -expand true

# Set up hierarchy stuff
global MOD_CODE_LISTBOX MOD_CODE_LABEL
frame .cppRunMenu.hierarchy_frame

set MOD_CODE_LABEL [label .cppRunMenu.hierarchy_frame.label -text "Hierarchy File:  None" -padx 1 -pady 5]

frame .cppRunMenu.hierarchy_frame.modcode -width 30

label .cppRunMenu.hierarchy_frame.modcode.label -text "             " -padx 1
set MOD_CODE_LISTBOX [listbox .cppRunMenu.hierarchy_frame.modcode.modules \
   -height 7 -width 40 \
   -yscrollcommand {.cppRunMenu.hierarchy_frame.modcode.yscroll set} \
   -xscrollcommand {.cppRunMenu.hierarchy_frame.modcode.xscroll set}]
scrollbar .cppRunMenu.hierarchy_frame.modcode.yscroll -command {.cppRunMenu.hierarchy_frame.modcode.modules yview} -orient vertical -relief sunken
scrollbar .cppRunMenu.hierarchy_frame.modcode.xscroll -command {.cppRunMenu.hierarchy_frame.modcode.modules xview} -orient horizontal -relief sunken 

pack .cppRunMenu.hierarchy_frame.modcode.label -side left -anchor nw
pack .cppRunMenu.hierarchy_frame.modcode.xscroll -side bottom -fill x
pack .cppRunMenu.hierarchy_frame.modcode.yscroll -side right -fill y
pack .cppRunMenu.hierarchy_frame.modcode.modules -side left -fill both -expand true

bind $MOD_CODE_LISTBOX <Double-Button-1> "cppEditModuleCode"
#bind $MOD_CODE_LISTBOX <Return> "cppEditModuleCode"


pack .cppRunMenu.hierarchy_frame.label -side top -anchor nw
#pack .cppRunMenu.hierarchy_frame.buttons -side top  -fill x 
pack .cppRunMenu.hierarchy_frame.modcode -side top  -fill x -expand true


pack .cppRunMenu.hierarchy_frame -side top -fill both -expand true

$MOD_CODE_LISTBOX insert end "--- Need to Create a Sim File By Pushing 'Edit Sim File' Button ---"

cppSynchronize full
focus $MOD_CODE_LISTBOX

}

proc cppUpdateRunMode { } {
global cppRunMode prev_cppRunMode

if {$cppRunMode != $prev_cppRunMode} {
    cppUpdateHierFile load
}
set prev_cppRunMode $cppRunMode
}

#########################################################################
# Simple parser
# Usage:   out = cppSimpleParser " ab cd efg" 1
#             >  out = cd
#          out = cppSimpleParser " ab cd efg" 2
#             >  out = efg
#          out = cppSimpleParser " ab cd efg" -1
#             >  out = 3
#          out = cppSimpleParser " ab cd efg" 4
#             >  out = NULL
proc cppSimpleParser {input_string item_num} {
   set mark_index 0
   set cur_string $input_string

    while {[string range $cur_string $mark_index $mark_index] == " "} {
	set mark_index [expr $mark_index + 1]
    }
    set cur_string [string range $cur_string $mark_index end]
    set item_count 0

    while {$cur_string != ""} {
        set mark_index [string first " " $cur_string]
        if {$mark_index < 0} {
           set cur_item $cur_string
	   break;
	}
        set mark_index2 $mark_index
        while {[string range $cur_string $mark_index2 $mark_index2] == " "} {
	    set mark_index2 [expr $mark_index2 + 1]
	}
        set cur_item [string range $cur_string 0 [expr $mark_index - 1]]
        set remainder [string range $cur_string $mark_index2 end]
        set cur_string $remainder
        if {$item_num == $item_count} { break }
        set item_count [expr $item_count + 1] 
    }

    if {$item_num != $item_count} {
        if {$item_num == -1} {
	   set item_count [expr $item_count + 1]
           return $item_count
	} else {
           return NULL
	}
    } else {
        return $cur_item
    }
}

#########################################################################
# Simple parse and replace
# Usage:   out = cppSimpleParseAndReplace " ab cd efg" 1 "xy"
#             >  out = " ab xy efg"
#          out = cppSimpleParseAndReplace " ab cd efg" 4 "xy"
#             >  out = NULL
proc cppSimpleParseAndReplace {input_string item_num new_item} {
   set cur_string $input_string
   set mark_index 0

    while {[string range $cur_string $mark_index $mark_index] == " "} {
	set mark_index [expr $mark_index + 1]
    }
    set cur_string [string range $cur_string $mark_index end]
    set stop_index [expr $mark_index - 1]
    set item_count 0

    while {$cur_string != ""} {
        set mark_index [string first " " $cur_string]
        if {$mark_index < 0} {
           set cur_item $cur_string
           set remainder ""
	   break;
	}
        set mark_index2 $mark_index
        while {[string range $cur_string $mark_index2 $mark_index2] == " "} {
	    set mark_index2 [expr $mark_index2 + 1]
	}
        set cur_item [string range $cur_string 0 [expr $mark_index - 1]]
        set remainder [string range $cur_string $mark_index2 end]
        set cur_string $remainder
        if {$item_num == $item_count} { break }
	set stop_index [expr $stop_index + $mark_index2]
        set item_count [expr $item_count + 1] 
    }

    if {$item_num != $item_count} {
        return NULL
    } else {
        set initial_string [string range $input_string 0 $stop_index]
        return "$initial_string$new_item $remainder"
    }
}


# Edit Module Code
proc cppEditModuleCode {} {

global MOD_CODE_LISTBOX

set cur_mod_index [$MOD_CODE_LISTBOX curselection]
set instance_flag 0
if {$cur_mod_index != ""} {
    set cur_mod_line [$MOD_CODE_LISTBOX get $cur_mod_index] 
    set first_arg [cppSimpleParser $cur_mod_line 0]
    if {$first_arg == ":"} {
        set instance_flag 1
        set code_type [cppSimpleParser $cur_mod_line 1]
        while {$first_arg == ":"} {
            set cur_mod_index [expr $cur_mod_index - 1]
            set cur_mod_line [$MOD_CODE_LISTBOX get $cur_mod_index] 
            set first_arg [cppSimpleParser $cur_mod_line 0]
	}
    } else {
        set code_type [cppSimpleParser $cur_mod_line 0]
    }
    set module_code_type [cppSimpleParser $cur_mod_line 0]
    set module_name [cppSimpleParser $cur_mod_line 1]
    set num_args [cppSimpleParser $cur_mod_line -1]
    if {$num_args == 3} {
	set avail_code [cppSimpleParser $cur_mod_line 2]
    } else {
	set avail_code [cppSimpleParser $cur_mod_line 3]
    }

    cppModuleOptionsMenu $module_name $module_code_type $code_type $avail_code $instance_flag
}

}

# Update the hierarchy file
proc cppUpdateHierFile {control_string} {

global MOD_CODE_LISTBOX MOD_CODE_LABEL message_log tcl_platform \
       CPPSIMSHARED_HOME CPPSIM_HOME cppRunMode CUR_SIM_FILE CUR_DIR
 
if {$tcl_platform(platform) == "windows"} {
   set cppsimshared_bin_directory "$CPPSIMSHARED_HOME/bin/win32"
} else {
   set cppsimshared_bin_directory "$CPPSIMSHARED_HOME/bin"
}

set yscroll_setting [$MOD_CODE_LISTBOX yview]

if {$control_string == "cppsim"} {
   set new_item "c"
} elseif {$control_string == "verilog"} {
   set new_item "v"
} elseif {$control_string == "expand"} {
   set new_item "e"
} elseif {$control_string == "ignore"} {
   set new_item "i"
}

if {$control_string == "cppsim" || $control_string == "verilog" || \
    $control_string == "expand" || $control_string == "ignore"} {
    set cur_mod_index [$MOD_CODE_LISTBOX curselection]
    if {$cur_mod_index != ""} {
       set cur_mod_line [$MOD_CODE_LISTBOX get $cur_mod_index] 
       set code_type [cppSimpleParser $cur_mod_line 0]
       set arg_num 0
       if {$code_type == ":"} {
           set code_type [cppSimpleParser $cur_mod_line 1]
           set arg_num 1
	}
       set new_line [cppSimpleParseAndReplace $cur_mod_line $arg_num $new_item]
       $MOD_CODE_LISTBOX delete $cur_mod_index
       $MOD_CODE_LISTBOX insert $cur_mod_index $new_line
       $MOD_CODE_LISTBOX selection set $cur_mod_index
       $MOD_CODE_LISTBOX activate $cur_mod_index
    }
}


if {$control_string == "update" || $control_string == "load"} {
   if {$CUR_SIM_FILE == "None"} {
      $MOD_CODE_LABEL configure -text "Hierarchy File:  None"
      $MOD_CODE_LISTBOX delete 0 end
      $MOD_CODE_LISTBOX insert end "--- Need to Create a Sim File By Pushing 'Edit Sim File' Button ---"
      return
   }
   set base_name [string trimright $CUR_SIM_FILE "par"]
   set base_name [string trimright $base_name "."]
    if {$cppRunMode == "Verilog"} {
       set hier_file_name "$base_name.hier_v"
    } else {
       set hier_file_name "$base_name.hier_c"
    }
   $MOD_CODE_LABEL configure -text "Hierarchy File:  $hier_file_name"
   set reload_hier_file_flag 1
   ## cur_mod_index set to -1 in case "update" is not run
   set cur_mod_index -1
} else {
   set reload_hier_file_flag 0
}


if {$control_string == "update"} {
    ## cur_mod_index is used by the "load" routine below
    set cur_mod_index [$MOD_CODE_LISTBOX curselection]

    set listbox_contents [$MOD_CODE_LISTBOX get 0 end]
    ####### Open hier file
    if {[catch "set fid \[open $hier_file_name w\]" err]} {
       tk_dialog_new .cppsim_message "Error in updating hierarchy file" \
	  "$err" "" 0 {OK} 
       return
    }

    set length_listbox [llength $listbox_contents]
    set cur_index 0
    while {$cur_index < $length_listbox} {
        set cur_listbox_entry [lindex $listbox_contents $cur_index]
        puts $fid $cur_listbox_entry
	set cur_index [expr $cur_index + 1]
    }
    if {[catch {close $fid} err]} {
       tk_dialog_new .cppsim_message "Error in updating hierarchy file" \
	  "$err" "" 0 {OK} 
       return
    }
}

if {$reload_hier_file_flag == 1 && $CUR_SIM_FILE != "None"} {

   $message_log insert end "\n-------------- Updating Hierarchy File  --------------\n\n"

   if {$cppRunMode == "Verilog"} {
      catch {open "|$cppsimshared_bin_directory/net2code -hier_v $CUR_SIM_FILE $CPPSIM_HOME/Netlist/netlist.cppsim |& cat"} fid
   } else {
      catch {open "|$cppsimshared_bin_directory/net2code -hier_c $CUR_SIM_FILE $CPPSIM_HOME/Netlist/netlist.cppsim |& cat"} fid
   }
   set message_input [read $fid]
   $message_log insert end $message_input
   $message_log see end

   if {[catch {close $fid} err] != 0 || [file isfile $hier_file_name] == 0} {
      $message_log insert end "\n------------------ Net2code Program Exited with Errors --------------------\n"

      $message_log see end
      return
   } else {
      $message_log insert end "*********************  Done!  *********************\n"
      $message_log see end
   }
   catch {open $hier_file_name} fid
   $MOD_CODE_LISTBOX delete 0 end

   while {[gets $fid line] >= 0} { 
       $MOD_CODE_LISTBOX insert end $line
    }
   catch {close $fid} err
   if {$cur_mod_index != -1} {
       $MOD_CODE_LISTBOX selection set $cur_mod_index
       $MOD_CODE_LISTBOX activate $cur_mod_index
   }
}
$MOD_CODE_LISTBOX yview moveto [lindex $yscroll_setting 0]
 
}


# Run the program and arrange to read its messages
proc cppRunRoutine {} {

global message_log CPPSIMSHARED_HOME CPPSIM_HOME tcl_platform CPPSIM_RUN_FLAG \
       cppRunMode CUR_PID CUR_SIM_FILE TOP_CELL TOP_LIBRARY CUR_DIR 

if {$CPPSIM_RUN_FLAG == 1} {
    return
}
set CPPSIM_RUN_FLAG 1

$message_log delete 1.0 end

$message_log insert end "******* cell: $TOP_CELL  (Library: $TOP_LIBRARY) *******\n\n"
$message_log insert end "----------------- running netlister -----------------\n\n"

set netlist_dir "$CPPSIM_HOME/Netlist"
if {[file isdirectory $netlist_dir] == 0} {

   $message_log insert end "Error:  CppSim does not have a Netlist directory!\n"
   $message_log insert end "      (i.e., can't open directory '$netlist_dir')\n"
   $message_log insert end "      ->  create this directory to correct this problem\n"
   $message_log insert end "\n------------------ Netlisting Exited with Errors --------------------\n"
   $message_log see end
   set CPPSIM_RUN_FLAG 0
   return
}

if {[file isfile "$netlist_dir/netlist.cppsim"] != 0} {
    if {[catch {file delete "$netlist_dir/netlist.cppsim"} mssg]} {
        $message_log insert end "Error:  cannot delete netlist file!\n"
        $message_log insert end "     (i.e., cannot delete '$netlist_dir/netlist.cppsim')\n"
        $message_log insert end "      ->  remove this file to correct this problem\n"
        $message_log insert end "\n------------------ Netlisting Exited with Errors --------------------\n"
        $message_log see end
        set CPPSIM_RUN_FLAG 0
        return
    } 
}

if {$tcl_platform(platform) == "windows"} {
   set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin/win32"
   set cppsimshared_bin_directory "$CPPSIMSHARED_HOME/bin/win32"
   set make_command "$CPPSIMSHARED_HOME/Msys/bin/make"
} else {
   set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin"
   set cppsimshared_bin_directory "$CPPSIMSHARED_HOME/bin"
   set make_command "make"
}

catch {open "|$sue2_bin_directory/sue_cppsim_netlister $TOP_CELL \
        $CPPSIM_HOME/Sue2/sue.lib $netlist_dir/netlist.cppsim |& cat"} fid
set message_input [read $fid]
$message_log insert end $message_input
$message_log see end

if {[catch {close $fid} err] != 0 || [file isfile "$netlist_dir/netlist.cppsim"] == 0} {
    $message_log insert end "\n------------------ Netlisting Exited with Errors --------------------\n"
    $message_log see end
    set CPPSIM_RUN_FLAG 0
    return
}

$message_log insert end "\n-------------- running net2code program --------------\n\n"
$message_log see end

if {[file isfile "Makefile"] != 0} {
      if {[catch {file delete "Makefile"} mssg]} {
         $message_log insert end "Error:  cannot delete 'Makefile' file!\n"
         $message_log insert end "     (i.e., cannot delete '$CUR_DIR/Makefile')\n"
         $message_log insert end "      ->  remove this file to correct this problem\n"
         $message_log insert end "\n------------------ Net2code Program Exited with Errors --------------------\n"
         $message_log see end
         set CPPSIM_RUN_FLAG 0
         return
      } 
}

if {[file isfile "Matlab/Makefile"] != 0} {
      if {[catch {file delete "Matlab/Makefile"} mssg]} {
         $message_log insert end "Error:  cannot delete 'Matlab/Makefile' file!\n"
         $message_log insert end "     (i.e., cannot delete '$CUR_DIR/Matlab/Makefile')\n"
         $message_log insert end "      ->  remove this file to correct this problem\n"
         $message_log insert end "\n------------------ Net2code Program Exited with Errors --------------------\n"
         $message_log see end
         set CPPSIM_RUN_FLAG 0
         return
      } 
}

if {$cppRunMode == "CppSim"} {

   catch {open "|$cppsimshared_bin_directory/net2code -cpp $CUR_SIM_FILE $netlist_dir/netlist.cppsim |& cat"} fid
   set message_input [read $fid]
   $message_log insert end $message_input
   $message_log see end

   if {[catch {close $fid} err] != 0 || [file isfile "Makefile"] == 0} {
      $message_log insert end "\n------------------ Net2code Program Exited with Errors --------------------\n"
      $message_log see end
      set CPPSIM_RUN_FLAG 0
      return
   } else {
      $message_log insert end "\nNet2code program completed with no errors\n"
      $message_log see end
   }

   $message_log insert end "\n-------------- compiling and running CppSim simulation  --------------\n\n"
   $message_log see end

   if {[catch {open "|$make_command |& cat"} CUR_PID] == 0} {
      fileevent $CUR_PID readable cppMessageLog
    } else {
      $message_log insert end "\n------------------ Make command failed ?? --------------------\n"
      if {$tcl_platform(os) == "Darwin"} {
          $message_log insert end "\n--- DID YOU INSTALL XCODE AND INCLUDE COMMAND LINE TOOLS ? ---\n"
      }
      $message_log see end
   }
} elseif {$cppRunMode == "Verilog"} {
   catch {open "|$cppsimshared_bin_directory/net2code -vpp $CUR_SIM_FILE $netlist_dir/netlist.cppsim |& cat"} fid
   set message_input [read $fid]
   $message_log insert end $message_input
   $message_log see end

   if {[catch {close $fid} err] != 0 || [file isfile "Makefile"] == 0} {
      $message_log insert end "\n------------------ Net2code Program Exited with Errors --------------------\n"
      $message_log see end
      set CPPSIM_RUN_FLAG 0
      return
   } else {
      $message_log insert end "\nNet2code program completed with no errors\n"
      $message_log see end
   }

   $message_log insert end "\n-------------- compiling and running Verilog simulation  --------------\n\n"
   $message_log see end

   if {[catch {open "|$make_command |& cat"} CUR_PID] == 0} {
      fileevent $CUR_PID readable cppMessageLog
    } else {
      $message_log insert end "\n------------------ Make command failed ?? --------------------\n"
      $message_log see end
   }
} elseif {$cppRunMode == "Matlab"} {
  catch {open "|$cppsimshared_bin_directory/net2code -matlab $CUR_SIM_FILE $netlist_dir/netlist.cppsim |& cat"} fid
   set message_input [read $fid]
   $message_log insert end $message_input
   $message_log see end

   if {[catch {close $fid} err] != 0} {
      $message_log insert end "\n------------------ Net2code Program Exited with Errors --------------------\n"
      $message_log see end
      set CPPSIM_RUN_FLAG 0
      return
   } else {
      $message_log insert end "\nNet2code program completed with no errors\n"
      $message_log see end
   }

   if {[file isfile "Matlab/Makefile"] != 0} {
      cd "$CUR_DIR/Matlab"

      $message_log insert end "\n-------------- compiling 'initialize_usrp' code  --------------\n\n"
      $message_log see end

      if {[catch {open "|$make_command |& cat"} CUR_PID] == 0} {
         fileevent $CUR_PID readable cppMessageLogUSRP
      } else {
         $message_log insert end "\n------------------ Make command failed ?? --------------------\n"
         $message_log see end
         set CPPSIM_RUN_FLAG 0
      }
      cd $CUR_DIR
   } else {
    $message_log insert end "\n****** Note:  to compile mex function within Matlab: ******\n"
    $message_log insert end ">> cd $CPPSIM_HOME/SimRuns/$TOP_LIBRARY/$TOP_CELL/Matlab\n"
    $message_log insert end ">> compile_$TOP_CELL\n"
    $message_log insert end "\n****** Note:  to run mex function within Matlab: ******\n"
    $message_log insert end ">> addpath('$CPPSIM_HOME/SimRuns/$TOP_LIBRARY/$TOP_CELL/Matlab')\n"
    $message_log insert end ">> $TOP_CELL  %% (shows input and output signals) %%\n"

    $message_log insert end "\n*********************  Done!  *********************\n"
    $message_log see end
    set CPPSIM_RUN_FLAG 0
   }
} elseif {$cppRunMode == "Simulink"} {
  catch {open "|$cppsimshared_bin_directory/net2code -simulink $CUR_SIM_FILE $netlist_dir/netlist.cppsim |& cat"} fid
   set message_input [read $fid]
   $message_log insert end $message_input
   $message_log see end

   if {[catch {close $fid} err] != 0} {
      $message_log insert end "\n------------------ Net2code Program Exited with Errors --------------------\n"
      $message_log see end
      set CPPSIM_RUN_FLAG 0
      return
   } else {
      $message_log insert end "\nNet2code program completed with no errors\n"
      $message_log see end
   }

   if {[file isfile "Matlab/Makefile"] != 0} {
      cd "$CUR_DIR/Matlab"

      $message_log insert end "\n-------------- compiling 'initialize_usrp' code  --------------\n\n"
      $message_log see end

      if {[catch {open "|$make_command |& cat"} CUR_PID] == 0} {
         fileevent $CUR_PID readable cppMessageLogUSRP
      } else {
         $message_log insert end "\n------------------ Make command failed ?? --------------------\n"
         $message_log see end
         set CPPSIM_RUN_FLAG 0
      }
      cd $CUR_DIR
   } else {
    $message_log insert end "\n****** Note:  to compile S-Function within Matlab and set path: ******\n"
    $message_log insert end ">> cd $CPPSIM_HOME/SimRuns/$TOP_LIBRARY/$TOP_CELL/Matlab\n"
    $message_log insert end ">> compile_$TOP_CELL\_s\n"
    $message_log insert end ">> addpath('$CPPSIM_HOME/SimRuns/$TOP_LIBRARY/$TOP_CELL/Matlab')\n"
    $message_log insert end "\n****** Note:  to use S-Function within Simulink: ******\n"
    $message_log insert end "1) Drag 'S-Function' block from Simulink Library Browser to your model and double-click on it \n"
    $message_log insert end "2) Enter '$TOP_CELL\_s' for 'S-function name:' \n"
    $message_log insert end "3) Enter appropriate values for 'S-function parameters:' \n"
   catch {open "$CPPSIM_HOME/SimRuns/$TOP_LIBRARY/$TOP_CELL/Matlab/$TOP_CELL\_s.txt"} fid_sc
   set message_input [read $fid_sc]
   catch {close $fid_sc}
   $message_log insert end $message_input
    $message_log insert end "*********************************************  Done!  ********************************************"
   $message_log see end
    set CPPSIM_RUN_FLAG 0
   }
}

}

# Read and log output from the program
proc cppMessageLog {} {
global CUR_PID message_log CPPSIM_HOME TOP_CELL TOP_LIBRARY \
       CPPSIM_RUN_FLAG

if [eof $CUR_PID] {
    catch {close $CUR_PID}
    $message_log insert end "Note:  run files contained in directory:\n     $CPPSIM_HOME/SimRuns/$TOP_LIBRARY/$TOP_CELL\n"
    $message_log see end
    set CPPSIM_RUN_FLAG 0
} else {
gets $CUR_PID line
$message_log insert end $line\n
$message_log see end
}
}

# Read and log output from the program
proc cppMessageLogUSRP {} {
global CUR_PID message_log CPPSIM_HOME TOP_CELL TOP_LIBRARY \
       CPPSIM_RUN_FLAG

if [eof $CUR_PID] {
    catch {close $CUR_PID}
    $message_log insert end "\n****** Note:  to compile mex function within Matlab: ******\n"
    $message_log insert end "> cd $CPPSIM_HOME/SimRuns/$TOP_LIBRARY/$TOP_CELL/Matlab\n"
    $message_log insert end "> compile_$TOP_CELL\n"
    $message_log insert end "\n****** Note:  to run mex function within Matlab: ******\n"
    $message_log insert end "> addpath('$CPPSIM_HOME/SimRuns/$TOP_LIBRARY/$TOP_CELL/Matlab')\n"
    $message_log insert end "> system('initialize_usrp')  %% (Initializes USRP board) %%\n"
    $message_log insert end "> $TOP_CELL  %% (shows input and output signals) %%\n"

    $message_log insert end "\n*********************  Done!  *********************\n"
    $message_log see end
    set CPPSIM_RUN_FLAG 0
} else {
gets $CUR_PID line
$message_log insert end $line\n
$message_log see end
}
}


#########################################################################################################

# Netlist only routine
proc cppNetlistOnlyRoutine {option_string} {


global message_log CPPSIMSHARED_HOME CPPSIM_HOME tcl_platform \
       cppRunMode CUR_SIM_FILE TOP_CELL TOP_LIBRARY

if {$option_string == "full"} {
   $message_log delete 1.0 end
   $message_log insert end "******* cell: $TOP_CELL  (Library: $TOP_LIBRARY) *******\n\n"
}
$message_log insert end "----------------- running netlister -----------------\n\n"




set netlist_dir "$CPPSIM_HOME/Netlist"
if {[file isdirectory $netlist_dir] == 0} {

   $message_log insert end "Error:  CppSim does not have a Netlist directory!\n"
   $message_log insert end "      (i.e., can't open directory '$netlist_dir')\n"
   $message_log insert end "      ->  create this directory to correct this problem\n"
   $message_log insert end "\n------------------ Netlisting Exited with Errors --------------------\n"
   $message_log see end
   return
}

if {[file isfile "$netlist_dir/netlist.cppsim"] != 0} {
    if {[catch {file delete "$netlist_dir/netlist.cppsim"} mssg]} {
        $message_log insert end "Error:  cannot delete netlist file!\n"
        $message_log insert end "     (i.e., cannot delete '$netlist_dir/netlist.cppsim')\n"
        $message_log insert end "      ->  remove this file to correct this problem\n"
        $message_log insert end "\n------------------ Netlisting Exited with Errors --------------------\n"
        $message_log see end
        return
    } 
}

if {$tcl_platform(platform) == "windows"} {
   set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin/win32"
} else {
   set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin"
}

catch {open "|$sue2_bin_directory/sue_cppsim_netlister $TOP_CELL \
        $CPPSIM_HOME/Sue2/sue.lib $netlist_dir/netlist.cppsim |& cat"} fid
set message_input [read $fid]
$message_log insert end $message_input
$message_log see end

if {[catch {close $fid} err] != 0 || [file isfile "$netlist_dir/netlist.cppsim"] == 0} {
    $message_log insert end "\n------------------ Netlisting Exited with Errors --------------------\n"
    $message_log see end
    return
}


if {$cppRunMode == "CppSim" && $option_string == "full"} {
   if {$tcl_platform(platform) != "windows"} {
      $message_log insert end "\n\n****** Note:  to run CppSim from UNIX shell: ******\n"
      $message_log insert end "> cd $CPPSIM_HOME/SimRuns/$TOP_LIBRARY/$TOP_CELL\n"
      if {$CUR_SIM_FILE == "test.par"} {
         $message_log insert end "> net2code -cpp test.par $CPPSIM_HOME/Netlist/netlist.cppsim\n"
      } else {
         $message_log insert end "> net2code -cpp $CUR_SIM_FILE $CPPSIM_HOME/Netlist/netlist.cppsim\n"
      }
      $message_log insert end "> make\n"
   }
   $message_log insert end "\n****** Note:  to run CppSim within Matlab: ******\n"
   $message_log insert end "> cd $CPPSIM_HOME/SimRuns/$TOP_LIBRARY/$TOP_CELL\n"
    if {$CUR_SIM_FILE == "test.par"} {
        $message_log insert end "> cppsim\n\n"
    } else {
        $message_log insert end "> cppsim('$CUR_SIM_FILE')\n\n"
    }
   $message_log insert end "*********************  Done!  *********************\n"
}
}

#########################################################################################################

# Edit Sim File
proc cppEditRoutine {} {

global CPPSIM_HOME CPPSIMSHARED_HOME  \
       CUR_SIM_FILE message_log TOP_CELL TOP_LIBRARY CUR_DIR


#$message_log delete 1.0 end


if {$CUR_SIM_FILE == "None"} {

    set CUR_SIM_FILE test.par
    set cur_file "$CUR_DIR/$CUR_SIM_FILE"
    set base_name [string trimright $CUR_SIM_FILE "par"]
    set base_name [string trimright $base_name "."]

    ####### Open sim file
    if {[catch "set fid \[open $cur_file w\]" err]} {
       tk_dialog_new .cppsim_message "Error in creating Sim File" \
	  "$err" "" 0 {OK} 
       return
    }

    puts $fid "/////////////////////////////////////////////////////////////"
    puts $fid "// CppSim Sim File: $CUR_SIM_FILE"
    puts $fid "// Cell: $TOP_CELL"
    puts $fid "// Library: $TOP_LIBRARY"
    puts $fid "/////////////////////////////////////////////////////////////\n"
    puts $fid "// Number of simulation time steps"
    puts $fid "// Example: num_sim_steps: 10e3"
    puts $fid "num_sim_steps: \n"
    puts $fid "// Time step of simulator (in seconds)"
    puts $fid "// Example: Ts: 1/10e9"
    puts $fid "Ts: \n"
    puts $fid "// Output File name"
    puts $fid "// Example:  name below produces $base_name.tr0, $base_name.tr1, ..."
    puts $fid "// Note: you can decimate, start saving at a given time offset, etc."
    puts $fid "//    -> See pages 34-35 of CppSim manual (i.e., output: section)"
    puts $fid "output: $base_name \n"
    puts $fid "// Nodes to be included in Output File"
    puts $fid "// Example: probe: n0 n1 xi12.n3 xi14.xi12.n0"
    puts $fid "probe: \n"
    puts $fid "/////////////////////////////////////////////////////////////"
    puts $fid "// Note:  Items below can be kept unaltered if desired"
    puts $fid "/////////////////////////////////////////////////////////////\n"

    puts $fid "// Numerical integration method for electrical schematics"
    puts $fid "// 1.0: Backward Euler (default)"
    puts $fid "// 0.0: Trap (more accurate, but prone to ringing)"
    puts $fid "electrical_integration_damping_factor: 1.0\n"

    puts $fid "// Values for global nodes used in schematic"
    puts $fid "// Example: global_nodes: gnd=0.0 avdd=1.5 dvdd=1.5"
    puts $fid "global_nodes: \n"

    puts $fid "// Values for global parameters used in schematic"
    puts $fid "// Example: global_param: in_gl=92.1 delta_gl=0.0 step_time_gl=100e3*Ts"
    puts $fid "global_param: \n"

    puts $fid "// Rerun simulation with different global parameter values"
    puts $fid "// Example: alter: in_gl = 90:2:98"
    puts $fid "// See pages 37-38 of CppSim manual (i.e., alter: section)"
    puts $fid "alter: "

    if {[catch {close $fid} err]} {
       tk_dialog_new .cppsim_message "Error in creating Sim File" \
	  "$err" "" 0 {OK} 
       return
    }

    cppSynchronize lite
}

set cur_file "$CUR_DIR/$CUR_SIM_FILE"

cppEditFile $cur_file
}

#########################################################################################################

# Synchronize to Sue2 schematic
proc cppSynchronize {option_string} {
global message_log CPPSIM_HOME CUR_SIM_FILE SIM_FILE_MENU \
      TOP_CELL TOP_LIBRARY TOP_SIM_WINDOW cur_s tcl_platform \
      CPPSIMSHARED_HOME CUR_DIR cppRunMode

if {$tcl_platform(platform) == "windows"} {
     set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin/win32"
} else {
     set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin"
}

catch "exec $sue2_bin_directory/find_sue_module_library $cur_s \
                 $CPPSIM_HOME/Sue2/sue.lib" library_name_message
set last_index [string last "file:" $library_name_message]
set TOP_LIBRARY_raw [string range $library_name_message 0 [expr $last_index - 2]]
set check_priv_lib_flag_index [string first ":Private" $TOP_LIBRARY_raw]
if {$check_priv_lib_flag_index < 1} {
   set TOP_LIBRARY $TOP_LIBRARY_raw
} else {
    set TOP_LIBRARY [string range $TOP_LIBRARY_raw 0 [expr $check_priv_lib_flag_index - 1]]
}

if {[string first "Error" $TOP_LIBRARY] != -1} {
    tk_dialog_new .cppsim_message "Error in finding CppSim module" \
	"$TOP_LIBRARY" "" 0 {OK} 
    return
}
set TOP_CELL $cur_s

if {$cppRunMode == "CppSim"} {
   wm title $TOP_SIM_WINDOW "CppSim Run Menu --- cell: $TOP_CELL, library: $TOP_LIBRARY"
} else {
   wm title $TOP_SIM_WINDOW "VppSim Run Menu --- cell: $TOP_CELL, library: $TOP_LIBRARY"
}

set cur_dir "$CPPSIM_HOME/SimRuns"
if {[file isdirectory $cur_dir] == 0} {
    if {[catch {file mkdir $cur_dir} message]} {
      tk_dialog_new .cppsim_message "Error in creating SimRuns directory" \
	  "$message" "" 0 {OK} 
    }
}
set cur_dir "$CPPSIM_HOME/SimRuns/$TOP_LIBRARY"
if {[file isdirectory $cur_dir] == 0} {
    if {[catch {file mkdir $cur_dir} message]} {
      tk_dialog_new .cppsim_message "Error in creating SimRuns directory" \
	  "$message" "" 0 {OK} 
    }
}
set cur_dir "$CPPSIM_HOME/SimRuns/$TOP_LIBRARY/$TOP_CELL"
if {[file isdirectory $cur_dir] == 0} {
    if {[catch {file mkdir $cur_dir} message]} {
      tk_dialog_new .cppsim_message "Error in creating SimRuns directory" \
	  "$message" "" 0 {OK} 
    }
}
cd $cur_dir
set CUR_DIR $cur_dir

$message_log delete 1.0 end
$message_log insert end "******* cell: $TOP_CELL  (Library: $TOP_LIBRARY) *******\n\n"

if {$option_string == "full"} {
   cppNetlistOnlyRoutine lite
}

if {[catch {glob *.par} message] != 1} {
   $SIM_FILE_MENU delete 0 end
   set count 0
   foreach file [glob *.par] {
      $SIM_FILE_MENU add radiobutton -label $file
       $SIM_FILE_MENU entryconfigure $count -command "set CUR_SIM_FILE $file; cppUpdateHierFile load"
      if {$count == 0} {
           set CUR_SIM_FILE $file
           cppUpdateHierFile load
      }
      set count [expr $count + 1]
   }
} else {
   $SIM_FILE_MENU delete 0 end
   $SIM_FILE_MENU add radiobutton -label "None"
   $SIM_FILE_MENU entryconfigure 0 -command "set CUR_SIM_FILE None"
   set CUR_SIM_FILE "None"
   cppUpdateHierFile load
}

}

#########################################################################################################

# Kill current CppSim run
proc cppKillCppSimCID {} {
global CUR_PID cppRunMode message_log tcl_platform CPPSIM_RUN_FLAG CUR_SIM_FILE

if {$CPPSIM_RUN_FLAG == 0} {
    $message_log insert end "\n$------------------ No process to kill! --------------------\n"
    $message_log see end
    return
}
set CPPSIM_RUN_FLAG 0

set pid_val [lindex [pid $CUR_PID] 0]
set pid_length [llength $pid_val]

if {$tcl_platform(platform) == "windows"} {
   catch {open "|taskkill /PID $pid_val /F /T |& cat"} fid
} else {
   if {$tcl_platform(os) == "Darwin"} {
       if {$cppRunMode == "CppSim"} {
          set base_name [string trimright $CUR_SIM_FILE "par"]
          set base_name [string trimright $base_name "."]
          catch {open "|killall -z $base_name |& cat"} fid
       } else {
          catch {open "|killall -z vvp |& cat"} fid
       }
   } else {
       if {$cppRunMode == "CppSim"} {
          set base_name [string trimright $CUR_SIM_FILE "par"]
          set base_name [string trimright $base_name "."]
          catch {open "|killall -e $base_name |& cat"} fid
       } else {
          catch {open "|killall -e vvp |& cat"} fid
       }
   }
}


set message_input [read $fid]
$message_log insert end "\n$message_input"
$message_log see end

if {[catch {close $fid} err] != 0} {
    $message_log insert end "\n$------------------ Encountered a problem when trying to kill process --------------------\n"
    $message_log insert end $err
    $message_log see end
    return
} else {
    $message_log insert end "\n$------------------ SIMULATION TERMINATED --------------------\n"
    $message_log see end
}

}
#########################################################################################################

# Kill current CppSim run
proc cppClose {} {

global TOP_SIM_WINDOW CPPSIM_RUN_FLAG

if {$CPPSIM_RUN_FLAG == 1} {
   tk_dialog_new .cppsim_message "Error: Current Simulation Run Not Completed" \
	  "Can't close CppSim/VppSim Run Menu or switch between CppSim/VppSim modes while simulation is running.  Hit Kill Run button to end current simulation if desired." "" 0 {OK} 
   return
} else {
   catch {destroy $TOP_SIM_WINDOW}
}
}

# Pops up a window to create/edit CppSim code for a given icon

proc cppModuleOptionsMenu {mod_name mod_code_type code_type avail_code instance_flag} {

  global command cppRunMode tcl_platform

  set command 0

  # Just in case there is an old one around
  catch {destroy .cppsim_menu}

  toplevel .cppsim_menu 
  set w .cppsim_menu

  set x [expr [winfo screenwidth $w]/2 - [winfo reqwidth $w]/2 \
	    - [winfo vrootx [winfo parent $w]]]
  set y [expr [winfo screenheight $w]/2 - [winfo reqheight $w]/2 \
	    - [winfo vrooty [winfo parent $w]]]

  wm geometry .cppsim_menu "+$x+$y"
  wm title .cppsim_menu $mod_name
    
  bind .cppsim_menu <Escape> {set command "" ; cppUpdateHierFile load ; catch {destroy .cppsim_menu}}
  bind .cppsim_menu <Return> {set command "" ; cppUpdateHierFile update ; catch {destroy .cppsim_menu}}

  frame .cppsim_menu.bot_buttons

  frame .cppsim_menu.default -relief sunken -bd 1
  button .cppsim_menu.done -text "Done" -command \
      {set command "" ; cppUpdateHierFile update ; catch {destroy .cppsim_menu}}
  pack .cppsim_menu.done -in .cppsim_menu.default -padx 1m -pady 1m -ipadx 2m
  pack .cppsim_menu.default -side left -in .cppsim_menu.bot_buttons \
      -padx 4m -ipadx 1m -pady 1m -expand 1

  button .cppsim_menu.cancel -text "Cancel" \
      -command {set command "" ; cppUpdateHierFile load ; catch {destroy .cppsim_menu}}
  pack .cppsim_menu.cancel -side left -in .cppsim_menu.bot_buttons \
      -padx 4m -ipadx 2m -pady 1m -expand 1


  label .cppsim_menu.note -text [cppCreateModuleLabel $mod_name] -justify left 
  pack .cppsim_menu.note -side top
  pack .cppsim_menu.bot_buttons -side bottom

  ############################################
  frame .cppsim_menu.buttons -borderwidth 10

  if {$instance_flag == 0} {
     if {$avail_code == "(cppsim,verilog)" || $avail_code == "(cppsim)"} {
         radiobutton .cppsim_menu.buttons.button1 -text "Use CppSim Code" \
            -command "cppUpdateHierFile cppsim" -variable code_type \
            -value cppsim
         pack .cppsim_menu.buttons.button1 -side top -anchor w
         if {$code_type == "c"} {
           .cppsim_menu.buttons.button1 select
         }
     }
     if {$avail_code == "(cppsim,verilog)" || $avail_code == "(verilog)"} {
         radiobutton .cppsim_menu.buttons.button2 -text "Use Verilog Code" \
             -command "cppUpdateHierFile verilog" -variable code_type \
             -value verilog
         pack .cppsim_menu.buttons.button2 -side top -anchor w
         if {$code_type == "v"} {
           .cppsim_menu.buttons.button2 select
         }
     }
     if {$avail_code == "(cppsim,verilog)"} {
        radiobutton .cppsim_menu.buttons.button3 -text "Expand Into Instances" \
            -command "cppUpdateHierFile expand" -variable code_type \
            -value both
        pack .cppsim_menu.buttons.button3 -side top -anchor w
        if {$code_type == "e"} {
            .cppsim_menu.buttons.button3 select
        }
     }
     radiobutton .cppsim_menu.buttons.button4 -text "Ignore Module Code" \
            -command "cppUpdateHierFile ignore" -variable code_type \
            -value ignore
     pack .cppsim_menu.buttons.button4 -side top -anchor w
     if {$code_type == "i"} {
         .cppsim_menu.buttons.button4 select
     }
  } elseif {$mod_code_type == "e"} {
     if {$avail_code == "(cppsim,verilog)" || $avail_code == "(cppsim)"} {
         radiobutton .cppsim_menu.buttons.button1 -text "Use CppSim Code" \
            -command "cppUpdateHierFile cppsim" -variable code_type \
            -value cppsim
         pack .cppsim_menu.buttons.button1 -side top -anchor w
         if {$code_type == "c"} {
           .cppsim_menu.buttons.button1 select
         }
     }
     if {$avail_code == "(cppsim,verilog)" || $avail_code == "(verilog)"} {
         radiobutton .cppsim_menu.buttons.button2 -text "Use Verilog Code" \
               -command "cppUpdateHierFile verilog" -variable code_type \
               -value verilog
         pack .cppsim_menu.buttons.button2 -side top -anchor w
         if {$code_type == "v"} {
            .cppsim_menu.buttons.button2 select
         }
     }
  } else {
      if {$cppRunMode == "Verilog" && $avail_code == "(cppsim,verilog)"} {
         label .cppsim_menu.buttons.label -text "Instance code type overridden by module code type '$mod_code_type'\n --- Change module code type to 'e' (i.e., Expanded) to modify instance code type ---"
      } else {
         label .cppsim_menu.buttons.label -text "Instance code type overridden by module code type '$mod_code_type'\n --- Instance code type cannot be independently changed in this case ---"
      }
     pack .cppsim_menu.buttons.label -side top -anchor w
  }
  
  ############################################
  pack .cppsim_menu.buttons -side top

  cppCreateCodeButtons $mod_name .cppsim_menu

  # Border pixels for X windows.  Can't seem to figure these out.
  set XBORDER 3
  set YBORDER 25

  # If the prop_menu floats off the screen, move it back on.
  set dx [min [expr [winfo screenwidth .cppsim_menu]-[winfo width .cppsim_menu]- \
		   $x-$XBORDER] 0]
  set dy [min [expr [winfo screenheight .cppsim_menu]-[winfo height .cppsim_menu]- \
		   $y-$YBORDER] 0]
  if {$dx < 0 || $dy < 0} {
    wm geometry .cppsim_menu "+[expr $x+$dx]+[expr $y+$dy]"    
  }
   
  if {$tcl_platform(os) == "Darwin" || $tcl_platform(platform) == "windows"} {
      grab set .cppsim_menu
  }

  vwait command
  return $command
}
