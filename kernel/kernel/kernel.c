#include <stdint.h>
#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/memory_management.h>
#include <kernel/serial.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/memory_access.h>
#include <kernel/multiboot.h>

#include <kernel/rsdt.h>

void kernel_main(multiboot_info_t* mbd, uint32_t magic) {	
  terminal_initialize();
  init_serial();
  init_gdt();
  init_idt();
  init_mm();
  init_rsdt();

  printf("Hello, welcome to pekOS!\n");

  // map_page((uintptr_t*) 0x30000000, (uintptr_t*) 0xC0300000, 1);
  uintptr_t* ptr = (uintptr_t*) 0xC0300000;
  *ptr = 0;
  printf("%x\n", *((uintptr_t*) 0xC0300000));

/*
  // TESTS
  printf("TESTS\n");

  print_at_address((uintptr_t*) 0xC0000000);
  print_word_at_address((uintptr_t*) 0xC000040E);
  print_word_at_address((uintptr_t*) 0xC0000413);
  
  // address of EBDA (might be incorrect depending on the system)
  uint32_t address_EBDA = read_word_at_address((uintptr_t*) 0xC000040E) << 4;
  // KiB of available RAM
  uint32_t BIOS_area_available_RAM = read_word_at_address((uintptr_t*) 0xC0000413);

  if ((address_EBDA) == ((BIOS_area_available_RAM << 10))) {
    printf("EBDA address was correct\n");
  } else {
    printf("EBDA address was wrong\n");
  }

  // gdt null segment
  print_at_address((uintptr_t*) 0xC0108020);
  // print_at_address((uintptr_t*) 0xC0108024);

  // address mapped for testing in boot.S
  print_at_address((uintptr_t*) 0x80000000);
*/
}
