import os
from PIL import Image, ImageSequence

GIF_FILE = "FVB.gif"
OUTPUT_H = "gif_data.h"

def rgb888_to_rgb565(r, g, b):
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)

def convert_with_compression():
    with Image.open(GIF_FILE) as im:
        width, height = im.size
        frames = [f.convert('RGB') for f in ImageSequence.Iterator(im)]
        
        with open(OUTPUT_H, "w") as f:
            f.write("#include <pgmspace.h>\n")
            f.write(f"#define FRAME_WIDTH {width}\n#define FRAME_HEIGHT {height}\n")
            f.write(f"#define FRAME_COUNT {len(frames)}\n\n")

            last_frame = None
            for i, frame in enumerate(frames):
                diff_data = []
                for y in range(height):
                    for x in range(width):
                        curr_p = frame.getpixel((x, y))
                        curr_565 = rgb888_to_rgb565(*curr_p)
                        
                        # إذا كان أول إطار، أو البكسل تغير عن الإطار السابق
                        if last_frame is None or curr_565 != rgb888_to_rgb565(*last_frame.getpixel((x, y))):
                            # تخزين: X(1 byte), Y(1 byte), Color(2 bytes)
                            diff_data.append(f"{x},{y},0x{curr_565:04X}")
                
                f.write(f"const uint16_t frame{i}[] PROGMEM = {{{len(diff_data)}, {', '.join(diff_data)}}};\n")
                last_frame = frame
            
            f.write("\nconst uint16_t* const frames[] PROGMEM = {" + ", ".join([f"frame{i}" for i in range(len(frames))]) + "};\n")
    print("Done! Check gif_data.h")

convert_with_compression()
