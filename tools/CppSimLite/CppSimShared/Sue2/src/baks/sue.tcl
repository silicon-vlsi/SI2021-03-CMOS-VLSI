#!/usr/bin/wish -f

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

# Note that no site customization should be done in this file.  ALL 
# customization should be done in .suerc

# otherwise if you mistype, you wait forever
set auto_noexec 1

# directory where sue is (this file is assumed to be down src)
global CPPSIM_HOME CPPSIMSHARED_HOME EDITOR tcl_platform

set USER_HOME $::env(HOME)

if {[info exists env(CPPSIMHOME)] == 1 || [info exists env(CppSimHome)] == 1} {
   set CPPSIM_HOME [use_first env(CPPSIMHOME) env(CppSimHome)]
   set tilde_index [string first ~ $CPPSIM_HOME]
   if { $tilde_index != -1 } {
      set CPPSIM_HOME [string replace $CPPSIM_HOME $tilde_index $tilde_index $USER_HOME]
   }

   while {1} {
      set index_val [string first \\ $CPPSIM_HOME]
      if { $index_val != -1 } {
          set CPPSIM_HOME [string replace $CPPSIM_HOME $index_val $index_val "/"]
      } else { break }
   }
} else {
   set CPPSIM_HOME "$USER_HOME/CppSim"
}

if {[info exists env(CPPSIMSHAREDHOME)] == 1 || [info exists env(CppSimSharedHome)] == 1} {
   set CPPSIMSHARED_HOME [use_first env(CPPSIMSHAREDHOME) env(CppSimSharedHome)]

   set tilde_index [string first ~ $CPPSIMSHARED_HOME]
   if { $tilde_index != -1 } {
       set CPPSIMSHARED_HOME [string replace $CPPSIMSHARED_HOME $tilde_index $tilde_index $USER_HOME]
   }

   while {1} {
      set index_val [string first \\ $CPPSIMSHARED_HOME]
      if { $index_val != -1 } {
          set CPPSIMSHARED_HOME [string replace $CPPSIMSHARED_HOME $index_val $index_val "/"]
      } else { break }
   }
} else {
   set CPPSIMSHARED_HOME "$CPPSIM_HOME/CppSimShared"
}

set EDITOR [use_first env(EDITOR) env(editor)]
if {[string length $EDITOR] == 0} {
       set EDITOR "emacs"
} else {
   set tilde_index [string first ~ $EDITOR]
   if { $tilde_index != -1 } {
        set EDITOR [string replace $EDITOR $tilde_index $tilde_index $USER_HOME]
   }
}

#insert_unique auto_path 0 $CPPSIMSHARED_HOME/Sue2/src $CPPSIMSHARED_HOME/packages

# for 2-headed displays, environmental variable PROBE_DISPLAY if it exists 
# determines what display to bring up nst/signalscan on.  The fallback of
# SPICETOOL_DISPLAY is for historical reasons.  
#
# valid fields are "", ":0.0", ":0.1", or "other" for other display than sue.

set PROBE_DISPLAY [use_first env(PROBE_DISPLAY) env(SPICETOOL_DISPLAY)]
if {$PROBE_DISPLAY == "other"} {
  set screen [winfo screen .]
  set parts [split $screen .]
  set PROBE_DISPLAY "-display [lindex $parts 0].[expr 1 - [lindex $parts 1]]"
} elseif {$PROBE_DISPLAY != "" && \
	      [string range $PROBE_DISPLAY 0 7] != "-display"} {
  set PROBE_DISPLAY "-display $PROBE_DISPLAY"
}

# Load in the default settings (these settings can be changed by users
# by setting their value in a "~/.suerc" and/or ".suerc" file)
puts "CppSim home directory for user:  CPPSIMHOME = $CPPSIM_HOME"
puts "CppSim shared directory:  CPPSIMSHAREDHOME = $CPPSIMSHARED_HOME"
puts "Sourcing Sue2 configuration file: $CPPSIM_HOME/Sue2/.suerc"
source $CPPSIM_HOME/Sue2/.suerc

