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

# Random useful generic utility procedures for sue.

# checks the tag list of an id to see if the tag is found

proc is_tagged {id tag} {

  global cur_c

  set cur_c_tags [$cur_c gettags $id]
  set check_tag [lsearch $cur_c_tags $tag]
  if {$check_tag == -1} {
     return 0
  } else {
     return 1
  }
}

# finds the id of the origin of the object with a tag inst#
# if the # is missing, then the object is the origin

proc find_origin {id} {

  global cur_c

  set tags [$cur_c gettags $id]

  # look for inst# in the tags
  set inst_index [lsearch -regexp $tags inst]
  if {$inst_index == -1} {
    return $id
  }
  # extract out # out of inst#
  return [string range [lindex $tags $inst_index] 4 end]
}


# finds the tag that matches everyone in the group given by id.

proc find_origin_tag {id} {

  global cur_c

  set tags [$cur_c gettags $id]

  # look for inst# in the tags
  set inst_index [lsearch -regexp $tags inst]
  if {$inst_index == -1} {
     return $id
  }
  return [lindex $tags $inst_index]
}


# Finds the center of either a rectangle or line box from its id

proc center {id} {

  global cur_c

  set c [$cur_c coords $id]
  set type [$cur_c type $id]


  if {$type == "line"} {
      set cind0 [lindex $c 0]
      set cind1 [lindex $c 1]
      set cind4 [lindex $c 4]
      set cind5 [lindex $c 5]
      set cind0p4 [expr ${cind0}+${cind4}]
      set cind1p5 [expr ${cind1}+${cind5}]

      return [list [expr ${cind0p4}/2] \
		  [expr ${cind1p5}/2]]
  }
  if {$type == "rectangle" || $type == "arc"} {
      set cind0 [lindex $c 0]
      set cind1 [lindex $c 1]
      set cind2 [lindex $c 2]
      set cind3 [lindex $c 3]
      set cind0p2 [expr ${cind0}+${cind2}]
      set cind1p3 [expr ${cind1}+${cind3}]

      return [list [expr ${cind0p2}/2] \
		  [expr ${cind1p3}/2]]
  }
  if {$type == ""} {
    # id doesn't exist
    return ""
  }
  error "Don't know how to find center of type $type."
}


# Finds the center of the 4 coords or a bbox.

proc center_bbox {bbox} {

    set bboxind0 [lindex $bbox 0]
    set bboxind1 [lindex $bbox 1]
    set bboxind2 [lindex $bbox 2]
    set bboxind3 [lindex $bbox 3]
    set bboxind0p2 [expr ${bboxind0} + ${bboxind2}]
    set bboxind1p3 [expr ${bboxind1} + ${bboxind3}]
    set bboxind0p2d2 [expr ${bboxind0p2} / 2]
    set bboxind1p3d2 [expr ${bboxind1p3} / 2]
    set ret_list [list ${bboxind0p2d2} ${bboxind1p3d2}]
    return $ret_list
}


# Gets the value out of an associative list keyed by arg.  If the optional
# index is given, will get the "index" value out of the list which is
# used for getting the default value.

proc get_assoc {name list {index 1}} {

  foreach assoc $list {
    if {[lindex $assoc 0] == $name} {
      return [lindex $assoc $index]
    }
  }
  return ""
}


proc put_assoc {value name list {index 1}} {

  set new_list ""
  foreach assoc $list {
    if {[lindex $assoc 0] == $name} {
      lappend new_list [lreplace $assoc $index $index $value]
    } else {
      lappend new_list $assoc
    }
  }
  return $new_list
}


# copies arrays

proc copy_array {new_name old_name} {

  upvar 1 $new_name new
  upvar 1 $old_name old
  
  if {[info exists new]} {
    unset new
  }

  foreach name [array names old] {
    set new($name) $old($name)
  }
}


# tags with new_tag all objects that have both tag1 and tag2

proc intersect_tag {new_tag tag1 tag2} {

  global cur_c

  $cur_c addtag Xsect all
  $cur_c dtag $tag1 Xsect
  $cur_c addtag $new_tag withtag $tag2
  $cur_c dtag Xsect $new_tag
  $cur_c dtag Xsect
}


# returns all id's which contain both tag1 and tag2

