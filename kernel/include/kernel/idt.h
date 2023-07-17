#include <stdint.h>

struct idt_entry_struct {
   uint16_t isr_low;        // offset bits 0..15
   uint16_t kernel_cs;        // a code segment selector in GDT or LDT
   uint8_t  reserved;            // unused, set to 0
   uint8_t  type_attributes; // gate type, dpl, and p fields
   uint16_t isr_high;        // offset bits 16..31
} __attribute__((packed));

// descriptor structure
struct idtr_struct {
  uint16_t limit;
  uint32_t base;
} __attribute__((packed));

typedef idt_entry_struct idt_entry_t; 
typedef idtr_struct idtr_t; 
