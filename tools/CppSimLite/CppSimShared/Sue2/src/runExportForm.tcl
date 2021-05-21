#### Procedures for CppSim Export Tool


proc create_run_export_form {} {

global CPPSIM_HOME CPPSIMSHARED_HOME export_destination_file \
       TOP_EXPORT_WINDOW message_export_log cur_s tcl_platform  \
       EXPORT_LIB_LISTBOX EXPORT_RUN_FLAG

catch {destroy $TOP_EXPORT_WINDOW}
set import_source_dir "$CPPSIM_HOME/Import_Export"
set export_destination_file "NewLib"

toplevel .cppExportForm 
set TOP_EXPORT_WINDOW .cppExportForm
set EXPORT_RUN_FLAG 0

set x [expr [winfo screenwidth $TOP_EXPORT_WINDOW]/2 - [winfo reqwidth $TOP_EXPORT_WINDOW]/2 \
	    - [winfo vrootx [winfo parent $TOP_EXPORT_WINDOW]]]
set y [expr [winfo screenheight $TOP_EXPORT_WINDOW]/2 - [winfo reqheight $TOP_EXPORT_WINDOW]/2 \
	    - [winfo vrooty [winfo parent $TOP_EXPORT_WINDOW]]]

wm geometry .cppExportForm "+$x+$y"
wm title .cppExportForm "Export CppSim Library"
    
bind .cppExportForm <Escape> {catch {destroy .cppExportForm} ; return}

# Create a frame for buttons and entry.
frame .cppExportForm.button_frame -borderwidth 10

pack .cppExportForm.button_frame -side top -fill x
# Create the command buttons.
button .cppExportForm.button_frame.export -text "Export" -width 15 -command "cppRunExport"
button .cppExportForm.button_frame.close -padx 5 -text "Close" -command cppExportClose
pack .cppExportForm.button_frame.close  .cppExportForm.button_frame.export -side left -padx 2

frame .cppExportForm.select_destination_frame
label .cppExportForm.select_destination_frame.label -text "Destination File:" -padx 1
entry .cppExportForm.select_destination_frame.destination_library_entry -width 58 -relief sunken \
-textvariable export_destination_file
label .cppExportForm.select_destination_frame.label2 -text ".tar.gz" -padx 1

pack .cppExportForm.select_destination_frame.label -side left -padx 5
pack .cppExportForm.select_destination_frame.destination_library_entry -side left -fill x
pack .cppExportForm.select_destination_frame.label2 -side left -padx 1
pack .cppExportForm.select_destination_frame -side top -fill x -pady 2


# Create menu options for sim file

frame .cppExportForm.export_file_frame -width 30
label .cppExportForm.export_file_frame.label -text "Source Library:  " -padx 1 
set EXPORT_LIB_LISTBOX [listbox .cppExportForm.export_file_frame.files \
   -height 7 -width 20 \
   -yscrollcommand {.cppExportForm.export_file_frame.yscroll set} \
   -xscrollcommand {.cppExportForm.export_file_frame.xscroll set}] 

scrollbar .cppExportForm.export_file_frame.yscroll -command {.cppExportForm.export_file_frame.files yview} -orient vertical -relief sunken
scrollbar .cppExportForm.export_file_frame.xscroll -command {.cppExportForm.export_file_frame.files xview} -orient horizontal -relief sunken 

pack .cppExportForm.export_file_frame.label -side left -anchor nw 
#pack .cppExportForm.export_file_frame.files -side left -expand true
pack .cppExportForm.export_file_frame.xscroll -side bottom -fill x
pack .cppExportForm.export_file_frame.yscroll -side right -fill y
pack .cppExportForm.export_file_frame.files -side left -fill both -expand true


pack .cppExportForm.export_file_frame -side top -fill both -expand true -pady 2 -padx 5

bind $EXPORT_LIB_LISTBOX <ButtonRelease-1> "cppUpdateExportDestFile"
focus $EXPORT_LIB_LISTBOX

# Create a text widget to log the output
frame .cppExportForm.message_frame -pady 10
label .cppExportForm.message_frame.label -text "Result:  " -padx 1
set message_export_log [text .cppExportForm.message_frame.message -width 80 -height 14 \
-borderwidth 2 -relief raised -setgrid true \
-yscrollcommand {.cppExportForm.message_frame.yscroll set} \
-xscrollcommand {.cppExportForm.message_frame.xscroll set}]
scrollbar .cppExportForm.message_frame.yscroll -command {.cppExportForm.message_frame.message yview} -orient vertical -relief sunken
scrollbar .cppExportForm.message_frame.xscroll -command {.cppExportForm.message_frame.message xview} -orient horizontal -relief sunken

pack .cppExportForm.message_frame.label -anchor nw -side left
pack .cppExportForm.message_frame.xscroll -side bottom -fill x
pack .cppExportForm.message_frame.yscroll -side right -fill y
pack .cppExportForm.message_frame.message -side left -fill both -expand true
pack .cppExportForm.message_frame -side top -fill both -expand true -padx 5

cppExportFileEntries
}

