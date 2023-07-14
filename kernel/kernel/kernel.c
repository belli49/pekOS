#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/paging.h>
#include <kernel/serial.h>
#include <kernel/gdt.h>

void kernel_main(void) {	
  terminal_initialize();
  paging_init();
  init_serial();
  init_gdt();

  printf("Hello, welcome to pekOS!\n");
}
