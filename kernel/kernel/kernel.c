#include <stdint.h>
#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/memory_management.h>
#include <kernel/serial.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/memory_access.h>
#include <kernel/paging.h>
#include <kernel/fadt.h>
#include <kernel/ps2.h>
#include <kernel/pic.h>
#include <kernel/apic.h>

#include <kernel/multiboot.h>


#ifndef ACPISDT_HEADER_DEFINED
#define ACPISDT_HEADER_DEFINED

#include <kernel/rsdt.h>

#endif


void tests();

void kernel_main(multiboot_info_t* mbd, uint32_t magic) {	
  terminal_initialize();
  init_serial();
  init_pic();
  init_gdt();
  init_idt();
  init_page_allocation();
  // init_mm();
  init_rsdt();
  init_apic();
  init_ps2();

  printf("Hello, welcome to pekOS!\n");
  while (1);

  // tests();
}

void tests() {
  // page allocation tests
  uintptr_t* ptr = (uintptr_t*) 0x30300000;
  *ptr = 2;
  // printf("%x\n", *((uintptr_t*) 0x30300000));
  // printf("%x\n", *((uintptr_t*) 0xC0300000));

}
