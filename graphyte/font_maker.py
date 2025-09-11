from PIL import Image, ImageFont, ImageDraw
import sys, os

# Config
GLYPH_W = 10          # max width per character
GLYPH_H = 16          # height per character
FONT_SIZE = 64        # large render for quality
START_CHAR = 32
END_CHAR = 126

def render_font(ttf_path):
    font = ImageFont.truetype(ttf_path, FONT_SIZE)
    bitmaps = {}

    for code in range(START_CHAR, END_CHAR + 1):
        ch = chr(code)

        # render large character
        img_large = Image.new("L", (FONT_SIZE*2, FONT_SIZE*2), 0)
        draw = ImageDraw.Draw(img_large)
        draw.text((0,0), ch, fill=255, font=font)

        # crop tight bounding box
        bbox = img_large.getbbox()
        if not bbox:
            bbox = (0, 0, FONT_SIZE, FONT_SIZE)
        img_cropped = img_large.crop(bbox)

        # get original size
        w, h = img_cropped.size

        # scale vertically to 16 px
        new_h = GLYPH_H
        # scale horizontally only if wider than 10 px
        new_w = GLYPH_W if w > GLYPH_W else w

        img_resized = img_cropped.resize((new_w, new_h), Image.LANCZOS)

        # create final 10x16 image (pad horizontally if needed)
        img_final = Image.new("L", (GLYPH_W, GLYPH_H), 0)
        x_offset = (GLYPH_W - new_w)//2
        img_final.paste(img_resized, (x_offset,0))

        # convert to 0/1 bitmap
        bitmap = []
        for yy in range(GLYPH_H):
            row = []
            for xx in range(GLYPH_W):
                pixel = img_final.getpixel((xx, yy))
                row.append("1" if pixel > 128 else "0")
            bitmap.append("".join(row))

        bitmaps[ch] = bitmap

    return bitmaps

def save_bitmaps(bitmaps, out_file):
    with open(out_file, "w") as f:
        for ch, bitmap in bitmaps.items():
            f.write(f"CHAR {ord(ch)} '{ch}'\n")
            for row in bitmap:
                f.write(row + "\n")
            f.write("\n")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python font_maker.py path/to/font.ttf")
        sys.exit(1)

    ttf_path = sys.argv[1]
    font_name = os.path.splitext(os.path.basename(ttf_path))[0]
    output_file = f"{font_name}.txt"

    bitmaps = render_font(ttf_path)
    save_bitmaps(bitmaps, output_file)
    print(f"Font bitmaps saved to {output_file}")
