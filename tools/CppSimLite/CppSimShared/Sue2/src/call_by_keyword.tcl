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

# The call_by_keyword procedure is meant to facilitate call by keyword in tcl 
# instead of call by order.  A procedure "foo" with variables a, b, c can
# be called, for example, by
#
#      foo -b 23 -c {bar qux}
#
# if it is defined as follows
#
# proc foo {args} {
#    call_by_keyword $args {{b 34} {c {hi there}}}
#    ...
# }
#
# call_by_keyword will define, in the context of foo, the variables b,c,
# the array default_value which stores the default values of b,c, and
# the array defaulted which stores whether the default value was used (1)
# or not (0).


# Note all local variables used in call_by_keyword will interfere with the
# calling procedures.

proc call_by_keyword {_ARG_LIST _DEFAULT_LIST} {
  
  upvar 1 default_value default_value
  upvar 1 defaulted defaulted
  foreach _DEFAULT $_DEFAULT_LIST {
    set _ARG_NAME [lindex $_DEFAULT 0] 
    upvar 1 $_ARG_NAME $_ARG_NAME
    set $_ARG_NAME [lindex $_DEFAULT 1]
    set default_value($_ARG_NAME) [set $_ARG_NAME]
    set defaulted($_ARG_NAME) 1
  }

  for {set _INDEX 0} {$_INDEX < [llength $_ARG_LIST]} {incr _INDEX 2} {
    set _ARG_NAME [string range [lindex $_ARG_LIST $_INDEX] 1 end]
    set _ARG_VALUE [lindex $_ARG_LIST [expr $_INDEX+1]]
    
    if {[info exists $_ARG_NAME] == 0} {
      error "\"$_ARG_NAME\" is not a valid argument to call_by_keyword."
    }
    set $_ARG_NAME $_ARG_VALUE
    set defaulted($_ARG_NAME) 0
  }
}


# like call_by_keyword. returns a list of its arguments not keyworded, e.g.:
#      foo -b 23 hi -c {bar qux} there
# sets b=23, c={bar qux} and returns hi there.

proc call_with_keyword {_ARG_LIST _DEFAULT_LIST} {
  
  set _RETURN ""

  upvar 1 default_value default_value
  upvar 1 defaulted defaulted
  foreach _DEFAULT $_DEFAULT_LIST {
    set _ARG_NAME [lindex $_DEFAULT 0] 
    upvar 1 $_ARG_NAME $_ARG_NAME
    set $_ARG_NAME [lindex $_DEFAULT 1]
    set default_value($_ARG_NAME) [set $_ARG_NAME]
    set defaulted($_ARG_NAME) 1
  }

  for {set _INDEX 0} {$_INDEX < [llength $_ARG_LIST]} {} {
    if {[string index [lindex $_ARG_LIST $_INDEX] 0] == "-"} {
      # is a keyword pair
      set _ARG_NAME [string range [lindex $_ARG_LIST $_INDEX] 1 end]
      set _ARG_VALUE [lindex $_ARG_LIST [expr $_INDEX+1]]
    
      if {[info exists $_ARG_NAME] == 0} {
	error "\"$_ARG_NAME\" is not a valid argument to call_with_keyword."
      }
      set $_ARG_NAME $_ARG_VALUE
      set defaulted($_ARG_NAME) 0
      incr _INDEX 2

    } else {
      # not a keyword pair
      lappend _RETURN [lindex $_ARG_LIST $_INDEX]

      incr _INDEX
    }
  }
  return $_RETURN
}


# like call_with_keyword but returns a list of arguments not keyworded AND
# keyworded arguments not in default list.  unlike call_with_keyword,
# doesn't error if arguments aren't in default list.

proc call_use_keyword {_ARG_LIST _DEFAULT_LIST} {
  
  set _RETURN ""

  upvar 1 default_value default_value
  upvar 1 defaulted defaulted
  foreach _DEFAULT $_DEFAULT_LIST {
    set _ARG_NAME [lindex $_DEFAULT 0] 
    upvar 1 $_ARG_NAME $_ARG_NAME
    set $_ARG_NAME [lindex $_DEFAULT 1]
    set default_value($_ARG_NAME) [set $_ARG_NAME]
    set defaulted($_ARG_NAME) 1
  }

  for {set _INDEX 0} {$_INDEX < [llength $_ARG_LIST]} {} {
    if {[string index [lindex $_ARG_LIST $_INDEX] 0] == "-"} {
      # is a keyword pair
      set _ARG_NAME [string range [lindex $_ARG_LIST $_INDEX] 1 end]
      set _ARG_VALUE [lindex $_ARG_LIST [expr $_INDEX+1]]
    
      if {[info exists $_ARG_NAME] == 0} {
	lappend _RETURN -$_ARG_NAME $_ARG_VALUE
      } else {
	set $_ARG_NAME $_ARG_VALUE
	set defaulted($_ARG_NAME) 0
      }
      incr _INDEX 2

    } else {
      # not a keyword pair
      lappend _RETURN [lindex $_ARG_LIST $_INDEX]

      incr _INDEX
    }
  }
  return $_RETURN
}

    
proc test {args} {
  call_by_keyword $args {{a 4} {b ""}}
  puts "a=$a def=$default_value(a) $defaulted(a) b=$b def=$default_value(b)"
}

proc test2 {args} {
  set value [call_with_keyword $args {{a 4} {b ""}}]
  puts "a=$a def=$default_value(a) $defaulted(a) b=$b def=$default_value(b)"
  return $value
}
