#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/paging.h>

void paging_init() {
  _read_cr0();
  printf("ok\n");

  return;
}

