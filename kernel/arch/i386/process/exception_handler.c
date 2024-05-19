#include <stdint.h>
#include <stdio.h>

#include <kernel/exception_handler.h>
#include <kernel/paging.h>
#include <kernel/apic.h>
#include <kernel/pic.h>
#include <kernel/ps2.h>


void exception_handler() {
  printf("It was an exception :(\n");
  __asm__ volatile ("cli; hlt"); // Completely hangs the computer
}


void exception_handler_code(uint32_t code) {
  printf("It was an exception :( %d\n", code);

  if (code >= 32 && code < 48) PIC_sendEOI(code - 32);
  else if (code >= 48 && code < 64) apic_send_eoi();
  return;
}



void test_exception_handler() {
  //printf("Test exception handler called\n");
  apic_send_eoi();
  return;
}



void test_kbd_interrupt(uint32_t code) {
  uint8_t scan_code;

  switch (code) {
    case 33:
      // PIC interrupt (IRQ1)
      scan_code = read_ps2_data();
      printf("scancode read from PIC interrupt %x\n", scan_code);
      PIC_sendEOI(code - 32);
      break;

    case 50:  
      // IOAPIC interrupt (redirected to ioapic IRQ1 in qemu)
      scan_code = read_ps2_data();
      printf("scancode read from IOAPIC interrupt %x\n", scan_code);
      apic_send_eoi();
      break;

    default:
      // if this case is called, interrupt stub table is wrong (calling wrong routine)
      printf("kbd interrupt called from invalid IRQ error\n");
      __asm__ volatile ("cli; hlt"); // Completely hangs the computer
      break;
  }

  apic_send_eoi();
  return;
}



void system_timer_interrupt() {
  // printf("System timer interrupt called\n");
  apic_send_eoi();
  return;
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
