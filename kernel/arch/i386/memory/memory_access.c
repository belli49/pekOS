#include <stdint.h>
#include <stdio.h>

#include <kernel/memory_access.h>

uint8_t read_byte_at_address(uintptr_t* addr) {
  return (uint16_t) *addr;
}

uint16_t read_word_at_address(uintptr_t* addr) {
  return (uint16_t) *addr;
}

uint32_t read_at_address(uintptr_t* addr) {
  return (uint32_t) *addr;
}

void print_byte_at_address(uintptr_t* addr) {
  printf("Byte at address %x: ", addr);
  printf("%x\n", read_byte_at_address(addr));
}

void print_word_at_address(uintptr_t* addr) {
  printf("Word at address %x: ", addr);
  printf("%x\n", read_word_at_address(addr));
}

void print_at_address(uintptr_t* addr) {
  printf("int at address %x: ", addr);
  printf("%x\n", read_at_address(addr));
}