proc cppUpdateExportDestFile {} {
global EXPORT_LIB_LISTBOX export_destination_file

set cur_index [$EXPORT_LIB_LISTBOX curselection]
if {$cur_index != ""} {
    set export_library_raw [$EXPORT_LIB_LISTBOX get $cur_index]
    set base_index [string first :Private $export_library_raw]
    if {$base_index != -1} {
	set export_library [string range $export_library_raw 0 [expr $base_index - 1]]
    } else {
        set export_library $export_library_raw
    }
    set export_destination_file [string tolower $export_library]
}
}


# CppSim Fill In Library Entries
proc cppExportFileEntries {} {

global CPPSIM_HOME CPPSIMSHARED_HOME EXPORT_LIB_LISTBOX export_destination_file

$EXPORT_LIB_LISTBOX delete 0 end
set sue_lib_filename $CPPSIM_HOME/Sue2/sue.lib

catch {open "$sue_lib_filename"} fid
set cur_index 0
while {[gets $fid line] >= 0} {
   set dir_name [cpp_public_or_private_library $line] 
   if {$dir_name != ""} {
      $EXPORT_LIB_LISTBOX insert end $dir_name
       set cur_index [expr $cur_index + 1]
   }
}
catch {close $fid} err
if {$cur_index > 0} {
    set cur_index 0
    $EXPORT_LIB_LISTBOX selection set $cur_index
    $EXPORT_LIB_LISTBOX activate $cur_index
}
set cur_index [$EXPORT_LIB_LISTBOX curselection]
if {$cur_index != ""} {
    set export_library_raw [$EXPORT_LIB_LISTBOX get $cur_index]
    set base_index [string first :Private $export_library_raw]
    if {$base_index != -1} {
	set export_library [string range $export_library_raw 0 [expr $base_index - 1]]
    } else {
        set export_library $export_library_raw
    }
    set export_destination_file [string tolower $export_library]
}
}

