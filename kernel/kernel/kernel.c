#include <stdint.h>
#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/paging.h>
#include <kernel/serial.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/memory_test.h>

void kernel_main(void) {	
  terminal_initialize();
  init_serial();
  init_gdt();
  paging_init();
  init_idt();

  printf("Hello, welcome to pekOS!\n");


  // TESTS
  printf("TESTS\n");

  // print_at_address((uintptr_t*) 0xC0000000);

  // gdt null segment
  print_at_address((uintptr_t*) 0xC0108020);
  // print_at_address((uintptr_t*) 0xC0108024);
  print_at_address((uintptr_t*) 0xC0108028);
  print_at_address((uintptr_t*) 0xC010802C);
  print_at_address((uintptr_t*) 0xC0108030);
  print_at_address((uintptr_t*) 0xC0108038);
  print_at_address((uintptr_t*) 0xC0108040);

  // tss segment
  print_at_address((uintptr_t*) 0xC0108048);
  print_at_address((uintptr_t*) 0xC010804C);

  print_at_address((uintptr_t*) 0xC0108050);
  print_at_address((uintptr_t*) 0xC0108058);
  print_at_address((uintptr_t*) 0xC0108060);
  // print_at_address((uintptr_t*) 0xC0108068);

  print_at_address((uintptr_t*) 0x80000000);
}
