#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/paging.h>
#include <kernel/serial.h>

void kernel_main(void) {
	terminal_initialize();
  paging_init();
  init_serial();

  printf("Hello, welcome to pekOS!\n");
}
