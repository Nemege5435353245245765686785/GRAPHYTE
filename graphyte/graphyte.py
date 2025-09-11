import subprocess
import os

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

def draw_line(x0, y0, x1, y1, invert=False):
    cmd = "--invert-line" if invert else "--line"
    return run_cmd([cmd, str(x0), str(y0), str(x1), str(y1)])

def draw_rect(x, y, w, h, filled=False, invert=False):
    if filled and invert:
        cmd = "--invert-rect-filled"
    elif filled:
        cmd = "--rect-filled"
    elif invert:
        cmd = "--invert-rect"
    else:
        cmd = "--rect"
    return run_cmd([cmd, str(x), str(y), str(w), str(h)])

def draw_circle(xc, yc, r, filled=False, invert=False):
    if filled and invert:
        cmd = "--invert-circle-filled"
    elif filled:
        cmd = "--circle-filled"
    elif invert:
        cmd = "--invert-circle"
    else:
        cmd = "--circle"
    return run_cmd([cmd, str(xc), str(yc), str(r)])

# -----------------------------
# Debug
def enable_debug():
    return run_cmd(["--debug"])

# -----------------------------
# Example usage (if run directly)
if __name__ == "__main__":
    print(clear())                       # Clear display
    print(draw_text(0, 0, "GRAPHYTE", "gazette.txt"))  # Draw text
	time.sleep(1)
    print(draw_line(0, 0, 79, 15))       # Draw diagonal line
    print(draw_rect(10, 2, 20, 5))       # Draw rectangle
    print(draw_circle(40, 8, 6, filled=True))  # Draw filled circle
