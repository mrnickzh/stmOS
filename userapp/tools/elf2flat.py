#!/usr/bin/env python3
import sys, struct, subprocess, os
from elftools.elf.elffile import ELFFile
from elftools.elf.relocation import RelocationSection
from elftools.elf.enums import ENUM_RELOC_TYPE_ARM

FLAT_MAGIC = 0x464C4154
HEADER_FMT = '<10I'   # 40 байт

def main():
    if len(sys.argv) != 3:
        print("usage: elf2flat.py in.elf out.flat"); sys.exit(1)
    
    elf_path = sys.argv[1]
    out_path = sys.argv[2]
    
    # 1. Извлекаем чистый бинарный образ (text + rodata + data) через objcopy
    bin_path = out_path + ".tmp.bin"
    subprocess.check_call(["arm-none-eabi-objcopy", "-O", "binary", elf_path, bin_path])
    
    with open(bin_path, "rb") as f:
        image = f.read()
    os.remove(bin_path)
    
    # 2. Читаем ELF для получения размеров и релокаций
    with open(elf_path, 'rb') as f:
        elf = ELFFile(f)
        
        # Получаем размер bss
        bss_sec = elf.get_section_by_name('.bss')
        bss_size = bss_sec['sh_size'] if bss_sec else 0
        
        # Получаем точку входа
        entry = elf.header.e_entry
        
        # Определяем text_size и data_size
        data_sec = elf.get_section_by_name('.data')
        if data_sec:
            text_size = data_sec['sh_addr']
            data_size = len(image) - text_size
        else:
            text_size = len(image)
            data_size = 0
            
        total_memory = len(image) + bss_size
        
        # 3. Собираем только ВАЛИДНЫЕ R_ARM_ABS32
        relocs = []
        # Только эти секции содержат реальные R_ARM_ABS32
        TARGET_SECTIONS = {'.text', '.data', '.rodata'}

        for rsec in elf.iter_sections():
            if not isinstance(rsec, RelocationSection): 
                continue
            
            # Get target section name
            if rsec.name.startswith('.rela'):
                tgt_name = rsec.name[5:]
            elif rsec.name.startswith('.rel'):
                tgt_name = rsec.name[4:]
            else:
                continue
            
            # ⚠️ ПРОПУСКАЕМ .debug_*, .ARM.exidx, .comment — только реальные
            if tgt_name not in TARGET_SECTIONS:
                continue
            
            tgt_sec = elf.get_section_by_name(tgt_name)
            if tgt_sec is None: 
                continue
            
            for r in rsec.iter_relocations():
                if r['r_info_type'] != ENUM_RELOC_TYPE_ARM['R_ARM_ABS32']:
                    continue
                relocs.append(tgt_sec['sh_addr'] + r['r_offset'])
        
        # 4. Записываем .flat
        reloc_off = 40 + len(image)
        
        with open(out_path, 'wb') as out:
            out.write(struct.pack(HEADER_FMT,
                FLAT_MAGIC, 1, 0, entry,
                text_size, data_size, bss_size, 1024,
                reloc_off, len(relocs)))
            out.write(image)
            for r in relocs:
                out.write(struct.pack('<I', r))
        
        print(f"OK: entry={entry:#x}, text={text_size}, data={data_size}, "
              f"bss={bss_size}, relocs={len(relocs)}")

if __name__ == '__main__':
    main()