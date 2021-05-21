#### Procedures for CppSim Import Tool


proc create_run_import_form {} {

global CPPSIM_HOME CPPSIMSHARED_HOME import_destination_library \
       TOP_IMPORT_WINDOW message_import_log cur_s tcl_platform PREV_DIRECTORY \
       import_source_dir IMPORT_FILE_LISTBOX SOURCE_FILE_LABEL IMPORT_RUN_FLAG prev_import_index

catch {destroy $TOP_IMPORT_WINDOW}
set PREV_DIRECTORY ""
set import_source_dir "$CPPSIM_HOME/Import_Export"
set import_destination_library "NewLib"
set IMPORT_RUN_FLAG 0
set prev_import_index ""

toplevel .cppImportForm 
set TOP_IMPORT_WINDOW .cppImportForm

set x [expr [winfo screenwidth $TOP_IMPORT_WINDOW]/2 - [winfo reqwidth $TOP_IMPORT_WINDOW]/2 \
	    - [winfo vrootx [winfo parent $TOP_IMPORT_WINDOW]]]
set y [expr [winfo screenheight $TOP_IMPORT_WINDOW]/2 - [winfo reqheight $TOP_IMPORT_WINDOW]/2 \
	    - [winfo vrooty [winfo parent $TOP_IMPORT_WINDOW]]]

wm geometry .cppImportForm "+$x+$y"
wm title .cppImportForm "Import CppSim Library"
    
bind .cppImportForm <Escape> {catch {destroy .cppImportForm} ; return}

# Create a frame for buttons and entry.
frame .cppImportForm.button_frame -borderwidth 10

pack .cppImportForm.button_frame -side top -fill x
# Create the command buttons.
button .cppImportForm.button_frame.preview -text "Preview" -width 15 -command "cppRunImport preview"
button .cppImportForm.button_frame.import -text "Import" -width 15 -command "cppRunImport import"
button .cppImportForm.button_frame.close -padx 5 -text "Close" -command cppImportClose
pack .cppImportForm.button_frame.close .cppImportForm.button_frame.preview .cppImportForm.button_frame.import -side left -padx 2

frame .cppImportForm.select_destination_frame
label .cppImportForm.select_destination_frame.label -text "Destination Library:" -padx 1
entry .cppImportForm.select_destination_frame.destination_library_entry -width 58 -relief sunken \
-textvariable import_destination_library

pack .cppImportForm.select_destination_frame.label -side left -padx 5
pack .cppImportForm.select_destination_frame.destination_library_entry -side left -fill x
pack .cppImportForm.select_destination_frame -side top -fill x -pady 2


frame .cppImportForm.select_source_frame
label .cppImportForm.select_source_frame.label -text "Source Directory:" -padx 1
button .cppImportForm.select_source_frame.select_button -text "Browse" -padx 10 -command cppImportBrowse
entry .cppImportForm.select_source_frame.source_dir_entry -width 58 -relief sunken \
-textvariable import_source_dir

pack .cppImportForm.select_source_frame.label -side left -padx 5
pack .cppImportForm.select_source_frame.select_button -side left -padx 13
pack .cppImportForm.select_source_frame.source_dir_entry -side left -fill x
pack .cppImportForm.select_source_frame -side top -fill x -pady 2


# Create a labeled entry for the Ts: command
#frame .cppImportForm.ts_frame
#label .cppImportForm.ts_frame.ts_label -text "Ts:         " -padx 1
#entry .cppImportForm.ts_frame.ts_entry -width 20 -relief sunken \
#-textvariable ts_value
#pack .cppImportForm.ts_frame.ts_label -side left
#pack .cppImportForm.ts_frame.ts_entry -side left -fill x -expand true
#pack .cppImportForm.ts_frame -side top -fill x


# Create menu options for sim file

frame .cppImportForm.import_file_frame -width 30
label .cppImportForm.import_file_frame.label -text "Source File/Library: " -padx 1 
set IMPORT_FILE_LISTBOX [listbox .cppImportForm.import_file_frame.files \
   -height 7 -width 20 \
   -yscrollcommand {.cppImportForm.import_file_frame.yscroll set} \
   -xscrollcommand {.cppImportForm.import_file_frame.xscroll set}]
scrollbar .cppImportForm.import_file_frame.yscroll -command {.cppImportForm.import_file_frame.files yview} -orient vertical -relief sunken
scrollbar .cppImportForm.import_file_frame.xscroll -command {.cppImportForm.import_file_frame.files xview} -orient horizontal -relief sunken 

pack .cppImportForm.import_file_frame.label -side left -anchor nw 
#pack .cppImportForm.import_file_frame.files -side left -expand true
pack .cppImportForm.import_file_frame.xscroll -side bottom -fill x
pack .cppImportForm.import_file_frame.yscroll -side right -fill y
pack .cppImportForm.import_file_frame.files -side left -fill both -expand true


bind $IMPORT_FILE_LISTBOX <ButtonRelease-1> "cppUpdateImportLibName"
#bind $IMPORT_FILE_LISTBOX <Double-Button-1> "cppEditModuleCode"
#bind $IMPORT_FILE_LISTBOX <Return> "cppEditModuleCode"

pack .cppImportForm.import_file_frame -side top -fill both -expand true -pady 2 -padx 5

# $IMPORT_FILE_LISTBOX insert end "--- Need to Create a Sim File By Pushing 'Edit Sim File' Button ---"
focus $IMPORT_FILE_LISTBOX

# Create a text widget to log the output
frame .cppImportForm.message_frame -pady 10
label .cppImportForm.message_frame.label -text "Result:  " -padx 1
set message_import_log [text .cppImportForm.message_frame.message -width 80 -height 14 \
-borderwidth 2 -relief raised -setgrid true \
-yscrollcommand {.cppImportForm.message_frame.yscroll set} \
-xscrollcommand {.cppImportForm.message_frame.xscroll set}]
scrollbar .cppImportForm.message_frame.yscroll -command {.cppImportForm.message_frame.message yview} -orient vertical -relief sunken
scrollbar .cppImportForm.message_frame.xscroll -command {.cppImportForm.message_frame.message xview} -orient horizontal -relief sunken

pack .cppImportForm.message_frame.label -anchor nw -side left
pack .cppImportForm.message_frame.xscroll -side bottom -fill x
pack .cppImportForm.message_frame.yscroll -side right -fill y
pack .cppImportForm.message_frame.message -side left -fill both -expand true
pack .cppImportForm.message_frame -side top -fill both -expand true -padx 5

cppImportFileEntries

$message_import_log delete 1.0 end
$message_import_log insert end "\n-------  NOTE:  YOU WILL NEED TO RESTART SUE2 ONCE YOU ARE  -------\n"
$message_import_log insert end   "-------         FINISHED WITH IMPORT TOOL OPERATIONS        -------\n"
$message_import_log insert end "\n         ALSO:  Note that you can import libraries from a CppSim (Version 2)\n"
$message_import_log insert end "                      installation by choosing 'Source Directory:' as the base\n"
$message_import_log insert end "                      directory location of that installation (i.e., c:/CppSim_old)\n"
$message_import_log insert end "                      by using the 'Browse' button above"
$message_import_log see end

}

