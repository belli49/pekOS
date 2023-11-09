#include <stdint.h>
#include <stdio.h>

#include <kernel/memory_test.h>

uint32_t read_at_address(uintptr_t* addr) {
  return (uint32_t) *addr;
}

void print_at_address(uintptr_t* addr) {
  printf("Testing read at address %x: ", addr);
  printf("%x\n", read_at_address(addr));
}
