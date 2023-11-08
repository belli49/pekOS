#include <stdint.h>

extern void* isr_stub_table[];

struct idt_entry_struct {
   uint16_t isr_low;         // offset bits 0..15
   uint16_t kernel_cs;       // a code segment selector in GDT or LDT
   uint8_t  reserved;        // unused, set to 0
   uint8_t  type_attributes; // gate type, dpl, and p fields
   uint16_t isr_high;        // offset bits 16..31
} __attribute__((packed));

// descriptor structure
struct idtr_struct {
  uint16_t limit;
  uint32_t base;
} __attribute__((packed));

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags);
void init_idt(void);

typedef struct idt_entry_struct idt_entry_t; 
typedef struct idtr_struct idtr_t; 