proc cpp_public_or_private_library {lib_name} {

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

proc cpp_first_cell_in_library {lib_name} {

global CPPSIM_HOME CPPSIMSHARED_HOME message_export_log

set private_suelib_dir $CPPSIM_HOME/SueLib
set shared_suelib_dir $CPPSIMSHARED_HOME/SueLib

set private_dir_exists [file isdirectory $private_suelib_dir/$lib_name]

set shared_dir_exists [file isdirectory $shared_suelib_dir/$lib_name]

if {$private_dir_exists} {
   set file_list [glob -nocomplain -tails -directory $private_suelib_dir/$lib_name *.sue]
   set num_files [llength $file_list]
   if {$num_files < 1} {
       if {$shared_dir_exists} {
           $message_export_log insert end "\n------------------ Error: No .sue files in library '$lib_name:Private' --------------------\n"
       } else {
           $message_export_log insert end "\n------------------ Error: No .sue files in library '$lib_name' --------------------\n"
       }
      $message_export_log insert end "\n------------------ Exporting Exited with Errors --------------------\n"
      $message_export_log see end
      return ""
   }
   set first_sue_file [lindex $file_list 0]
   return $first_sue_file
} else {
   if {$shared_dir_exists} {
      set file_list [glob -nocomplain -tails -directory $shared_suelib_dir/$lib_name *.sue]
      set num_files [llength $file_list]
      if {$num_files < 1} {
         $message_export_log insert end "\n------------------ Error: No .sue files in library '$lib_name' --------------------\n"
         $message_export_log insert end "\n------------------ Exporting Exited with Errors --------------------\n"
         $message_export_log see end
         return ""
      }
      set first_sue_file [lindex $file_list 0]
      return $first_sue_file

   } else {
      $message_export_log insert end "\n------------------ Error: Can't locate library '$lib_name' in the file system --------------------\n"
      $message_export_log insert end "\n------------------ Exporting Exited with Errors --------------------\n"
      $message_export_log see end
      return ""
   }
}
}


# CppSim Export routine
proc cppRunExport {} {

global message_export_log CPPSIMSHARED_HOME CPPSIM_HOME tcl_platform \
       export_destination_file EXPORT_LIB_LISTBOX EXPORT_RUN_FLAG\

if {$EXPORT_RUN_FLAG == 1} {
    return
}
set EXPORT_RUN_FLAG 1

if {$tcl_platform(platform) == "windows"} {
   set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin/win32"
} else {
   set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin"
}

set cur_index [$EXPORT_LIB_LISTBOX curselection]
if {$cur_index != ""} {
    set export_library_raw [$EXPORT_LIB_LISTBOX get $cur_index]
    set base_index [string first :Private $export_library_raw]
    if {$base_index != -1} {
	set export_library [string range $export_library_raw 0 [expr $base_index - 1]]
    } else {
        set export_library $export_library_raw
    }
} else {
    $message_export_log delete 1.0 end
    $message_export_log insert end "----------------- Error:  must select 'Source Library:' -----------------\n\n"
    $message_export_log see end
    set EXPORT_RUN_FLAG 0
    return
}

   $message_export_log delete 1.0 end
$message_export_log insert end "----------------- Running Export of Library '[cpp_public_or_private_library $export_library]' -----------------\n\n"
   $message_export_log see end


set first_sue_file [cpp_first_cell_in_library $export_library]
if {$first_sue_file == ""} {
    set EXPORT_RUN_FLAG 0
    return
}

set sue_lib_file $CPPSIM_HOME/Sue2/sue.lib

catch {open "|$sue2_bin_directory/export_cppsim_library $first_sue_file $sue_lib_file all |& cat"} fid
set message_input [read $fid]
$message_export_log insert end $message_input
$message_export_log see end

if {[catch {close $fid} err] != 0} {
      $message_export_log insert end "\n------------------ Exporting Exited with Errors --------------------\n"
      $message_export_log see end
      set EXPORT_RUN_FLAG 0
      return
} 

   if {$tcl_platform(platform) == "windows"} {
      set tar_bin_file "$CPPSIMSHARED_HOME/Msys/bin/tar"
   } else {
      set tar_bin_file "tar"
   }

set cur_directory [pwd]
cd $CPPSIM_HOME/Import_Export

$message_export_log insert end "\n... Creating file '$export_destination_file.tar.gz ...\n\n"
$message_export_log see end

catch {open "|$tar_bin_file cfz $export_destination_file.tar.gz ./SueLib ./CadenceLib ./SimRuns  |& cat"} fid
set message_input [read $fid]
$message_export_log insert end $message_input
$message_export_log see end

cd $cur_directory

if {[catch {close $fid} err] != 0} {
      $message_export_log insert end "\n------------------ Exporting Exited with Errors --------------------\n"
      $message_export_log see end
} else {
      $message_export_log insert end "\n------------------ File '$export_destination_file.tar.gz' was successfully created --------------------\n"
      $message_export_log insert end "------------------ in directory '$CPPSIM_HOME/Import_Export' --------------------\n"
      $message_export_log see end
} 
set EXPORT_RUN_FLAG 0
}

#########################################################################################################


# Kill current CppSim run
proc cppExportClose {} {

global TOP_EXPORT_WINDOW

catch {destroy $TOP_EXPORT_WINDOW}
}

