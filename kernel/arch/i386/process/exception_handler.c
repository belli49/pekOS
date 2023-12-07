#include <stdint.h>
#include <stdio.h>

#include <kernel/exception_handler.h>
#include <kernel/memory_management.h>

void exception_handler() {
  __asm__ volatile ("cli; hlt"); // Completely hangs the computer
}

void page_fault_handler(uint32_t error_code) {
  // fix error code and general interrupt calling
  // fix caller

  uintptr_t* virtual_address = (uintptr_t*) _read_cr2();
  printf("Exception call to page fault with error code %x\n", error_code);
  printf("Virtual address called: %x\n", (uintptr_t) virtual_address);

  if (error_code & 1) {
    // page protection violation - kill process?
    printf("Protection violation fault - halting kernel\n");
    return;
  } 


  printf("Page not present fault\n");
  if (error_code & 4) {
    // user mode - kill process?
    printf("User mode page fault - hating kernel\n");
    return;
  } 


  // kernel - allocate from 0xC0000000
  printf("Allocating kernel page\n");

  // find physical address
  uintptr_t* physaddr = find_free_physaddr();
  printf("at: %x\n", (uintptr_t) physaddr);

  // map page
  map_page(physaddr, virtual_address, 3);

  _write_cr3(_read_cr3());
  printf("Page successfully mapped - resuming execution\n");
}
