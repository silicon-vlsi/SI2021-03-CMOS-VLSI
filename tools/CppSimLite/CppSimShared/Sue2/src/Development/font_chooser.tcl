#!/usr/bin/wish -f
# OK button callback.  Get the string from getXLFD and write it out.
# The empty {} is where the parameters would go if there were any.
proc ok {} {
  puts [getXLFD]
  exit
}

# build up the XLFD string and return it
proc getXLFD {} {
  global bold italic fontsize
  set fstring ""
  set facename [.fontname get [.fontname curselection]]
  if {$bold} {
    set fstring "*-$facename-bold"
  } else {
    set fstring "*-$facename-medium"
  }
  if {$italic} {
    if {$facename == "Helvetica" || $facename == "Courier"} {
      set fstring "$fstring-o-normal--$fontsize-*"
    } else {
      set fstring "$fstring-i-normal--$fontsize-*"
    }
  } else {
    set fstring "$fstring-r-normal--$fontsize-*"
  }
  return $fstring
}

# reset the font resource of the preview widget
proc resetFont {} {
  .preview configure -font [getXLFD]  
}

# Scrollbar callback, y is either -1 or 1 depending on whether the
# up arrow or down arrow was pressed
proc changeSize y {
  global fontsize
# use regular expression to verify that font size field is a number
  if [regexp {^[0-9]+$} $fontsize] {
    set fontsize [expr $fontsize-$y]
  }
  else {
    set fontsize 1
  }
# can't call resetFont here, since Tk generates errors if a font
# that doesn't exist is requested (unlike the other two tools).
}

# help button (F1) callback
proc help {} {
  puts "no built-in help support"
}

# add this font to the option database Tk uses to determine what
# attributes each of the widgets have (this could have also been done
# using the .Xdefaults file).
option add *font -adobe-helvetica-medium-r-normal--14-140-75-75-p-77-iso8859-1

# tell the window manager how big to make the window (. is main window
# for our application)
wm geometry . 346x334
# global binding of F1 key to the help function
bind all <KeyPress-F1> help

# Create a label widget
label .namel -text "Name:"

# Widget placement is done with the "Placer" geometry manager, since
# that supports exact positioning.  The "Packer", while more powerful
# and more commonly used, uses a geometry management technique that
# makes layout of irregular dialogs like this one more difficult
place .namel -x 8 -y 4 -width 64 -height 32

# There is no scrolling list box in Tk, so create a list box and a
# scrollbar and attach them to each other by setting the commands
# appropriately
listbox .fontname -relief sunken -yscrollcommand ".fnsb set"
scrollbar .fnsb -relief sunken -command ".fontname yview"
place .fontname -x 16 -y 36 -width 184 -height 140
place .fnsb -x 200 -y 36 -width 16 -height 140

# Insert the font names into the list box
foreach name {Charter Clean Courier Helvetica Lucida LucidaBright\
      LucidaTypewriter {New Century Schoolbook} Symbol Times\
      fixed terminal} {
    .fontname insert end $name
}

# no option to be notified when a list item is selected,
# so bind it to the raw button event
bind .fontname <ButtonRelease-1> resetFont

label .sizel -text "Size:"
place .sizel -x 224 -y 40 -width 48 -height 32

# Make a global variables to attach to the entry widget
set fontsize 14
# no way be notified when another entry is selected
# so preview won't change if size is entered manually
entry .fontsize -relief sunken -textvariable fontsize
place .fontsize -x 272 -y 40 -width 40 -height 32
scrollbar .sb -command changeSize
place .sb  -x 312 -y 36 -width 22 -height 40

label .stylel -text "Style:"
place .stylel -x 224 -y 104 -width 52 -height 32

# Make global variables to attach to the checkbuttons
set bold 0
set italic 0
checkbutton .bold -text "Bold" -variable bold -command resetFont
place .bold  -x 276 -y 104 -width 60 -height 28
checkbutton .italic -text "Italic" -variable italic -command resetFont
place .italic -x 276 -y 132 -width 60 -height 28

label .previewl -text "Preview:"
place .previewl  -x 12 -y 200 -width 76 -height 32
set previewtext "Sample text"
entry .preview -relief sunken -textvariable previewtext
place .preview -x 84 -y 200 -width 246 -height 72

# no way to make buttons have the "default" look and behavior
# so leave it out
button .ok -text "OK" -command ok
place .ok  -x 13 -y 288 -width 96 -height 36

# attach the exit command directly to widget, no need for a function
button .cancel -text "Cancel" -command exit
place .cancel  -x 129 -y 288 -width 96 -height 36
button .help -text "Help" -command help
place .help -x 238 -y 288 -width 96 -height 36
