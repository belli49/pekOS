#include <stdint.h>
__attribute__((noreturn))
void exception_handler(void);
void page_fault_handler(uintptr_t* virtual_address, uint32_t error_code);