proc get_intersect_tag {tag1 tag2} {

  global cur_c

  $cur_c addtag Xsect1 all
  $cur_c dtag $tag1 Xsect1
  $cur_c addtag Xsect2 withtag $tag2
  $cur_c dtag Xsect1 Xsect2
  $cur_c dtag Xsect1

  set list [$cur_c find withtag Xsect2]
  $cur_c dtag Xsect2

  return $list
}


# simple procedure to clear the current canvas

proc clear {{tag all}} {

  global cur_c

  $cur_c delete $tag
}


# provides information about the state of the current canvas for debugging

proc info_canvas {{grep ""}} {

  global cur_c

  set alltags [$cur_c find all]
  foreach i $alltags {
    set info "$i [$cur_c type $i] [$cur_c gettags $i]"
    if {$grep == "" || [string first $grep $info] != -1} {
      puts $info
    }
  }
}


# Pretty prints a number using the spice metric suffixes

proc pp_number {num {format %g%s}} {
    if {$num == 0 || $num == Inf || $num == -Inf} {
	return 0
    }
    set suffixes {G M K "" m u n p f a}
    set logscale [expr floor(log10(abs(${num}))/3.0)]
    set mantissa [expr ${num}/pow(10,3.0*${logscale})]
    set exp [lindex $suffixes [expr round(3 - ${logscale})]]
    return [format $format $mantissa $exp]
}


# Parses a string with spice notation into a number.  Trailing units
# are ignored.  Examples: 3.2e-4, 32.4fF, 23 Ohms

proc parse_pp_number {string} {
  regexp -indices -nocase {[a-d]|[f-z]} $string index
  
  if {[info exists index] == 0} {
    return $string
  }

  set indexind0 [lindex $index 0]
  set mantissa [string range $string 0 [expr ${indexind0} - 1]]

  set suffixes "GMK munpfa"
  set suffix [string range $string [lindex $index 0] [lindex $index 1]]
  set pos [string first $suffix $suffixes]
  if {$pos == -1} {
    return $mantissa
  }
  set num [expr ${mantissa} * pow(10,3.0*(3-${pos}))]
  return $num
}

# This routine returns the procedure name of the calling routine
# an optional argument returns the nth calling procedure

proc current_procedure {{up 0}} {

  # get level
  set level [info level]

  # Actually want level of caller
  set level [expr ${level} - 1 - ${up}]

  set name [lindex [info level $level] 0]

  return $name
}


###########################################################################
# if two sets of coords are nearby (i.e. within del of eachother) then
# return a 1, otherwise return a 0.  This in theory is not required because
# of gridding.
###########################################################################

proc nearby {x1 y1 x2 y2} {

  global scale

  set del [expr ${scale}/3.0]

  if {[expr abs(${x1}-${x2})] < ${del} && [expr abs(${y1}-${y2})] < ${del}} {
    return 1
  }
  return 0
}

proc nearby_num {x1 x2} {

  global scale

  set del [expr ${scale}/3.0]

  if {[expr abs(${x1}-${x2})] < ${del}} {
    return 1
  }
  return 0
}


# Accepts a list of numbers and rounds them to the nearest integer, 
# returning the rounding value in a new list

proc round_list {x} {

  global scale

  set out ""
  foreach y $x {
     if {$scale == 10} {
	 set ycoarse [expr ${y} / 10]
	 lappend out [expr round(${ycoarse}) * 10]
     } else {
	 lappend out [expr round(${y})]
     }
  }

  return $out
}

proc round_list_by_ten {x} {

  set out ""
  foreach y $x {
      set ycoarse [expr ${y} / 10]
      lappend out [expr round(${ycoarse}) * 10]
  }

  return $out
}


# this could be merged into round_list but tcl is so pathetically slow
# with numbers, I couldn't do it.

proc round_list_scale {x scale} {

  set out ""
  foreach y $x {
      set ycoarse [expr ${y} / ${scale}]
      lappend out [expr round(${ycoarse})*${scale}]
  }

  return $out
}


# reverses the elements in a list

proc lreverse_sue2 {list} {

  set new_list ""

  foreach element $list {
    set new_list [concat $element $new_list]
  }

  return $new_list
}


# returns maximum of two numbers

proc max {a b} {

  if {$a > $b} {
    return $a

  } else {
    return $b
  }
}

# returns minimum of two numbers

proc min {a b} {

  if {$a < $b} {
    return $a

  } else {
    return $b
  }
}

# returns the size of the text with the given id as a keyword pair or ""
# if it is the default (standard).

