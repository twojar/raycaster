import sys
import os
from PIL import Image, ImageFont, ImageDraw

def generate_font_map(ttf_path, output_path="assets/textures/font.png", char_size=32):
    """
    Converts a TTF file into a retro-style PNG font map.
    Grid: 16x16 characters
    Default Character size: 32x32 (for 512x512 map)
    """
    if not os.path.exists(ttf_path):
        print(f"Error: TTF file not found at {ttf_path}")
        return

    grid_dim = 16
    img_size = char_size * grid_dim
    img = Image.new("RGBA", (img_size, img_size), (0, 0, 0, 0))
    
    try:
        # Load the font slightly smaller to ensure it fits in the box
        font = ImageFont.truetype(ttf_path, char_size - 4)
    except Exception as e:
        print(f"Error loading font: {e}")
        return

    print(f"Generating sharp {char_size}x{char_size} font map ({img_size}x{img_size})...")

    for i in range(256):
        char = chr(i)
        mask = font.getmask(char, mode="1")
        mask_w, mask_h = mask.size
        
        # Create a temp 1-bit image from the mask
        mask_img = Image.new("1", (mask_w, mask_h))
        mask_img.putdata(mask)
        
        char_img = Image.new("RGBA", (char_size, char_size), (0, 0, 0, 0))
        off_x = max(0, (char_size - mask_w) // 2)
        off_y = max(0, (char_size - mask_h) // 2)
        
        char_draw = Image.new("RGBA", (mask_w, mask_h), (255, 255, 255, 255))
        char_img.paste(char_draw, (off_x, off_y), mask_img)
        
        col = i % grid_dim
        row = i // grid_dim
        img.paste(char_img, (col * char_size, row * char_size))

    img.save(output_path)
    print(f"Success! Sharp font map saved to: {output_path}")

if __name__ == "__main__":
    path = sys.argv[1] if len(sys.argv) > 1 else "tools/PixelTimesNewRoman.ttf"
    size = int(sys.argv[2]) if len(sys.argv) > 2 else 16
    generate_font_map(path, char_size=size)
