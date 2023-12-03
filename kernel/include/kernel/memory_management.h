#include <stdint.h>

extern int _read_cr0(void);
extern int _read_cr3(void);
extern void _write_cr0(uint32_t val);
extern void _write_cr3(uint32_t val);

void init_mm(void);
void* get_physaddr(void* virtualaddr);
