#include <stdio.h>

#include <kernel/exception_handler.h>

void exception_handler() {
  __asm__ volatile ("cli; hlt"); // Completely hangs the computer
}

void page_fault_handler() {
  printf("Exception call to page fault\n");
}
