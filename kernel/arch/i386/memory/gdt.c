#include <stdint.h>
#include <string.h>

#include <kernel/serial.h>
#include <kernel/gdt.h>

tss_entry_t tss_entry;
gdt_entry gdt[6]; // one null segment, two ring 0 segments, two ring 3 segments, TSS segment
size_t gdt_size = sizeof(gdt);

// Set and add TSS entry to GDT
void write_tss(gdt_entry *g, gdt_entry *kernel_stack_segment) {
	// Compute the base and limit of the TSS for use in the GDT entry.
	uint32_t base = (uint32_t) &tss_entry;
	uint32_t limit = sizeof tss_entry;

	// Add a TSS descriptor to the GDT.
	g->limit_low = limit;
	g->base_low = base;
	g->accessed = 1; // With a system entry (`code_data_segment` = 0), 1 indicates TSS and 0 indicates LDT
	g->read_write = 0; // For a TSS, indicates busy (1) or not busy (0).
	g->conforming_expand_down = 0; // always 0 for TSS
	g->code = 1; // For a TSS, 1 indicates 32-bit (1) or 16-bit (0).
	g->code_data_segment=0; // indicates TSS/LDT (see also `accessed`)
	g->DPL = 0; // ring 0, see the comments below
	g->present = 1;
	g->limit_high = (limit & (0xf << 16)) >> 16; // isolate top nibble
	g->available = 0; // 0 for a TSS
	g->long_mode = 0;
	g->big = 0; // should leave zero according to manuals.
	g->gran = 0; // limit is in bytes, not pages
	g->base_high = (base & (0xff << 24)) >> 24; //isolate top byte

	// Ensure the TSS is initially zero'd.
	memset(&tss_entry, 0, sizeof tss_entry);

	tss_entry.ss0  = (uint32_t) (kernel_stack_segment);  // Set the kernel stack segment (kernel data)
	tss_entry.esp0 = (uint32_t) (&stack_top); // Set the kernel stack pointer.
	//note that CS is loaded from the IDT entry and should be the regular kernel code segment
}

void set_kernel_stack(uint32_t stack) { // Used when an interrupt occurs
	tss_entry.esp0 = stack;
}

void init_gdt() {

  // ring 0 segments
  gdt_entry *ring0_code = &gdt[1];
  gdt_entry *ring0_data = &gdt[2];

  ring0_code->limit_low = 0xFFFF;
  ring0_code->base_low = 0;
  // set access byte to 0x9A
  ring0_code->accessed = 0;
  ring0_code->read_write = 1; // since this is a code segment, specifies that the segment is readable
  ring0_code->conforming_expand_down = 0;
  ring0_code->code = 1;
  ring0_code->code_data_segment = 1;
  ring0_code->DPL = 0; // ring 0 (privilege level)
  ring0_code->present = 1;
  ring0_code->limit_high = 0xF;
  ring0_code->available = 1;
  ring0_code->long_mode = 0;
  ring0_code->big = 1; // it's 32 bits
  ring0_code->gran = 1; // 4KB page addressing
  ring0_code->base_high = 0;

  // set ring0_data with code set to 0
  *ring0_data = *ring0_code;
  ring0_data->code = 0;


  // ring 3 segments
  gdt_entry *ring3_code = &gdt[3];
  gdt_entry *ring3_data = &gdt[4];

  // ring3_code is the same as ring0_code with DPL = 0x11 (privilege level)
  *ring3_code = *ring0_code;
  ring3_code->DPL = 3;

  // ring3_data is the same as ring3_code with code set to 0
  *ring3_data = *ring3_code; // contents are similar so save time by copying
  ring3_data->code = 0; // not code but data


  flush_gdt();
  write_string_serial("Flushed GDT\n");

  write_tss(&gdt[5], &gdt[2]); // TSS segment will be the fifth (sixth counting null segment)
  flush_tss();
  write_string_serial("Flushed TSS\n");


  return;
}