# Source the user's generic customization file if it exists.  This will 
# override any defaults values just sourced in the default .suerc
#if {[file exists "~/.suerc"]} {
#  puts "Sourcing ~/.suerc"
#  source "~/.suerc"
#}

# MODE is used for Sim menu, choice of default icons to load, and netlisting/
# probing commands
set MODE [use_first MODE env(SUE_MODE) DEFAULT_MODE]

set NETLIST_TYPE [use_first NETLIST_TYPE MODE]
set NETLIST_PROPS [use_first NETLIST_PROPS MODE]

set PROBE_TYPE [use_first [string toupper $NETLIST_TYPE]_PROBE_TYPE]

# When = 1 this stops a backround canvas event handler
set DISABLE_CANVAS_EVENT 0

# set up the default list of tags which are saved and restored by
# save_bindings and restore_bindings
set TAGS_TO_SAVE "icon draw_item wire dot open edit_marker"

# here is basically a macro
set SNAP_XY {[$cur_c canvasx %x $scale] [$cur_c canvasy %y $scale]}
set NOSNAP_XY {[$cur_c canvasx %x] [$cur_c canvasy %y]}

# for propaging changes in icons thru schematics which contain instances
# of those icons.
set MODIFY_ICON(_index) 0

if {$tcl_platform(os) == "Darwin"} {
if {$FONT_MODE == "standard"} {
  # All that is used are the standard fonts included with most X servers
  set FONT(1) -*-fixed-medium-R-Normal--*-10-*
  set FONT(2) -*-fixed-medium-R-Normal--*-30-*
  set FONT(3) -*-fixed-medium-R-Normal--*-50-*
  set FONT(4) -*-fixed-medium-R-Normal--*-65-*
  set FONT(5) -*-fixed-medium-R-Normal--*-80-*
  set FONT(6) -*-fixed-medium-R-Normal--*-90-*
  set FONT(7) -*-fixed-medium-R-Normal--*-100-*
  set FONT(8) -*-fixed-medium-R-Normal--*-110-*
  set FONT(9) -*-fixed-medium-R-Normal--*-120-*
  set FONT(10) -*-fixed-medium-R-Normal--*-130-*
  set FONT(11) -*-fixed-medium-R-Normal--*-140-*
  set FONT(12) -*-fixed-medium-R-Normal--*-150-*
  set FONT(13) -*-fixed-medium-R-Normal--*-170-*
  set FONT(14) -*-fixed-medium-R-Normal--*-180-*
  set FONT(15) -*-fixed-medium-R-Normal--*-190-*
  set FONT(16) -*-fixed-medium-R-Normal--*-200-*
  set FONT(17) -*-fixed-medium-R-Normal--*-210-*
  set FONT(18) -*-fixed-medium-R-Normal--*-220-*
  set FONT(19) -*-fixed-medium-R-Normal--*-230-*
  set FONT(20) -*-fixed-medium-R-Normal--*-240-*
  set FONT(21) -*-fixed-medium-R-Normal--*-250-*
  set FONT(22) -*-fixed-medium-R-Normal--*-260-*
  set FONT(23) -*-fixed-medium-R-Normal--*-270-*
  set FONT(24) -*-fixed-medium-R-Normal--*-280-*
  set FONT(25) -*-fixed-medium-R-Normal--*-290-*
  set FONT(26) -*-fixed-medium-R-Normal--*-300-*
  set FONT(27) -*-fixed-medium-R-Normal--*-310-*
  set FONT(28) -*-fixed-medium-R-Normal--*-320-*
  set FONT(29) -*-fixed-medium-R-Normal--*-330-*
  set FONT(30) -*-fixed-medium-R-Normal--*-340-*
  set FONT(31) -*-fixed-medium-R-Normal--*-350-*
  set FONT(32) -*-fixed-medium-R-Normal--*-360-*
  set FONT(33) -*-fixed-medium-R-Normal--*-370-*
  set FONT(34) -*-fixed-medium-R-Normal--*-380-*
  set FONT(35) -*-fixed-medium-R-Normal--*-390-*
  set FONT(36) -*-fixed-medium-R-Normal--*-400-*
  set FONT(37) -*-fixed-medium-R-Normal--*-410-*
  set FONT(38) -*-fixed-medium-R-Normal--*-420-*
  set FONT(39) -*-fixed-medium-R-Normal--*-430-*
  set FONT(40) -*-fixed-medium-R-Normal--*-440-*

} elseif {$FONT_MODE == "font_server"} {
  # Uses fonts in addition to the standard (limited) fonts present on
  # most machines to improve the font proportionality.  These extra fonts 
  # are generated by the font server which is a part of X11R5 and later (??).  
  # Since font generation is fairly slow (a few seconds per font) only 
  # a small number of additional fonts are added.  Any ideas on a better
  # font mechanism would be appreciated.
  set FONT(1) -*-fixed-medium-R-Normal--*-10-*
  set FONT(2) -*-fixed-medium-R-Normal--*-10-*
  set FONT(3) -*-fixed-medium-R-Normal--*-40-*
  set FONT(4) -*-fixed-medium-R-Normal--*-40-*
  set FONT(5) -*-fixed-medium-R-Normal--*-80-*
  set FONT(6) -*-fixed-medium-R-Normal--*-80-*
  set FONT(7) -*-fixed-medium-R-Normal--*-100-*
  set FONT(8) -*-fixed-medium-R-Normal--*-120-*
  set FONT(9) -*-fixed-medium-R-Normal--*-140-*
  set FONT(10) -*-fixed-medium-R-Normal--*-140-*
  set FONT(11) -*-fixed-medium-R-Normal--*-180-*
  set FONT(12) -*-fixed-medium-R-Normal--*-180-*
  set FONT(13) -*-fixed-medium-R-Normal--*-180-*
  set FONT(14) -*-fixed-medium-R-Normal--*-180-*
  set FONT(15) -*-fixed-medium-R-Normal--*-240-*
  set FONT(16) -*-fixed-medium-R-Normal--*-240-*
  set FONT(17) -*-fixed-medium-R-Normal--*-240-*
  set FONT(18) -*-fixed-medium-R-Normal--*-240-*
  set FONT(19) -*-fixed-medium-R-Normal--*-240-*
  set FONT(20) -*-fixed-medium-R-Normal--*-240-*
  set FONT(21) -*-fixed-medium-R-Normal--*-450-*
  set FONT(22) -*-fixed-medium-R-Normal--*-450-*
  set FONT(23) -*-fixed-medium-R-Normal--*-450-*
  set FONT(24) -*-fixed-medium-R-Normal--*-450-*
  set FONT(25) -*-fixed-medium-R-Normal--*-450-*
  set FONT(26) -*-fixed-medium-R-Normal--*-450-*
  set FONT(27) -*-fixed-medium-R-Normal--*-450-*
  set FONT(28) -*-fixed-medium-R-Normal--*-450-*
  set FONT(29) -*-fixed-medium-R-Normal--*-450-*
  set FONT(30) -*-fixed-medium-R-Normal--*-450-*
  set FONT(31) -*-fixed-medium-R-Normal--*-450-*
  set FONT(32) -*-fixed-medium-R-Normal--*-450-*
  set FONT(33) -*-fixed-medium-R-Normal--*-450-*
  set FONT(34) -*-fixed-medium-R-Normal--*-450-*
  set FONT(35) -*-fixed-medium-R-Normal--*-450-*
  set FONT(36) -*-fixed-medium-R-Normal--*-450-*
  set FONT(37) -*-fixed-medium-R-Normal--*-450-*
  set FONT(38) -*-fixed-medium-R-Normal--*-450-*
  set FONT(39) -*-fixed-medium-R-Normal--*-450-*
  set FONT(40) -*-fixed-medium-R-Normal--*-450-*

} elseif {$FONT_MODE == "extended"} {
  # Uses fonts that have been added to xlsfonts.  Since these take no
  # additional time to load (though they do take extra memory?!), we
  # can use many more sizes.
  #
  # Generate new fonts using:
  #
  #    xset q   (last line displayed should be font path)
  #    cd <font path>
  #    makeafb -2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40 Helvetica-normal.f3b
  #    makeafb -42,44,46,48,50,52,54,56,60,62,64,66,68,70,72,74,76,78,80 Helvetica-normal.f3b
  #    convertfont Helvetica-normal*.afb
  #    bldfamily
  #    xset fp rehash

  set FONT(1) Helvetica-normal2
  set FONT(2) Helvetica-normal4
  set FONT(3) Helvetica-normal6
  set FONT(4) Helvetica-normal8
  set FONT(5) Helvetica-normal10
  set FONT(6) Helvetica-normal12
  set FONT(7) Helvetica-normal14
  set FONT(8) Helvetica-normal16
  set FONT(9) Helvetica-normal18
  set FONT(10) Helvetica-normal20
  set FONT(11) Helvetica-normal22
  set FONT(12) Helvetica-normal24
  set FONT(13) Helvetica-normal26
  set FONT(14) Helvetica-normal28
  set FONT(15) Helvetica-normal30
  set FONT(16) Helvetica-normal32
  set FONT(17) Helvetica-normal34
  set FONT(18) Helvetica-normal36
  set FONT(19) Helvetica-normal38
  set FONT(20) Helvetica-normal40
  set FONT(21) Helvetica-normal42
  set FONT(22) Helvetica-normal44
  set FONT(23) Helvetica-normal46
  set FONT(24) Helvetica-normal48
  set FONT(25) Helvetica-normal50
  set FONT(26) Helvetica-normal52
  set FONT(27) Helvetica-normal54
  set FONT(28) Helvetica-normal56
  set FONT(29) Helvetica-normal58
  set FONT(30) Helvetica-normal60
  set FONT(31) Helvetica-normal62
  set FONT(32) Helvetica-normal64
  set FONT(33) Helvetica-normal66
  set FONT(34) Helvetica-normal68
  set FONT(35) Helvetica-normal70
  set FONT(36) Helvetica-normal72
  set FONT(37) Helvetica-normal74
  set FONT(38) Helvetica-normal76
  set FONT(39) Helvetica-normal78
  set FONT(40) Helvetica-normal80

} else {
  puts "ERROR: FONT_MODE is set to an incorrect value"
}
} else {
if {$FONT_MODE == "standard"} {
  # All that is used are the standard fonts included with most X servers
  set FONT(1) -*-helvetica-normal-R-Normal--*-10-*
  set FONT(2) -*-helvetica-normal-R-Normal--*-30-*
  set FONT(3) -*-helvetica-normal-R-Normal--*-50-*
  set FONT(4) -*-helvetica-normal-R-Normal--*-65-*
  set FONT(5) -*-helvetica-normal-R-Normal--*-80-*
  set FONT(6) -*-helvetica-normal-R-Normal--*-90-*
  set FONT(7) -*-helvetica-normal-R-Normal--*-100-*
  set FONT(8) -*-helvetica-normal-R-Normal--*-110-*
  set FONT(9) -*-helvetica-normal-R-Normal--*-120-*
  set FONT(10) -*-helvetica-normal-R-Normal--*-130-*
  set FONT(11) -*-helvetica-normal-R-Normal--*-140-*
  set FONT(12) -*-helvetica-normal-R-Normal--*-150-*
  set FONT(13) -*-helvetica-normal-R-Normal--*-170-*
  set FONT(14) -*-helvetica-normal-R-Normal--*-180-*
  set FONT(15) -*-helvetica-normal-R-Normal--*-190-*
  set FONT(16) -*-helvetica-normal-R-Normal--*-200-*
  set FONT(17) -*-helvetica-normal-R-Normal--*-210-*
  set FONT(18) -*-helvetica-normal-R-Normal--*-220-*
  set FONT(19) -*-helvetica-normal-R-Normal--*-230-*
  set FONT(20) -*-helvetica-normal-R-Normal--*-240-*
  set FONT(21) -*-helvetica-normal-R-Normal--*-250-*
  set FONT(22) -*-helvetica-normal-R-Normal--*-260-*
  set FONT(23) -*-helvetica-normal-R-Normal--*-270-*
  set FONT(24) -*-helvetica-normal-R-Normal--*-280-*
  set FONT(25) -*-helvetica-normal-R-Normal--*-290-*
  set FONT(26) -*-helvetica-normal-R-Normal--*-300-*
  set FONT(27) -*-helvetica-normal-R-Normal--*-310-*
  set FONT(28) -*-helvetica-normal-R-Normal--*-320-*
  set FONT(29) -*-helvetica-normal-R-Normal--*-330-*
  set FONT(30) -*-helvetica-normal-R-Normal--*-340-*
  set FONT(31) -*-helvetica-normal-R-Normal--*-350-*
  set FONT(32) -*-helvetica-normal-R-Normal--*-360-*
  set FONT(33) -*-helvetica-normal-R-Normal--*-370-*
  set FONT(34) -*-helvetica-normal-R-Normal--*-380-*
  set FONT(35) -*-helvetica-normal-R-Normal--*-390-*
  set FONT(36) -*-helvetica-normal-R-Normal--*-400-*
  set FONT(37) -*-helvetica-normal-R-Normal--*-410-*
  set FONT(38) -*-helvetica-normal-R-Normal--*-420-*
  set FONT(39) -*-helvetica-normal-R-Normal--*-430-*
  set FONT(40) -*-helvetica-normal-R-Normal--*-440-*

} elseif {$FONT_MODE == "font_server"} {
  # Uses fonts in addition to the standard (limited) fonts present on
  # most machines to improve the font proportionality.  These extra fonts 
  # are generated by the font server which is a part of X11R5 and later (??).  
  # Since font generation is fairly slow (a few seconds per font) only 
  # a small number of additional fonts are added.  Any ideas on a better
  # font mechanism would be appreciated.
  set FONT(1) -*-helvetica-normal-R-Normal--*-10-*
  set FONT(2) -*-helvetica-normal-R-Normal--*-10-*
  set FONT(3) -*-helvetica-normal-R-Normal--*-40-*
  set FONT(4) -*-helvetica-normal-R-Normal--*-40-*
  set FONT(5) -*-helvetica-normal-R-Normal--*-80-*
  set FONT(6) -*-helvetica-normal-R-Normal--*-80-*
  set FONT(7) -*-helvetica-normal-R-Normal--*-100-*
  set FONT(8) -*-helvetica-normal-R-Normal--*-120-*
  set FONT(9) -*-helvetica-normal-R-Normal--*-140-*
  set FONT(10) -*-helvetica-normal-R-Normal--*-140-*
  set FONT(11) -*-helvetica-normal-R-Normal--*-180-*
  set FONT(12) -*-helvetica-normal-R-Normal--*-180-*
  set FONT(13) -*-helvetica-normal-R-Normal--*-180-*
  set FONT(14) -*-helvetica-normal-R-Normal--*-180-*
  set FONT(15) -*-helvetica-normal-R-Normal--*-240-*
  set FONT(16) -*-helvetica-normal-R-Normal--*-240-*
  set FONT(17) -*-helvetica-normal-R-Normal--*-240-*
  set FONT(18) -*-helvetica-normal-R-Normal--*-240-*
  set FONT(19) -*-helvetica-normal-R-Normal--*-240-*
  set FONT(20) -*-helvetica-normal-R-Normal--*-240-*
  set FONT(21) -*-helvetica-normal-R-Normal--*-450-*
  set FONT(22) -*-helvetica-normal-R-Normal--*-450-*
  set FONT(23) -*-helvetica-normal-R-Normal--*-450-*
  set FONT(24) -*-helvetica-normal-R-Normal--*-450-*
  set FONT(25) -*-helvetica-normal-R-Normal--*-450-*
  set FONT(26) -*-helvetica-normal-R-Normal--*-450-*
  set FONT(27) -*-helvetica-normal-R-Normal--*-450-*
  set FONT(28) -*-helvetica-normal-R-Normal--*-450-*
  set FONT(29) -*-helvetica-normal-R-Normal--*-450-*
  set FONT(30) -*-helvetica-normal-R-Normal--*-450-*
  set FONT(31) -*-helvetica-normal-R-Normal--*-450-*
  set FONT(32) -*-helvetica-normal-R-Normal--*-450-*
  set FONT(33) -*-helvetica-normal-R-Normal--*-450-*
  set FONT(34) -*-helvetica-normal-R-Normal--*-450-*
  set FONT(35) -*-helvetica-normal-R-Normal--*-450-*
  set FONT(36) -*-helvetica-normal-R-Normal--*-450-*
  set FONT(37) -*-helvetica-normal-R-Normal--*-450-*
  set FONT(38) -*-helvetica-normal-R-Normal--*-450-*
  set FONT(39) -*-helvetica-normal-R-Normal--*-450-*
  set FONT(40) -*-helvetica-normal-R-Normal--*-450-*

} elseif {$FONT_MODE == "extended"} {
  # Uses fonts that have been added to xlsfonts.  Since these take no
  # additional time to load (though they do take extra memory?!), we
  # can use many more sizes.
  #
  # Generate new fonts using:
  #
  #    xset q   (last line displayed should be font path)
  #    cd <font path>
  #    makeafb -2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40 Helvetica-normal.f3b
  #    makeafb -42,44,46,48,50,52,54,56,60,62,64,66,68,70,72,74,76,78,80 Helvetica-normal.f3b
  #    convertfont Helvetica-normal*.afb
  #    bldfamily
  #    xset fp rehash

  set FONT(1) Helvetica-normal2
  set FONT(2) Helvetica-normal4
  set FONT(3) Helvetica-normal6
  set FONT(4) Helvetica-normal8
  set FONT(5) Helvetica-normal10
  set FONT(6) Helvetica-normal12
  set FONT(7) Helvetica-normal14
  set FONT(8) Helvetica-normal16
  set FONT(9) Helvetica-normal18
  set FONT(10) Helvetica-normal20
  set FONT(11) Helvetica-normal22
  set FONT(12) Helvetica-normal24
  set FONT(13) Helvetica-normal26
  set FONT(14) Helvetica-normal28
  set FONT(15) Helvetica-normal30
  set FONT(16) Helvetica-normal32
  set FONT(17) Helvetica-normal34
  set FONT(18) Helvetica-normal36
  set FONT(19) Helvetica-normal38
  set FONT(20) Helvetica-normal40
  set FONT(21) Helvetica-normal42
  set FONT(22) Helvetica-normal44
  set FONT(23) Helvetica-normal46
  set FONT(24) Helvetica-normal48
  set FONT(25) Helvetica-normal50
  set FONT(26) Helvetica-normal52
  set FONT(27) Helvetica-normal54
  set FONT(28) Helvetica-normal56
  set FONT(29) Helvetica-normal58
  set FONT(30) Helvetica-normal60
  set FONT(31) Helvetica-normal62
  set FONT(32) Helvetica-normal64
  set FONT(33) Helvetica-normal66
  set FONT(34) Helvetica-normal68
  set FONT(35) Helvetica-normal70
  set FONT(36) Helvetica-normal72
  set FONT(37) Helvetica-normal74
  set FONT(38) Helvetica-normal76
  set FONT(39) Helvetica-normal78
  set FONT(40) Helvetica-normal80

} else {
  puts "ERROR: FONT_MODE is set to an incorrect value"
}
}

