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

  uintptr_t* ptr = (uintptr_t*) 0xC0300000;
  *ptr = 0;
  printf("%x\n", *((uintptr_t*) 0x30300000));
  // printf("%x\n", *((uintptr_t*) 0xC0300000));



}
