import binascii

def file_to_c_array(filename):
    with open(filename, "rb") as f:
        data = f.read()
    
    with open("audio_data.h", "w") as f:
        f.write("#include <pgmspace.h>\n\n")
        f.write(f"const unsigned char audio_wav[] PROGMEM = {{\n")
        # تحويل البايتات إلى تنسيق Hex
        hex_data = [f"0x{b:02x}" for b in data]
        for i in range(0, len(hex_data), 12):
            f.write("  " + ", ".join(hex_data[i:i+12]) + ",\n")
        f.write(f"}};\n\n")
        f.write(f"#define AUDIO_LEN {len(data)}\n")

file_to_c_array("audioo.wav")
