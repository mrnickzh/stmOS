#!/usr/bin/env python3
import sys, os

def main():
    if len(sys.argv) != 3:
        print("usage: bin2c.py in.bin out.h"); sys.exit(1)
    
    with open(sys.argv[1], 'rb') as f:
        data = f.read()
    
    name = os.path.basename(sys.argv[2]).replace('.', '_').upper()
    
    with open(sys.argv[2], 'w') as f:
        f.write(f"#ifndef {name}_H\n#define {name}_H\n\n")
        f.write(f"#include <stdint.h>\n\n")
        f.write(f"static const uint8_t g_{name.lower()}_data[{len(data)}] = {{\n")
        for i in range(0, len(data), 12):
            chunk = data[i:i+12]
            f.write("    " + ", ".join(f"0x{b:02X}" for b in chunk) + ",\n")
        f.write("};\n\n")
        f.write(f"static const uint32_t g_{name.lower()}_size = {len(data)}U;\n\n#endif\n")
    
    print(f"Generated {sys.argv[2]}: {len(data)} bytes")

main()