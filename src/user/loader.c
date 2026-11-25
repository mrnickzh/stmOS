#include "user/loader.h"

#include "user/flat.h"
#include "memory/page.h"
#include "utils/strutil.h"
#include "scheduler/scheduler.h"
#include "utils/num2ascii.h"
#include "debug/semihosting.h"

int exec_flat(const uint8_t *data, uint32_t size, const char *name)
{
    if (size < sizeof(flat_header_t)) { return -1; } // header size check
    const flat_header_t *hdr = (const flat_header_t *)data;
    if (hdr->magic != FLAT_MAGIC) { return -2; } // magic check

    uint32_t total = hdr->text_size + hdr->data_size + hdr->bss_size;
    uint32_t pages = (total + PAGE_SIZE - 1) / PAGE_SIZE;
    
    uint8_t *base = (uint8_t *)page_alloc(pages);
    if (!base) return -3;

    // copy text + data to RAM
    strncopy(base, data + sizeof(flat_header_t),
           hdr->text_size + hdr->data_size);
    strncopy(base + hdr->text_size + hdr->data_size, 0, hdr->bss_size);

    // resolve reloc
    if (hdr->reloc_count > 0) {
        const uint32_t *reloc = (const uint32_t *)(data + hdr->reloc_off);
        for (uint32_t i = 0; i < hdr->reloc_count; i++) {
            if (reloc[i] >= total) { return -4; }
            uint32_t *p = (uint32_t *)(base + reloc[i]);
            *p += (uint32_t)base;
        }
    }

    // resolve stack
    uint32_t stack_sz = hdr->stack_hint ? hdr->stack_hint : 1024;

    int pid = task_create(name, (void (*)(void))(base + hdr->entry), stack_sz / 4);
	
	int offset = 0;
	char str[64];
	offset += s2ascii("pid: ", (str + offset));
	offset += i2ascii(pid, (str + offset));
	offset += s2ascii("\n", (str + offset));
	str[offset] = '\0';
	semihost_write(str, offset);
	
	// set base and pages 
	task_ctx_t *t = task_by_pid(pid);
	t->prog_base  = (uint32_t)base;
    t->prog_pages = pages;
	
    return pid;
}