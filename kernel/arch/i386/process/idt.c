#define IDT_MAX_DESCRIPTORS 256
#define GDT_OFFSET_KERNEL_CODE 0x08 // this value can be whatever offset your kernel code selector is in your GDTTT
                                    // here, it is the second entry on the GDT => 1 (idx) * 8 (size of GDT entry)

#include <stdint.h>

#include <kernel/idt.h>
#include <kernel/serial.h>

// 8-byte aligned for performance
__attribute__((aligned(0x10)))
static idt_entry_t idt[IDT_MAX_DESCRIPTORS];
static idtr_t idtr;

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags) {
  idt_entry_t* descriptor = &idt[vector];

  descriptor->isr_low         = (uint32_t)isr & 0xFFFF;
  descriptor->kernel_cs       = GDT_OFFSET_KERNEL_CODE; 
  descriptor->type_attributes = flags;
  descriptor->isr_high        = (uint32_t)isr >> 16;
  descriptor->reserved        = 0;
}

void init_idt() {
  idtr.base = (uintptr_t) &idt[0];
  idtr.limit = (uint16_t) sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;

  for (uint16_t vector = 0; vector < 256; vector++) {
    idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
    // vectors[vector] = true; // why does this exist?
  }

  __asm__ volatile ("lidt %0" : : "m"(idtr)); // load the new IDT
  __asm__ volatile ("sti"); // set the interrupt flag

  write_string_serial("Flushed IDT\n");
}