# Maximum number of font sizes
set FONT(MAX) 40

proc setup_font {name scaling} {

  global FONT

  for {set i 1} {$i <= $FONT(MAX)} {incr i} {
    set FONT($name,$i) $FONT([max 1 [min $FONT(MAX) [expr round($scaling*$i)]]])
  }
}

# setup the font matrix for 3 size fonts
setup_font small $FONT(small)
setup_font standard 1.0
setup_font large $FONT(large)

# make a window
set WIN .win1
make_window $WIN


# Load cells listed in sue.lib file

global SUE_LIB_DIR
set SUE_LIB_DIR "$CPPSIM_HOME/Sue2"
# puts "SUE_LIB_DIR = $SUE_LIB_DIR"

puts "Sourcing Sue2 library file:  $SUE_LIB_DIR/sue.lib"
# set SUE_LIB_DIR [string trimright [pwd] "/bin"]
set sue_lib_file_name $SUE_LIB_DIR/sue.lib
# puts "file = $sue_lib_file_name"
if {[catch {set sue_lib_file [open $sue_lib_file_name r]} error]} {
    # failed, file probably doesn't exist
    set button [tk_dialog_new .sue_lib_issue "Error: sue.lib not found!" \
		    "Error: $error" \
		    "" 0 {Exit}]
    modify_exit
}

