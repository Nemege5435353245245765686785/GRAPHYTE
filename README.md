# GRAPHYTE

**GRAPHYTE** is a graphics library for a 16x2 display running on a Raspberry Pi.  
It provides both a **C-based display driver** and a **Python wrapper**, making it easy to draw shapes, render text, and scroll content on a character LCD.

---

## 📦 Setup

1. Put all files from this repository into a folder.
2. Compile the C programs:

```bash
gcc lcd.c -l wiringPi -o lcd
gcc lcd_editor.c -o lcd_editor
```

---

## 🚀 Usage

### Starting and Stopping the LCD Driver
Before drawing, you need to start the LCD driver:
```bash
./lcd_editor --start-lcd
```

To stop it:
```bash
./lcd_editor --kill-lcd
```

---

## 🐍 Python Wrapper (graphyte.py)

To use GRAPHYTE with Python, make sure `graphyte.py` and the compiled files are in the same folder.

```python
import os
import graphyte

# Start the LCD driver
os.system("./lcd_editor --start-lcd")

# Example calls
graphyte.clear()
graphyte.draw_text(0, 0, "Hello!")
graphyte.draw_line(0, 0, 79, 15)
graphyte.draw_circle(40, 8, 5, filled=True)
graphyte.scroll_text(0, 50, "Scrolling Text", direction=1)

# Stop the LCD driver
os.system("./lcd_editor --kill-lcd")
```

### Python Functions
- `run_cmd(args)` → Runs `./lcd_editor` with given arguments.  
- `clear()` → Clears all pixels on the display.  
- `draw_text(x, y, text, font=None)` → Draws text at `(x, y)` using an optional font.  
- `scroll_text(y, speed, text, direction=1, font=None)` → Scrolls text on row `y`.  
- `draw_line(x0, y0, x1, y1, invert=False)` → Draws (or inverts) a line.  
- `draw_rect(x, y, w, h, filled=False, invert=False)` → Draws a rectangle.  
- `draw_circle(xc, yc, r, filled=False, invert=False)` → Draws a circle.  
- `enable_debug()` → Prints the pixel matrix to the terminal.  

---

## ⚙️ C Library Commands

You can also call the `lcd_editor` binary directly.

| Command | Description | Example |
|---------|-------------|---------|
| `--debug` | Enable debug mode | `./lcd_editor --debug` |
| `--start-lcd` | Start the LCD driver | `./lcd_editor --start-lcd` |
| `--kill-lcd` | Stop the LCD driver | `./lcd_editor --kill-lcd` |
| `--font <file>` | Load a font file | `./lcd_editor --font myfont.txt` |
| `--text <x> <y> <string>` | Draw static text | `./lcd_editor --font myfont.txt --text 0 0 "Hello"` |
| `--scroll-text <y> <speed> <string> <dir>` | Scroll text | `./lcd_editor --scroll-text 0 50 "Scrolling" 1` |
| `--clear` | Clear the display | `./lcd_editor --clear` |
| `--line <x0> <y0> <x1> <y1>` | Draw line | `./lcd_editor --line 0 0 79 15` |
| `--invert-line <x0> <y0> <x1> <y1>` | Invert line | `./lcd_editor --invert-line 0 0 79 15` |
| `--rect <x> <y> <w> <h>` | Rectangle outline | `./lcd_editor --rect 10 2 20 5` |
| `--invert-rect <x> <y> <w> <h>` | Invert rectangle border | `./lcd_editor --invert-rect 10 2 20 5` |
| `--rect-filled <x> <y> <w> <h>` | Filled rectangle | `./lcd_editor --rect-filled 10 2 20 5` |
| `--invert-rect-filled <x> <y> <w> <h>` | Invert filled rectangle | `./lcd_editor --invert-rect-filled 10 2 20 5` |
| `--circle <xc> <yc> <r>` | Circle outline | `./lcd_editor --circle 40 8 5` |
| `--invert-circle <xc> <yc> <r>` | Invert circle outline | `./lcd_editor --invert-circle 40 8 5` |
| `--circle-filled <xc> <yc> <r>` | Filled circle | `./lcd_editor --circle-filled 40 8 5` |
| `--invert-circle-filled <xc> <yc> <r>` | Invert filled circle | `./lcd_editor --invert-circle-filled 40 8 5` |

---

## ✍️ Font Creation

GRAPHYTE includes a font tool: `font_maker.py`.  
It lets you convert `.ttf` fonts into human-readable `.txt` fonts for the LCD.

### Install Dependencies
```bash
pip3 install PIL
```

### Generate a Font
```bash
python3 font_maker.py your_font.ttf
```

This will create `your_font.txt`, which can be used with:
```python
graphyte.draw_text(0, 0, "Hello", font="your_font.txt")
```

or:
```bash
./lcd_editor --font your_font.txt --text 0 0 "Hello"
```

---

## 🖼️ Features
- Clear and reset display
- Draw text with custom fonts
- Scroll text horizontally
- Draw lines, rectangles, and circles
- Filled and inverted shapes
- Debug mode (view pixel matrix in terminal)
- Python wrapper for easy scripting

---

## 📌 Notes
- Always start the LCD driver before drawing:  
  `./lcd_editor --start-lcd`  
- Stop it when done:  
  `./lcd_editor --kill-lcd`

---

## 📄 License
This project is released under the **MIT License**.