proc text_size {id} {

  global cur_c

  set tags [$cur_c gettags $id]
  set size [string range [lindex $tags [lsearch $tags size_*]] 5 end]
  if {$size == "standard"} {
    return ""
  } else {
    return " -size $size"
  }
}


# returns the anchor position of the text with the given id as a keyword
# pair of "" if it is the default (w).  Only works for icons where it
# makes any sense.

proc text_anchor {id} {

  global cur_c cur_s

  set anchor [lindex [$cur_c itemconfigure $id -anchor] 4]
  if {$anchor == "w"} {
    return ""
  } else {
    return " -anchor $anchor"
  }
}


# returns the value of the first variable name in the list that is defined
# and not equal to "".  If the first character of the name is ' then
# it is a literal.

proc use_first args {
  
  foreach var $args {
    if {[string index $var 0] == "'"} {
      return [string range $var 1 end]
    }
    upvar 1 $var name
    if {[info exists name]} {
       if {$name != ""} {
	   return $name
       }
    }
  }
}



# just like incr but if the argument isn't defined, sets to 0.

proc incrX {var {inc 1}} {

  upvar 1 $var data

  if {[info exists data] != 1} {
    set data 0
  }

  incr data $inc
}


# inserts the args into the list if they are not already in it.

proc insert_unique {qlist pos args} {

  upvar 1 $qlist list

  foreach element $args {
    if {[lsearch $list $element] == -1} {
      set list [linsert $list $pos $element]
    }
  }
  return $list
}


# saves up error messages.  When flushed will give a popup window if
# the appropriate switch is set in .suerc

proc sue_error {message {cell ""}} {

  global SUE_ERRORS ERROR_POPUP NONEWLINE NETLIST_CACHE NETLIST

  if {$message == "flush"} {
    if {[use_first SUE_ERRORS] == ""} {
      # no errors to report
      return
    }

    if {$ERROR_POPUP == 1} {
      # make a popup and show all the happy errors
      set button [tk_dialog_new .errors "ERRORS" \
		      [join $SUE_ERRORS "\n"] \
		      "" 0 {ok} {complain}]

      if {$button == 1} {
	# user hit the complain key
	set button [tk_dialog_new .errors "COMPLAINTS" \
			"What do you mean?\nThis bug is yours!" \
			"" 0 {ok}]
      }
    }

    # clear away those nasty errors
    set SUE_ERRORS ""

    set NETLIST(error) 0

    return
  }

  if {[use_first NONEWLINE] == 1} {
    puts ""
    set NONEWLINE 0
  }

  puts $message
  lappend SUE_ERRORS $message

  set NETLIST_CACHE($cell,error) error

  set NETLIST(error) 1

  return
}


proc sue_warning {message} {

  global NONEWLINE

  if {[use_first NONEWLINE] == 1} {
    puts ""
    set NONEWLINE 0
  }

  puts $message

  return
}


# cleans up the directory by turning it into an absolute path and removing
# any auto-mount garbage.

proc clean_dir {dir} {

  global AUTO_MOUNT_PREFIX

  set dir_new $dir
  if {[string index $dir_new 0] == "."} {
    set dir_new "[pwd][string range $dir_new 1 end]"
  }

  # make a relative path into an absolute path
#  if {[string index $dir_new 0] != "/"} {
#    set dir_new "[pwd]/$dir_new"
#  }

  # don't we just love auto-mounters???
  set bogus [lindex [split $dir_new /] 1]
  if {$bogus == $AUTO_MOUNT_PREFIX} {
    set dir_new /[join [lrange [split $dir_new /] 2 end] /]
  }

  return $dir_new
}


# Makes it easy to reload changes/fixes.

proc reload {file} {

  global CPPSIMSHARED_HOME

  set file_new $file
  if {[string first . $file_new] == -1} {
    set file_new "$file_new.tcl"
  }

  puts "Sourcing $CPPSIMSHARED_HOME/Sue2/src/$file_new"
  # need to source this file in the top level context
  uplevel #0 "source $CPPSIMSHARED_HOME/Sue2/src/$file_new"
}


# Translates numbers like 2x and 3x into '2*Wmin', '3*Wmin'

proc x {exp} {

  set exp_new $exp
  regsub {^([0-9\.]+)(x)$} $exp_new {'\1*Wmin'} exp_new

  return $exp_new
}
