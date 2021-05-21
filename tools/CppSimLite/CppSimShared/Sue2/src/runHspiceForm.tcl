#### Procedures for Hspice Simulation Window

proc create_run_hspice_form {} {

global CPPSIM_HOME CPPSIMSHARED_HOME \
       CUR_SIM_FILE SIM_FILE_MENU TOP_LIBRARY TOP_CELL EDITOR \
       TOP_HSPSIM_WINDOW message_log cur_s tcl_platform HSPICE_RUN_FLAG

catch {destroy $TOP_HSPSIM_WINDOW}

set HSPICE_RUN_FLAG 0
toplevel .hspRunMenu 
set TOP_HSPSIM_WINDOW .hspRunMenu

set x [expr [winfo screenwidth $TOP_HSPSIM_WINDOW]/2 - [winfo reqwidth $TOP_HSPSIM_WINDOW]/2 \
	    - [winfo vrootx [winfo parent $TOP_HSPSIM_WINDOW]]]
set y [expr [winfo screenheight $TOP_HSPSIM_WINDOW]/2 - [winfo reqheight $TOP_HSPSIM_WINDOW]/2 \
	    - [winfo vrooty [winfo parent $TOP_HSPSIM_WINDOW]]]

wm geometry .hspRunMenu "+$x+$y"
# wm title .hspRunMenu "Hspice Run Menu --- cell: $TOP_CELL, library: $TOP_LIBRARY"
    
bind .hspRunMenu <Escape> {catch {destroy .hspRunMenu} ; return}

# Create a frame for buttons and entry.
frame .hspRunMenu.button_frame -borderwidth 10

pack .hspRunMenu.button_frame -side top -fill x
# Create the command buttons.
button .hspRunMenu.button_frame.compile_run -padx 5 -text "Netlist/HSPC/Hspice" -command hspRunRoutine
button .hspRunMenu.button_frame.netlisthspc -padx 5 -text "Netlist/HSPC" -command "hspNetlistHSPCRoutine full"
button .hspRunMenu.button_frame.netlist -padx 5 -text "Netlist Only" -command "hspNetlistOnlyRoutine full"
button .hspRunMenu.button_frame.edit_sim_file -padx 5 -text "Edit Sim File" -command hspEditRoutine
button .hspRunMenu.button_frame.synchronize -padx 5 -text "Synchronize" -command "hspSynchronize lite"
button .hspRunMenu.button_frame.kill_run -padx 5 -text "Kill Run" -command hspKillHspSimCID
button .hspRunMenu.button_frame.close -padx 5 -text "Close" -command hspClose


pack .hspRunMenu.button_frame.close .hspRunMenu.button_frame.kill_run .hspRunMenu.button_frame.synchronize .hspRunMenu.button_frame.edit_sim_file .hspRunMenu.button_frame.netlist .hspRunMenu.button_frame.netlisthspc .hspRunMenu.button_frame.compile_run -side left -padx 1





# Create a labeled entry for the Ts: command
#frame .hspRunMenu.ts_frame
#label .hspRunMenu.ts_frame.ts_label -text "Ts:         " -padx 1
#entry .hspRunMenu.ts_frame.ts_entry -width 20 -relief sunken \
#-textvariable ts_value
#pack .hspRunMenu.ts_frame.ts_label -side left
#pack .hspRunMenu.ts_frame.ts_entry -side left -fill x -expand true
#pack .hspRunMenu.ts_frame -side top -fill x

# Create menu options for sim file
frame .hspRunMenu.sim_file_frame
label .hspRunMenu.sim_file_frame.file_label -text "Sim File:   " -padx 1
set SIM_FILE_MENU [tk_optionMenu .hspRunMenu.sim_file_frame.file_options CUR_SIM_FILE "None"]
pack .hspRunMenu.sim_file_frame.file_label -side left
pack .hspRunMenu.sim_file_frame.file_options -side left -fill x
pack .hspRunMenu.sim_file_frame -side top -fill x

# Set up key binding equivalents to the buttons
# bind .hspRunMenu.top.cmd <Return> Run
# bind .hspRunMenu.top.cmd <Control-c> Stop
# focus .hspRunMenu.top.cmd

# Create a text widget to log the output
frame .hspRunMenu.message_frame -pady 10
label .hspRunMenu.message_frame.label -text "Result:   " -padx 1
set message_log [text .hspRunMenu.message_frame.message -width 80 -height 14 \
-borderwidth 2 -relief raised -setgrid true \
-yscrollcommand {.hspRunMenu.message_frame.yscroll set} \
-xscrollcommand {.hspRunMenu.message_frame.xscroll set}]
scrollbar .hspRunMenu.message_frame.yscroll -command {.hspRunMenu.message_frame.message yview} -orient vertical -relief sunken
scrollbar .hspRunMenu.message_frame.xscroll -command {.hspRunMenu.message_frame.message xview} -orient horizontal -relief sunken

pack .hspRunMenu.message_frame.label -anchor nw -side left
pack .hspRunMenu.message_frame.xscroll -side bottom -fill x
pack .hspRunMenu.message_frame.yscroll -side right -fill y
pack .hspRunMenu.message_frame.message -side left -fill both -expand true
pack .hspRunMenu.message_frame -side top -fill both -expand true


hspSynchronize lite

}


