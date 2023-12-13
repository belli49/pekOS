#include <stdint.h>

extern void _flush_TLB(void);
extern uint32_t _read_cr0(void);
extern uint32_t _read_cr2(void);
extern uint32_t _read_cr3(void);
extern void _write_cr0(uint32_t val);
extern void _write_cr2(uint32_t val);
extern void _write_cr3(uint32_t val);