#  set initial schematic listbox directory based on first
#  line in sue.lib file


global INIT_SCHEM_DIR
set found_first_valid_dir_flag 0
set INIT_SCHEM_DIR ""

make_icon_listbox
while {[gets $sue_lib_file line] >= 0} {
    if {[catch {glob $CPPSIM_HOME/SueLib/$line/*.sue} error] != 1} {
       add_auto_path $CPPSIM_HOME/SueLib/$line
       foreach file [glob $CPPSIM_HOME/SueLib/$line/*.sue] {
          source $file
       }
       if {$found_first_valid_dir_flag == 0} {
           set found_first_valid_dir_flag 1
           set INIT_SCHEM_DIR $CPPSIM_HOME/SueLib/$line           
       } elseif {$found_first_valid_dir_flag == 1} {
           set found_first_valid_dir_flag 2
           make_icon_listbox $CPPSIM_HOME/SueLib/$line .icons1 
       } elseif {$found_first_valid_dir_flag == 2} {
           set found_first_valid_dir_flag 3
           make_icon_listbox $CPPSIM_HOME/SueLib/$line .icons2 
       }
    } elseif {[catch {glob $CPPSIMSHARED_HOME/SueLib/$line/*.sue} error] != 1} {
       add_auto_path $CPPSIMSHARED_HOME/SueLib/$line
       foreach file [glob $CPPSIMSHARED_HOME/SueLib/$line/*.sue] {
          source $file
       }
       if {$found_first_valid_dir_flag == 0} {
           set found_first_valid_dir_flag 1
           set INIT_SCHEM_DIR $CPPSIMSHARED_HOME/SueLib/$line           
       } elseif {$found_first_valid_dir_flag == 1} {
           set found_first_valid_dir_flag 2
           make_icon_listbox $CPPSIMSHARED_HOME/SueLib/$line .icons1 
       } elseif {$found_first_valid_dir_flag == 2} {
           set found_first_valid_dir_flag 3
           make_icon_listbox $CPPSIMSHARED_HOME/SueLib/$line .icons2 
       }
    }
}
close $sue_lib_file

# Setup the default properties that get added to every
# new icon -- this is only a convenience.  Everything gets a name, and
# in spice mode everything gets a M property. 

if {$MODE == "spice"} {
#  set DEFAULT_PROPERTIES {{-type user -name name} {-type user -name M}}
    set DEFAULT_PROPERTIES {{$name} {example_param = $example_param} \
	     {-type user -name name -default x} {-type user -name example_param -default 1.0}}
}

if {$MODE == "verilog"} {
  set DEFAULT_PROPERTIES {{-type user -name name}} 
}


# Now load up any command line libraries.  Unlike the the defaults icons,
# these don't get loaded until they are needed and thus do not show up
# automatically in the icon listbox.
#foreach lib [use_first LIB] {
#  add_auto_path $lib
#}


# Sets the default insert cursor to the correct color
option add *insertBackground $COLORS(fore)

# force wish to load in all of the tclIndexes so we know where stuff came from.
catch perrott

cd $CPPSIM_HOME/SueLib
# call up a blank schematic
make_new_schematic no_name




# if they get lost accidentally, we can find them again
save_bindings

# and hide the sue main window.  
wm protocol $WIN WM_DELETE_WINDOW modify_exit
wm withdraw .

# make a happy message for the user
set WIN_DATA($WIN,display_msg) "Welcome to Sue2 (version 1.0) - see the COPYING file for details on copyright/licensing issues"