# Run the program and arrange to read its messages
proc hspRunRoutine {} {

global message_log CPPSIMSHARED_HOME CPPSIM_HOME tcl_platform HSPICE_RUN_FLAG \
       CUR_H_PID CUR_SIM_FILE TOP_CELL TOP_LIBRARY CUR_DIR 

if {$HSPICE_RUN_FLAG == 1} {
    return
}

if {[auto_execok hspice] == ""} {
   $message_log delete 1.0 end
   $message_log insert end "Error:  hspice command not found!\n"
   $message_log insert end "      -> make sure that you have installed Hspice and\n"
   $message_log insert end "         included it in your path\n"
   $message_log insert end "\n------------------ Error: running hspice failed --------------------\n"
   $message_log see end
   return
}

set HSPICE_RUN_FLAG 1

$message_log delete 1.0 end

$message_log insert end "******* cell: $TOP_CELL  (Library: $TOP_LIBRARY) *******\n\n"

hspNetlistHSPCRoutine lite

set netlist_dir "$CPPSIM_HOME/SimRuns/$TOP_LIBRARY/$TOP_CELL"

if {[file isfile "netlist.hspsim"] == 0} {
   $message_log insert end "Error:  Netlist file is missing!\n"
   $message_log insert end "      (i.e., can't open file '$netlist_dir/netlist.hspsim')\n"
   $message_log insert end "\n------------------ Error: running netlister failed --------------------\n"
   $message_log see end
   return
}

if {[file isfile "simrun.sp"] == 0} {
   $message_log insert end "Error:  simrun.sp file is missing!\n"
   $message_log insert end "      (i.e., can't open file '$netlist_dir/netlist.hspsim')\n"
   $message_log insert end "\n------------------ Error: running HSPC failed --------------------\n"
   $message_log see end
   return
}

$message_log insert end "\n-------------- running hspice program --------------\n\n"

$message_log see end

if {[catch {open "|hspice -i simrun.sp -o simrun.log |& cat"} CUR_H_PID] == 0} {
    fileevent $CUR_H_PID readable hspMessageLog
} else {
    $message_log insert end "\n------------------ Error:  hspice command failed! --------------------\n"
    $message_log see end
}

$message_log insert end "\n---> Output directory:  $CPPSIM_HOME/SimRuns/$TOP_LIBRARY/$TOP_CELL\n"
$message_log insert end "\n*********************  Done!  *********************\n"
$message_log see end

}


# Read and log output from the program
proc hspMessageLog {} {
global CUR_H_PID message_log CPPSIM_HOME TOP_CELL TOP_LIBRARY \
       HSPICE_RUN_FLAG

if [eof $CUR_H_PID] {
    catch {close $CUR_H_PID}
    $message_log insert end "Note:  run files contained in directory:\n     $CPPSIM_HOME/SimRuns/$TOP_LIBRARY/$TOP_CELL\n"
    $message_log see end
    set HSPICE_RUN_FLAG 0
} else {
gets $CUR_H_PID line
$message_log insert end $line\n
$message_log see end
}
}

