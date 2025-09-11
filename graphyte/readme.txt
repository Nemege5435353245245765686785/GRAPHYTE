this is GRAPHYTE, a graphics library for a 16x2 display running
on a raspberry pi.

HOW TO SET UP:
First, You need to put all the files in this archive in a folder.

then you need to compile each .c file 

gcc lcd.c -lpigpio -o lcd
gcc lcd_editor.c -o lcd_editor

HOW TO USE IT:

you have to start the lcd driver first.

./lcd_editor --start-lcd

if you want to stop the lcd driver:

./lcd_editor --kill-lcd


and then you can draw lines, rectangles, circles,draw text, 
draw scrolling text,

GRAPHYTE is a wrapper for this c library i made to make it possible
to interface with python. 

you have to import this library to python (graphyte and all the library
files have to be in the same folder.)

import graphyte

os.system("./lcd_editor --start-lcd") → you have to run this before
anything else

os.system("./lcd_editor --kill-lcd") → this stops the running lcd driver

run_cmd(args) → Internal helper that runs ./lcd_editor with given arguments.

clear() → Clears all pixels on the display.

draw_text(x, y, text, font=None) → Draws a text string at (x,y) using an optional font file.

scroll_text(y, speed, text, direction=1, font=None) → Scrolls text horizontally on row y with speed and direction.

draw_line(x0, y0, x1, y1, invert=False) → Draws (or inverts) a line between two points.

draw_rect(x, y, w, h, filled=False, invert=False) → Draws a rectangle (outline, filled, or inverted).

draw_circle(xc, yc, r, filled=False, invert=False) → Draws a circle (outline, filled, or inverted).

enable_debug() → Runs in debug mode and prints the pixel matrix to the terminal.




these are examples for calling the c library directly without the wrapper
(graphyte.py)

--debug → Enables debug mode and prints the pixel buffer.
Example: ./lcd_editor --debug

--start-lcd → Starts the ./lcd driver process.
Example: ./lcd_editor --start-lcd

--kill-lcd → Stops the running ./lcd driver process.
Example: ./lcd_editor --kill-lcd

--font <file> → Loads a font file for text rendering.
Example: ./lcd_editor --font myfont.txt

--text <x> <y> <string> → Draws static text at (x,y).
Example: ./lcd_editor --font myfont.txt --text 0 0 "Hello"

--scroll-text <y> <speed_ms> <string> <dir> → Scrolls text on line y at given speed and direction (1=left, -1=right).
Example: ./lcd_editor --font myfont.txt --scroll-text 0 50 "Scrolling" 1

--clear → Clears the display.
Example: ./lcd_editor --clear

--line <x0> <y0> <x1> <y1> → Draws a line between (x0,y0) and (x1,y1).
Example: ./lcd_editor --line 0 0 79 15

--invert-line <x0> <y0> <x1> <y1> → Inverts pixels along a line.
Example: ./lcd_editor --invert-line 0 0 79 15

--rect <x> <y> <w> <h> → Draws rectangle outline.
Example: ./lcd_editor --rect 10 2 20 5

--invert-rect <x> <y> <w> <h> → Inverts rectangle border pixels.
Example: ./lcd_editor --invert-rect 10 2 20 5

--rect-filled <x> <y> <w> <h> → Draws filled rectangle.
Example: ./lcd_editor --rect-filled 10 2 20 5

--invert-rect-filled <x> <y> <w> <h> → Inverts pixels inside rectangle.
Example: ./lcd_editor --invert-rect-filled 10 2 20 5

--circle <xc> <yc> <r> → Draws circle outline.
Example: ./lcd_editor --circle 40 8 5

--invert-circle <xc> <yc> <r> → Inverts pixels along circle outline.
Example: ./lcd_editor --invert-circle 40 8 5

--circle-filled <xc> <yc> <r> → Draws filled circle.
Example: ./lcd_editor --circle-filled 40 8 5

--invert-circle-filled <xc> <yc> <r> → Inverts pixels inside filled circle.
Example: ./lcd_editor --invert-circle-filled 40 8 5


HOW TO MAKE A FONT: 

i also made a tool for this. as you can see in the archive folder there
is a python script called font_maker.py 
if you run this script on your computer, you can download fonts
off the internet and load them to the display

what this script does is spit out a txt file which is human readable
and editable if you import a txt file.

when calling scroll_text(), you will need a font file that has a .txt
format.

HOW TO USE:

you first have to install PIL:

pip3 install PIL

and then you can use it with:

python3 font_maker.py your_font.ttf

this will create a file named your_font.txt which you can use 
with graphyte.

