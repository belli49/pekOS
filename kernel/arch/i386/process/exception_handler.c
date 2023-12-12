#include <stdint.h>
#include <stdio.h>

#include <kernel/exception_handler.h>
#include <kernel/memory_management.h>

void exception_handler() {
  __asm__ volatile ("cli; hlt"); // Completely hangs the computer
}

void page_fault_handler(uintptr_t* virtual_address, uint32_t error_code) {
  // fix error code and general interrupt calling
  // fix caller
  // uintptr_t* virtual_address = (uintptr_t*) _read_cr2();

  printf("\nException call to page fault with error code %x\n", error_code);
  printf("Virtual address called: %x\n", (uintptr_t) virtual_address);

  if (error_code & 1) {
    // page protection violation - kill process?
    printf("Protection violation fault - halting kernel\n");
    exception_handler();
    return;
  } 


  printf("Page not present fault\n");
  if (error_code & 4) {
    // user mode - kill process?
    printf("User mode page fault - halting kernel\n");
    exception_handler();
    return;
  } 


  uintptr_t* physaddr;
  if (((uintptr_t) virtual_address >> 22) == 0x3FF) {
    printf("PT not present in PD\n");
    physaddr = allocate_pt();
  } else {
    physaddr = find_free_physaddr();
    printf("Mapping address ");
    // find physical address
    printf("to phys: %x\n", (uintptr_t) physaddr);
  }

  map_page(physaddr, virtual_address, 3);

  printf("Page successfully mapped - resuming execution\n\n");
}
