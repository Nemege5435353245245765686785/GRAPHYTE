import math
import time
from lcd_editor import clear, draw_line

# Cube vertices (x,y,z), centered around origin
CUBE_VERTICES = [
    [-1, -1, -1],
    [ 1, -1, -1],
    [ 1,  1, -1],
    [-1,  1, -1],
    [-1, -1,  1],
    [ 1, -1,  1],
    [ 1,  1,  1],
    [-1,  1,  1],
]

# Cube edges (pairs of vertex indices)
CUBE_EDGES = [
    (0,1),(1,2),(2,3),(3,0),  # back face
    (4,5),(5,6),(6,7),(7,4),  # front face
    (0,4),(1,5),(2,6),(3,7)   # connecting edges
]

# Projection function (simple perspective)
def project(x, y, z, scale=10, distance=10, width=80, height=16):
    factor = scale / (z + distance)
    px = int(width//2 + x * factor * (height/2))   # keep cube inside screen
    py = int(height//2 - y * factor * (height/2))
    return px, py

# Rotation function
def rotate(vertex, ax, ay, az):
    x, y, z = vertex

    # Rotate around X
    cosx, sinx = math.cos(ax), math.sin(ax)
    y, z = y*cosx - z*sinx, y*sinx + z*cosx

    # Rotate around Y
    cosy, siny = math.cos(ay), math.sin(ay)
    x, z = x*cosy + z*siny, -x*siny + z*cosy

    # Rotate around Z
    cosz, sinz = math.cos(az), math.sin(az)
    x, y = x*cosz - y*sinz, x*sinz + y*cosz

    return [x, y, z]

# -----------------------------
# Demo: spinning cube
def spinning_cube():
    angle = 0
    while True:
        clear()
        rotated = [rotate(v, angle, angle, angle) for v in CUBE_VERTICES]
        projected = [project(*v) for v in rotated]

        for edge in CUBE_EDGES:
            x0, y0 = projected[edge[0]]
            x1, y1 = projected[edge[1]]
            draw_line(x0, y0, x1, y1)

        time.sleep(0.1)
        angle += 0.1

# -----------------------------
if __name__ == "__main__":
    spinning_cube()
