#include <stdint.h>
#include <string.h>

#include <kernel/idt.h>

// 8-byte aligned for performance
__attribute__((aligned(0x10)))
idt_entry_t idt[256];

void init_idt() {
  return;
}
