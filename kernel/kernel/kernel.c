#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/paging.h>
#include <kernel/serial.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>

void kernel_main(void) {	
  terminal_initialize();
  init_serial();
  init_gdt();
  paging_init();
  init_idt();

  printf("Hello, welcome to pekOS!\n");
}
