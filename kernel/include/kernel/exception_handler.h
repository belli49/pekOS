#include <stdint.h>
__attribute__((noreturn))
void exception_handler(void);
void test_exception_handler(void);
void system_timer_interrupt(void);
void page_fault_handler(uintptr_t* virtual_address, uint32_t error_code);
