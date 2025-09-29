import subprocess
import os
import time

LCD_EDITOR = "./lcd_editor"

def run_cmd(args):
    """Run lcd_editor with given arguments and return output."""
    cmd = [LCD_EDITOR] + args
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, check=True)
        return result.stdout.strip()
    except subprocess.CalledProcessError as e:
        return e.stderr.strip()

# -----------------------------
# Driver control
def start_lcd():
    return run_cmd(["--start-lcd"])

def kill_lcd():
    return run_cmd(["--kill-lcd"])

# -----------------------------
# Display operations
def clear():
    return run_cmd(["--clear"])

def draw_text(x, y, text, font=None):
    args = []
    if font:
        args += ["--font", font]
    args += ["--text", str(x), str(y), text]
    return run_cmd(args)

def scroll_text(y, speed, text, direction=1, font=None):
    args = []
    if font:
        args += ["--font", font]
    args += ["--scroll-text", str(y), str(speed), text, str(direction)]
    return run_cmd(args)

def draw_line(x0, y0, x1, y1):
    return run_cmd(["--line", str(x0), str(y0), str(x1), str(y1)])

def invert_line(x0, y0, x1, y1):
    return run_cmd(["--invert-line", str(x0), str(y0), str(x1), str(y1)])

def draw_rect(x, y, w, h):
    return run_cmd(["--rect", str(x), str(y), str(w), str(h)])

def invert_rect(x, y, w, h):
    return run_cmd(["--invert-rect", str(x), str(y), str(w), str(h)])

def draw_filled_rect(x, y, w, h):
    return run_cmd(["--rect-filled", str(x), str(y), str(w), str(h)])

def invert_filled_rect(x, y, w, h):
    return run_cmd(["--invert-rect-filled", str(x), str(y), str(w), str(h)])

def draw_circle(xc, yc, r):
    return run_cmd(["--circle", str(xc), str(yc), str(r)])

def invert_circle(xc, yc, r):
    return run_cmd(["--invert-circle", str(xc), str(yc), str(r)])

def draw_filled_circle(xc, yc, r):
    return run_cmd(["--circle-filled", str(xc), str(yc), str(r)])

def invert_filled_circle(xc, yc, r):
    return run_cmd(["--invert-circle-filled", str(xc), str(yc), str(r)])

# -----------------------------
# Debug
def enable_debug():
    return run_cmd(["--debug"])

# -----------------------------
# Example usage (if run directly)
if __name__ == "__main__":
    print(clear())
    print(draw_text(0, 0, "GRAPHYTE", "gazette.txt"))
    time.sleep(1)
    print(draw_line(0, 0, 79, 15))
    print(invert_line(0, 0, 79, 15))
    print(draw_rect(10, 2, 20, 5))
    print(invert_rect(10, 2, 20, 5))
    print(draw_filled_rect(5, 5, 10, 4))
    print(invert_filled_rect(5, 5, 10, 4))
    print(draw_circle(40, 8, 6))
    print(invert_circle(40, 8, 6))
    print(draw_filled_circle(40, 8, 6))
    print(invert_filled_circle(40, 8, 6))