proc cppUpdateImportLibName {} {
global IMPORT_FILE_LISTBOX import_destination_library prev_import_index

set cur_index [$IMPORT_FILE_LISTBOX curselection]
if {$cur_index != "" && $cur_index != $prev_import_index} {
    set prev_import_index $cur_index
    set import_library_raw [$IMPORT_FILE_LISTBOX get $cur_index]
    set base_index [string first :Private $import_library_raw]
    if {$base_index != -1} {
	set import_library [string range $import_library_raw 0 [expr $base_index - 1]]
    } else {
        set import_library $import_library_raw
    }
    set suffix_index [string first "." $import_library]
    if {$suffix_index < 0} {
       set import_destination_library_temp [string tolower $import_library]
    } else {
       set import_destination_library_temp [string tolower [string range $import_library 0 [expr $suffix_index - 1]]]
    }
    set first_import_char [string range $import_destination_library_temp 0 0]
    set import_destination_library [string replace $import_destination_library_temp 0 0 [string toupper $first_import_char]]
}
}

# CppSim Fill In File/Library Entries
proc cppImportFileEntries {} {

    global CPPSIM_HOME import_source_dir IMPORT_FILE_LISTBOX

    $IMPORT_FILE_LISTBOX delete 0 end

    if {$import_source_dir == "$CPPSIM_HOME/Import_Export"} {
       set file_list [glob -nocomplain -tails -directory $import_source_dir *.tar.gz]
       set num_files [llength $file_list]
       set cur_index 0
       while {$cur_index < $num_files} {
           set cur_entry [lindex $file_list $cur_index]
           $IMPORT_FILE_LISTBOX insert end $cur_entry
	   set cur_index [expr $cur_index + 1]
       }

      set cur_index 0
      if {$num_files > 0} {
          $IMPORT_FILE_LISTBOX selection set $cur_index
          $IMPORT_FILE_LISTBOX activate $cur_index
      } else {
          $IMPORT_FILE_LISTBOX insert end "Error: no .tar.gz files"       
          $IMPORT_FILE_LISTBOX insert end "       Please place a .tar.gz file generated from 'Export CppSim Library' tool"
          $IMPORT_FILE_LISTBOX insert end "       into '$import_source_dir' directory"
          $IMPORT_FILE_LISTBOX insert end "       OR choose 'Source Directory:' as the base directory"
          $IMPORT_FILE_LISTBOX insert end "       location of a CppSim (Version 2) installation"
          $IMPORT_FILE_LISTBOX insert end "       using the 'Browse' button above"
      }
    } else {
         set file_list [glob -nocomplain -tails -directory $import_source_dir/Sue2 sue.lib]
         set num_files [llength $file_list]
         if {$num_files == 0} {
            $IMPORT_FILE_LISTBOX insert end "Error:  invalid choice of 'Source Directory:'"       
            $IMPORT_FILE_LISTBOX insert end "        Proper choices are:"
            $IMPORT_FILE_LISTBOX insert end "        1) '$CPPSIM_HOME/Import_Export'"              
            $IMPORT_FILE_LISTBOX insert end "        2) Base directory location of a CppSim (Version 2) installation"
	 } else {
	     if {$import_source_dir == "$CPPSIM_HOME"} {
                $IMPORT_FILE_LISTBOX insert end "Error:  invalid choice of 'Source Directory:'"       
                $IMPORT_FILE_LISTBOX insert end "        '$CPPSIM_HOME' is the base directory of THIS CppSim installation"       
                $IMPORT_FILE_LISTBOX insert end "        Proper choices are:"
                $IMPORT_FILE_LISTBOX insert end "        1) '$CPPSIM_HOME/Import_Export'"
                $IMPORT_FILE_LISTBOX insert end "        2) Base directory location of a CppSim (Version 2) installation"
	     } else {
                catch {open "$import_source_dir/Sue2/sue.lib"} fid
                set cur_index 0
		while {[gets $fid line] >= 0} {
		    set dir_name [cpp_check_import_library $line $import_source_dir/Sue2] 
		    if {$dir_name == "NULL_NOT_CPPSIM2"} {
                        $IMPORT_FILE_LISTBOX insert end "Error:  invalid choice of 'Source Directory:'"       
                        $IMPORT_FILE_LISTBOX insert end "        '$import_source_dir' corresponds to a CppSim (Version 2) installation"       
                        $IMPORT_FILE_LISTBOX insert end "        Proper choices are:"
                        $IMPORT_FILE_LISTBOX insert end "        1) '$CPPSIM_HOME/Import_Export'"              
                        $IMPORT_FILE_LISTBOX insert end "        2) Base directory location of a CppSim (Version 2) installation"
                        break;
		    } elseif {$dir_name != ""} {
                        $IMPORT_FILE_LISTBOX insert end $line
		        set cur_index [expr $cur_index + 1]
		    }
                }
                catch {close $fid} err
                if {$cur_index > 0} {
                    set cur_index 0
                    $IMPORT_FILE_LISTBOX selection set $cur_index
                    $IMPORT_FILE_LISTBOX activate $cur_index
                }
	     }
	 }
    }
}

