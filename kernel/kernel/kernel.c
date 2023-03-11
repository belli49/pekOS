#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/paging.h>

void kernel_main(void) {
	terminal_initialize();
  paging_init();
	printf("Hello, welcome to pekOS!\n");
}