# Read and log output from the program
proc hspMessageLogUSRP {} {
global CUR_H_PID message_log CPPSIM_HOME TOP_CELL TOP_LIBRARY \
       HSPICE_RUN_FLAG

if [eof $CUR_H_PID] {
    catch {close $CUR_H_PID}
    $message_log insert end "\n****** Note:  to compile mex function within Matlab: ******\n"
    $message_log insert end "> cd $CPPSIM_HOME/SimRuns/$TOP_LIBRARY/$TOP_CELL/Matlab\n"
    $message_log insert end "> compile_$TOP_CELL\n"
    $message_log insert end "\n****** Note:  to run mex function within Matlab: ******\n"
    $message_log insert end "> addpath('$CPPSIM_HOME/SimRuns/$TOP_LIBRARY/$TOP_CELL/Matlab')\n"
    $message_log insert end "> system('initialize_usrp')  %% (Initializes USRP board) %%\n"
    $message_log insert end "> $TOP_CELL  %% (shows input and output signals) %%\n"

    $message_log insert end "\n*********************  Done!  *********************\n"
    $message_log see end

    set HSPICE_RUN_FLAG 0
} else {
gets $CUR_H_PID line
$message_log insert end $line\n
$message_log see end
}
}


#########################################################################################################

# Netlist only routine
proc hspNetlistOnlyRoutine {option_string} {


global message_log CPPSIMSHARED_HOME CPPSIM_HOME tcl_platform \
       CUR_SIM_FILE TOP_CELL TOP_LIBRARY

if {$option_string == "full"} {
   $message_log delete 1.0 end
   $message_log insert end "******* cell: $TOP_CELL  (Library: $TOP_LIBRARY) *******\n\n"
}
$message_log insert end "----------------- running netlister -----------------\n\n"




#set netlist_dir "$CPPSIM_HOME/Netlist"
set netlist_dir "$CPPSIM_HOME/SimRuns/$TOP_LIBRARY/$TOP_CELL"
if {[file isdirectory $netlist_dir] == 0} {

   $message_log insert end "Error:  HspSim does not have a Netlist directory!\n"
   $message_log insert end "      (i.e., can't open directory '$netlist_dir')\n"
   $message_log insert end "      ->  create this directory to correct this problem\n"
   $message_log insert end "\n------------------ Netlisting Exited with Errors --------------------\n"
   $message_log see end
   return
}

if {[file isfile "$netlist_dir/netlist.hspsim"] != 0} {
    if {[catch {file delete "$netlist_dir/netlist.hspsim"} mssg]} {
        $message_log insert end "Error:  cannot delete netlist file!\n"
        $message_log insert end "     (i.e., cannot delete '$netlist_dir/netlist.hspsim')\n"
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

catch {open "|$sue2_bin_directory/sue_spice_netlister $TOP_CELL \
        $CPPSIM_HOME/Sue2/sue.lib $netlist_dir/netlist.hspsim no_top |& cat"} fid
set message_input [read $fid]
$message_log insert end $message_input
$message_log see end

if {[catch {close $fid} err] != 0 || [file isfile "$netlist_dir/netlist.hspsim"] == 0} {
    $message_log insert end "\n------------------ Netlisting Exited with Errors --------------------\n"
    $message_log see end
    return
}

$message_log insert end "--> Netlist location:  $CPPSIM_HOME/SimRuns/$TOP_LIBRARY/$TOP_CELL/netlist.hspsim\n"

if {$option_string == "full"} {
    if {$CUR_SIM_FILE == "None"} {
       $message_log insert end "\n\n****** Note:  you should create a SimRun file ******\n\n"
   } else {
     if {$tcl_platform(platform) != "windows"} {
        $message_log insert end "\n****** Note:  to run Hspice from UNIX shell: ******\n"
        $message_log insert end "> cd $CPPSIM_HOME/SimRuns/$TOP_LIBRARY/$TOP_CELL\n"
        $message_log insert end "> $CPPSIMSHARED_HOME/HspiceToolbox/HSPC/bin/hspc netlist.hspsim simrun.sp $CUR_SIM_FILE\n"
        $message_log insert end "> hspice -i simrun.sp -o simrun.log\n"
     } else {
        $message_log insert end "\n****** Note:  to run Hspice from Windows command window: ******\n"
        $message_log insert end "> cd $CPPSIM_HOME/SimRuns/$TOP_LIBRARY/$TOP_CELL\n"
        $message_log insert end "> $CPPSIMSHARED_HOME/HspiceToolbox/HSPC/bin/win32/hspc netlist.hspsim simrun.sp $CUR_SIM_FILE\n"
        $message_log insert end "> hspice -i simrun.sp -o simrun.log\n"
     }
     $message_log insert end "\n****** Note:  to run Hspice within Matlab: ******\n"
     $message_log insert end "> addpath('$CPPSIMSHARED_HOME/HspiceToolbox');\n"
     $message_log insert end "> cd $CPPSIM_HOME/SimRuns/$TOP_LIBRARY/$TOP_CELL\n"
     if {$CUR_SIM_FILE == "test.hspc"} {
        $message_log insert end "> hspsim\n\n"
     } else {
        $message_log insert end "> hspsim('$CUR_SIM_FILE')\n\n"
     }
   }
   $message_log insert end "*********************  Done!  *********************\n"
    $message_log see end
}
}

#########################################################################################################


#########################################################################################################

# Netlist and HSPC routines
proc hspNetlistHSPCRoutine {option_string} {


global message_log CPPSIMSHARED_HOME CPPSIM_HOME tcl_platform \
       CUR_SIM_FILE TOP_CELL TOP_LIBRARY

if {$CUR_SIM_FILE == "None"} {
    $message_log insert end "\n\n****** Error:  you must create a SimRun file to run HSPC ******\n\n"
    $message_log insert end "\n------------------ HSPC Exited with Errors --------------------\n"
    $message_log see end
    return
} 

if {$option_string == "full"} {
   $message_log delete 1.0 end
   $message_log insert end "******* cell: $TOP_CELL  (Library: $TOP_LIBRARY) *******\n\n"
}

hspNetlistOnlyRoutine lite

$message_log insert end "\n----------------- running hspc -----------------\n\n"


set netlist_dir "$CPPSIM_HOME/SimRuns/$TOP_LIBRARY/$TOP_CELL"
if {[file isdirectory $netlist_dir] == 0} {

   $message_log insert end "Error:  HSPC does not have a Netlist directory!\n"
   $message_log insert end "      (i.e., can't open directory '$netlist_dir')\n"
   $message_log insert end "      ->  create this directory to correct this problem\n"
   $message_log insert end "\n------------------ HSPC Exited with Errors --------------------\n"
   $message_log see end
   return
}

if {[file isfile "$netlist_dir/netlist.hspsim"] == 0} {
   $message_log insert end "Error:  HSPC does not have a netlist file!\n"
   $message_log insert end "      (i.e., can't open file '$netlist_dir/netlist.hspsim')\n"
   $message_log insert end "\n------------------ HSPC Exited with Errors --------------------\n"
   $message_log see end
   return
}

if {$tcl_platform(platform) == "windows"} {
   set hspc_bin_directory "$CPPSIMSHARED_HOME/HspiceToolbox/HSPC/bin/win32"
} else {
   set hspc_bin_directory "$CPPSIMSHARED_HOME/HspiceToolbox/HSPC/bin"
}

catch {open "|$hspc_bin_directory/hspc $netlist_dir/netlist.hspsim $netlist_dir/simrun.sp \
        $netlist_dir/$CUR_SIM_FILE |& cat"} fid
set message_input [read $fid]
$message_log insert end $message_input
$message_log see end

if {[catch {close $fid} err] != 0 || [file isfile "$netlist_dir/simrun.sp"] == 0} {
    $message_log insert end "\n------------------ HSPC Exited with Errors --------------------\n"
    $message_log see end
    return
}

$message_log insert end "Running HSPC on cell '$TOP_CELL' completed with no errors\n"
$message_log insert end "---> HSPC output location:  $CPPSIM_HOME/SimRuns/$TOP_LIBRARY/$TOP_CELL/simrun.sp\n"

if {$option_string == "full"} {
     if {$tcl_platform(platform) != "windows"} {
        $message_log insert end "\n****** Note:  to run Hspice from UNIX shell: ******\n"
        $message_log insert end "> cd $CPPSIM_HOME/SimRuns/$TOP_LIBRARY/$TOP_CELL\n"
        $message_log insert end "> hspice -i simrun.sp -o simrun.log\n"
     } else {
        $message_log insert end "\n****** Note:  to run Hspice from Windows command window: ******\n"
        $message_log insert end "> cd $CPPSIM_HOME/SimRuns/$TOP_LIBRARY/$TOP_CELL\n"
        $message_log insert end "> hspice -i simrun.sp -o simrun.log\n"
     }
     $message_log insert end "\n****** Note:  to run Hspice within Matlab: ******\n"
     $message_log insert end "> addpath('$CPPSIMSHARED_HOME/HspiceToolbox');\n"
     $message_log insert end "> cd $CPPSIM_HOME/SimRuns/$TOP_LIBRARY/$TOP_CELL\n"
     if {$CUR_SIM_FILE == "test.hspc"} {
        $message_log insert end "> hspsim\n\n"
     } else {
        $message_log insert end "> hspsim('$CUR_SIM_FILE')\n\n"
     }
   
   $message_log insert end "*********************  Done!  *********************\n"
    $message_log see end

}
}

#########################################################################################################


# Edit Sim File
proc hspEditRoutine {} {

global CPPSIM_HOME CPPSIMSHARED_HOME  \
       CUR_SIM_FILE message_log TOP_CELL TOP_LIBRARY CUR_DIR


#$message_log delete 1.0 end


if {$CUR_SIM_FILE == "None"} {

    set CUR_SIM_FILE test.hspc
    set cur_file "$CUR_DIR/$CUR_SIM_FILE"
    set base_name [string trimright $CUR_SIM_FILE "hpar"]
    set base_name [string trimright $base_name "."]

    ####### Open sim file
    if {[catch "set fid \[open $cur_file w\]" err]} {
       tk_dialog_new .hspsim_message "Error in creating Sim File" \
	  "$err" "" 0 {OK} 
       return
    }

    puts $fid "** Hspice Simulation of '$TOP_CELL' (Lib: $TOP_LIBRARY)\n"

puts $fid "***** Note:   to understand non-Hspice commands below,"
puts $fid "*****         look at HSPC manual at http://www.cppsim.com\n"

puts $fid "***** Parameters for Calculation of Diffusion Regions for CMOS Process"
puts $fid "> set_mode_diff geo"
puts $fid "> set_hdout .66u"
puts $fid "> set_hdin .74u"
puts $fid "> use_four_sided_perimeter\n"

puts $fid "***** Transistor Model File"
puts $fid ".prot"
puts $fid "***** Use Appropriate CMOS model file below"
puts $fid ".include 'mos018.mod'"
puts $fid ".unprot\n"

puts $fid "***** Temperature"
puts $fid ".temp 25\n"

puts $fid "***** Parameters"
puts $fid ".param vsupply=1.8\n"

puts $fid "***** DC Voltage Sources"
puts $fid "Vsup vdd 0 vsupply"
puts $fid "Vgnd gnd 0 0\n"

puts $fid "***** Simulation Options"
puts $fid ".options post=1 delmax=5p relv=1e-6 reli=1e-6 relmos=1e-6 method=gear\n"

puts $fid "***** Simulation Parameters (.tran or .ac or .dc statements)"
puts $fid ".tran 5p 40n\n"

puts $fid "***** Have operating point information sent to output file"
puts $fid ".op\n"

puts $fid "****** Digital Input Stimulus (remove lines below if you don't want this)"
puts $fid "*** Timing Statement:  % timing delay rise/fall_time period vlow vhigh"
puts $fid "% timing 0.0n .2n \[1/1e9] 0 vsupply"
puts $fid "*** Digital Input:  % input nodename \[set 0 1 1 0 1 ... R]"
puts $fid "% input in \[set 0 0 1 1 1 0 1 1 0 0 0 1 0 R]\n"

puts $fid "****** Analog Input Stimulus"
puts $fid "*** Simply add voltage or current sources as appropriate\n"

puts $fid "****** Selectively Probe Signals (uncomment below if you want this)"
puts $fid "*.options probe"
puts $fid "*.probe tran v(in) v(out)"

    if {[catch {close $fid} err]} {
       tk_dialog_new .hspsim_message "Error in creating Sim File" \
	  "$err" "" 0 {OK} 
       return
    }

    hspSynchronize lite
}

set cur_file "$CUR_DIR/$CUR_SIM_FILE"

cppEditFile $cur_file
}

#########################################################################################################

# Synchronize to Sue2 schematic
proc hspSynchronize {option_string} {
global message_log CPPSIM_HOME CUR_SIM_FILE SIM_FILE_MENU \
      TOP_CELL TOP_LIBRARY TOP_HSPSIM_WINDOW cur_s tcl_platform \
      CPPSIMSHARED_HOME CUR_DIR

if {$tcl_platform(platform) == "windows"} {
     set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin/win32"
} else {
     set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin"
}

catch "exec $sue2_bin_directory/find_sue_module_library $cur_s \
                 $CPPSIM_HOME/Sue2/sue.lib" library_name_message
set last_index [string last "file:" $library_name_message]
set TOP_LIBRARY [string range $library_name_message 0 [expr $last_index - 2]]

if {[string first "Error" $TOP_LIBRARY] != -1} {
    tk_dialog_new .hspsim_message "Error in finding HspSim module" \
	"$TOP_LIBRARY" "" 0 {OK} 
    return
}
set TOP_CELL $cur_s

wm title $TOP_HSPSIM_WINDOW "Hspice Run Menu --- cell: $TOP_CELL, library: $TOP_LIBRARY"

set cur_dir "$CPPSIM_HOME/SimRuns"
if {[file isdirectory $cur_dir] == 0} {
    if {[catch {file mkdir $cur_dir} message]} {
      tk_dialog_new .hspsim_message "Error in creating SimRuns directory" \
	  "$message" "" 0 {OK} 
    }
}
set cur_dir "$CPPSIM_HOME/SimRuns/$TOP_LIBRARY"
if {[file isdirectory $cur_dir] == 0} {
    if {[catch {file mkdir $cur_dir} message]} {
      tk_dialog_new .hspsim_message "Error in creating SimRuns directory" \
	  "$message" "" 0 {OK} 
    }
}
set cur_dir "$CPPSIM_HOME/SimRuns/$TOP_LIBRARY/$TOP_CELL"
if {[file isdirectory $cur_dir] == 0} {
    if {[catch {file mkdir $cur_dir} message]} {
      tk_dialog_new .hspsim_message "Error in creating SimRuns directory" \
	  "$message" "" 0 {OK} 
    }
}
cd $cur_dir
set CUR_DIR $cur_dir

$message_log delete 1.0 end
$message_log insert end "******* cell: $TOP_CELL  (Library: $TOP_LIBRARY) *******\n\n"

if {$option_string == "full"} {
   hspNetlistOnlyRoutine lite
}

if {[catch {glob *.hspc} message] != 1} {
   $SIM_FILE_MENU delete 0 end
   set count 0
   foreach file [glob *.hspc] {
      $SIM_FILE_MENU add radiobutton -label $file
       $SIM_FILE_MENU entryconfigure $count -command "set CUR_SIM_FILE $file"
      if {$count == 0} {
           set CUR_SIM_FILE $file
      }
      set count [expr $count + 1]
   }
} else {
   $SIM_FILE_MENU delete 0 end
   $SIM_FILE_MENU add radiobutton -label "None"
   $SIM_FILE_MENU entryconfigure 0 -command "set CUR_SIM_FILE None"
   set CUR_SIM_FILE "None"
}

}

#########################################################################################################

# Kill current HspSim run
proc hspKillHspSimCID {} {
global CUR_H_PID message_log tcl_platform HSPICE_RUN_FLAG

if {$HSPICE_RUN_FLAG == 0} {
    $message_log insert end "\n$------------------ No process to kill! --------------------\n"
    $message_log see end
    return
}
set HSPICE_RUN_FLAG 0

set pid_val [lindex [pid $CUR_H_PID] 0]
set pid_length [llength $pid_val]

if {$tcl_platform(platform) == "windows"} {
   catch {open "|taskkill /PID $pid_val /F /T |& cat"} fid
} else {
   catch {open "|kill -9 $pid_val |& cat"} fid
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

# Kill current HspSim run
proc hspClose {} {

global TOP_HSPSIM_WINDOW

catch {destroy $TOP_HSPSIM_WINDOW}
}