proc cpp_check_import_library {lib_name sue_lib_dir} {

set dir_exists [file isdirectory $sue_lib_dir/SueLib]

if {$dir_exists} {
   # Old version of CppSim
   set dir_exists [file isdirectory "$sue_lib_dir/SueLib/$lib_name"]
   if {$dir_exists} {
      return $lib_name
   } else {
      return ""
   }
} else {
   return "NULL_NOT_CPPSIM2"
}
}


# CppSim Choose Import Directory routine
proc cppImportBrowse {} {

    global import_source_dir PREV_DIRECTORY CPPSIM_HOME 

    if {$PREV_DIRECTORY == ""} {
       set PREV_DIRECTORY "$CPPSIM_HOME/Import_Export"
    }
    set directory $PREV_DIRECTORY
    # get the name of the directory from the file selector box
    set directory [tk_chooseDirectory -initialdir $PREV_DIRECTORY -title "Choose Import Directory"]

# if nil, file selector box cancelled -- do nothing
#    if {$directory == ""} { 
#      return 
#    }

    set import_source_dir $directory

    cppImportFileEntries
}


# CppSim Import routine
proc cppRunImport {option_string} {


global message_import_log CPPSIMSHARED_HOME CPPSIM_HOME tcl_platform \
       import_destination_library IMPORT_FILE_LISTBOX \
       import_source_dir IMPORT_RUN_FLAG

if {$IMPORT_RUN_FLAG == 1} {
    return
}
set IMPORT_RUN_FLAG 1

if {$tcl_platform(platform) == "windows"} {
   set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin/win32"
} else {
   set sue2_bin_directory "$CPPSIMSHARED_HOME/Sue2/bin"
}

set cur_index [$IMPORT_FILE_LISTBOX curselection]
if {$cur_index != ""} {
    set import_filename [$IMPORT_FILE_LISTBOX get $cur_index]
} else {
    $message_import_log delete 1.0 end
    $message_import_log insert end "----------------- Error:  must select 'Source File/Library:' -----------------\n\n"
    $message_import_log see end

    set IMPORT_RUN_FLAG 0
    return
}

set shared_suelib_dir $CPPSIMSHARED_HOME/SueLib
set shared_dir_exists [file isdirectory $shared_suelib_dir/$import_destination_library]
if {$shared_dir_exists} {
   set verify_flag [tk_dialog_new .cppsim_message "Verification of New Library Name" \
		  "Are you sure that you want to call your new library '$import_destination_library'?\nThis name corresponds to an existing, shared library.\nIf you move forward with saving under this library name, the existing shared library will no longer be accessible." "" 1 {Yes} {Cancel}]
    if {$verify_flag != 0} {
        set IMPORT_RUN_FLAG 0
	return
    }
}

if {$option_string == "preview"} {
   $message_import_log delete 1.0 end
   $message_import_log insert end "----------------- Running Preview -----------------\n\n"
   $message_import_log see end
} else {
   $message_import_log delete 1.0 end
   $message_import_log insert end "----------------- Running Import -----------------\n\n"
   $message_import_log see end
}





if {$import_source_dir == "$CPPSIM_HOME/Import_Export"} {

   if {$tcl_platform(platform) == "windows"} {
      set rm_bin_file "$CPPSIMSHARED_HOME/Msys/bin/rm"
   } else {
      set rm_bin_file "rm"
   }

   $message_import_log insert end "\n... Clearing out Import_Export directory ...\n\n"
   $message_import_log see end

   set dir_exists [file isdirectory $CPPSIM_HOME/Import_Export/SueLib]
   if {$dir_exists} {
      catch {open "|$rm_bin_file -rf $CPPSIM_HOME/Import_Export/SueLib |& cat"} fid
      set message_input [read $fid]
      $message_import_log insert end $message_input
      $message_import_log see end

      if {[catch {close $fid} err] != 0} {
         $message_import_log insert end "\n------------------ Importing Exited with Errors --------------------\n"
         $message_import_log see end
         set IMPORT_RUN_FLAG 0
         return
      }
   }

   set dir_exists [file isdirectory $CPPSIM_HOME/Import_Export/SimRuns]
   if {$dir_exists} {
      catch {open "|$rm_bin_file -rf $CPPSIM_HOME/Import_Export/SimRuns |& cat"} fid
      set message_input [read $fid]
      $message_import_log insert end $message_input
      $message_import_log see end

      if {[catch {close $fid} err] != 0} {
         $message_import_log insert end "\n------------------ Importing Exited with Errors --------------------\n"
         $message_import_log see end
         set IMPORT_RUN_FLAG 0
         return
      }
   }
   set dir_exists [file isdirectory $CPPSIM_HOME/Import_Export/CadenceLib]
   if {$dir_exists} {
      catch {open "|$rm_bin_file -rf $CPPSIM_HOME/Import_Export/CadenceLib |& cat"} fid
      set message_input [read $fid]
      $message_import_log insert end $message_input
      $message_import_log see end

      if {[catch {close $fid} err] != 0} {
         $message_import_log insert end "\n------------------ Importing Exited with Errors --------------------\n"
         $message_import_log see end
         set IMPORT_RUN_FLAG 0
         return
      }
   }

   if {$tcl_platform(platform) == "windows"} {
      set tar_bin_file "$CPPSIMSHARED_HOME/Msys/bin/bsdtar"
   } else {
      set tar_bin_file "tar"
   }

   set cur_directory [pwd]
   cd $import_source_dir

   $message_import_log insert end "\n... Extracting file '$import_filename' ...\n\n"
   $message_import_log see end

   catch {open "|$tar_bin_file xfz $import_filename |& cat"} fid
   set message_input [read $fid]
   $message_import_log insert end $message_input
   $message_import_log see end

   cd $cur_directory
   if {[catch {close $fid} err] != 0} {
      $message_import_log insert end "\n------------------ Importing Exited with Errors --------------------\n"
      $message_import_log see end
      set IMPORT_RUN_FLAG 0
      return
   } else {
      if {$option_string == "preview"} {
         $message_import_log insert end "\n... Now previewing contents of '$import_filename' ...\n"
      } else {
         $message_import_log insert end "\n... Now importing contents of '$import_filename' ...\n"
      }
      $message_import_log see end
   }


} else {

   set sue_lib_file "$import_source_dir/Sue2/sue.lib"

   $message_import_log insert end "... Exporting CppSim (Version 2) Library: '$import_filename' ...\n\n"
   $message_import_log see end

   set cur_directory "$import_source_dir/Sue2/SueLib/$import_filename"
   set file_list [glob -nocomplain -tails -directory $cur_directory *.sue]
   set num_files [llength $file_list]
   if {$num_files < 1} {
      $message_import_log insert end "\n------------------ Error: No .sue files in library '$import_filename' --------------------\n"
      $message_import_log insert end "\n------------------ Importing Exited with Errors --------------------\n"
      $message_import_log see end
      set IMPORT_RUN_FLAG 0
      return
   }
   set first_sue_file [lindex $file_list 0]

   catch {open "|$sue2_bin_directory/export_cppsim_library $first_sue_file $sue_lib_file all |& cat"} fid
   set message_input [read $fid]
   $message_import_log insert end $message_input
   $message_import_log see end

   if {[catch {close $fid} err] != 0} {
      $message_import_log insert end "\n------------------ Importing Exited with Errors --------------------\n"
      $message_import_log see end
      set IMPORT_RUN_FLAG 0
      return
   } else {
      if {$option_string == "preview"} {
         $message_import_log insert end "\n... Now previewing contents of library '$import_filename' ...\n"
      } else {
         $message_import_log insert end "\n... Now importing contents of library '$import_filename' ...\n"
      }
      $message_import_log see end
   }
}

set sue_lib_file "$CPPSIM_HOME/Sue2/sue.lib"


if {$option_string == "preview"} {
   catch {open "|$sue2_bin_directory/import_cppsim_library $sue_lib_file  \
        $import_destination_library preview |& cat"} fid
   set message_input [read $fid]
   $message_import_log insert end $message_input
   $message_import_log see end
} else {
   catch {open "|$sue2_bin_directory/import_cppsim_library $sue_lib_file  \
        $import_destination_library import |& cat"} fid
   set message_input [read $fid]
   $message_import_log insert end $message_input
   $message_import_log see end
}
set import_destination_library_full $CPPSIM_HOME/SueLib/$import_destination_library
set sue_files_exist [glob -nocomplain -tails -directory $import_destination_library_full *.sue]
if {$sue_files_exist == ""} {
   file delete -force $import_destination_library_full
   remove_lib_from_sue_lib_file $import_destination_library
}

if {[catch {close $fid} err] != 0} {
    $message_import_log insert end "\n------------------ Importing Exited with Errors --------------------\n"
    $message_import_log see end
}
update_sue_lib_file -add ""
set IMPORT_RUN_FLAG 0
}


proc remove_lib_from_sue_lib_file {remove_lib} {
global CPPSIM_HOME CPPSIMSHARED_HOME


set sue_lib_filename $CPPSIM_HOME/Sue2/sue.lib

catch {open "$sue_lib_filename"} fid
set sue_lib_list ""
while {[gets $fid line] >= 0} {
   set sue_lib_list [concat $sue_lib_list $line]
}
catch {close $fid} err

catch {open "$sue_lib_filename" w} fid
set num_sue_lib_entries [llength $sue_lib_list]
set cur_index 0
while {$cur_index < $num_sue_lib_entries} {
   set cur_entry [lindex $sue_lib_list $cur_index]
   if {$cur_entry != $remove_lib} {
       puts $fid $cur_entry
   }
   set cur_index [expr $cur_index + 1]
}
catch {close $fid} err
}

#########################################################################################################


# Kill current CppSim run
proc cppImportClose {} {

global TOP_IMPORT_WINDOW

catch {destroy $TOP_IMPORT_WINDOW}
}

