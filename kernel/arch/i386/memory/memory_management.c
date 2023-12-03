#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/memory_management.h>
#include <kernel/memory_access.h>

#define KERNEL_MAP_START_LOCATION 0xC0000000
#define KERNEL_PD_VIRTUAL_LOCATION 0xFFFFF000

uintptr_t* kernel_PD = (uintptr_t*) KERNEL_PD_VIRTUAL_LOCATION;

void init_mm() {
/*
  // Hacky way to get page directory location?
  // Should not rely on this although I think it
  // should work normally
  // DONE: Instead self-map during boot sequence in asm
  kernel_PD = (uintptr_t*) (_read_cr3() + KERNEL_MAP_START_LOCATION);
  printf("CR3 val: %x\n", kernel_PD);

  printf("Last entry in PD: %x\n", kernel_PD[1023]);

  // Map last PDE to itself:
  // NOTE: kernel_PD is 4KiB aligned, so no need to bitshift
  kernel_PD[1023] = (((uint32_t) kernel_PD) | 0x3);
*/

  // PD is self mapped to last PTE of PT at last PDE
  // It is also mapped at 0xC0000000 + cr3 (PD physical location)
  // We should access it through 0xFFFFF000 because it seems
  // less hacky and more in line with what others devs do (?)

  return;
}

void* get_physaddr(void* virtualaddr) {
    // first 10 bits of virtual address
    uintptr_t pdindex = (uintptr_t) virtualaddr >> 22;
    // next 10 bits of virtual address
    uintptr_t ptindex = (uintptr_t) virtualaddr >> 12 & 0x03FF;
    // last 12 bits of virtual address refer to byte location inside 4KiB chunk

    uintptr_t* pd = (uintptr_t*) 0xFFFFF000;
    // Here you need to check whether the PD entry is present.

    uintptr_t* pt = ((uintptr_t*) 0xFFC00000) + (0x400 * pdindex);
    // Here you need to check whether the PT entry is present.

    return (void*) ((pt[ptindex] & ~0xFFF) + ((uintptr_t) virtualaddr & 0xFFF));
}
