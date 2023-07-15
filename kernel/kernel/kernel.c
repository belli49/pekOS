#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/paging.h>
#include <kernel/serial.h>
#include <kernel/gdt.h>

void kernel_main(void) {	
  terminal_initialize();
  init_serial();
  init_gdt();
  paging_init();

  printf("Hello, welcome to pekOS!\n");
}
