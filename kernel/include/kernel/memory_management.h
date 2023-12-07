#include <stdint.h>

extern int _read_cr0(void);
extern int _read_cr2(void);
extern int _read_cr3(void);
extern void _write_cr0(uint32_t val);
extern void _write_cr2(uint32_t val);
extern void _write_cr3(uint32_t val);

void init_mm(void);
void map_virtual_memory(uintptr_t* virtual_mm);
void* get_pe_value(uintptr_t virtualaddr);
void* get_physaddr(uintptr_t virtualaddr);
void map_page(uintptr_t* physaddr, uintptr_t* virtualaddr, uintptr_t flags);
uintptr_t* find_free_physaddr(void);
