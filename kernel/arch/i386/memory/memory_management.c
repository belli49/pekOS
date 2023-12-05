#include <stdio.h>
#include <stdint.h>

#include <kernel/tty.h>
#include <kernel/memory_management.h>
#include <kernel/memory_access.h>

#define KERNEL_MAP_START_LOCATION 0xC0000000
#define KERNEL_PD_VIRTUAL_LOCATION 0xFFFFF000

uintptr_t* kernel_PD = (uintptr_t*) KERNEL_PD_VIRTUAL_LOCATION;
// each bit is a page -> each byte is 1e8 pages
// each page table is last 10 bits
// each page dir entry is first 10 bits
// -> we need 1e20 bits for the entire memory space
// 1e20 / 1e5 = 1e15 32-bit int entries
uintptr_t virtual_mm[(uint32_t) (1 << 15)]; 

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

/*
  printf("Physical address of PD; next 2 vals should be the same:\n");
  printf("%x\n", (uintptr_t) get_physaddr(0xFFFFF000));
  printf("%x\n", (uintptr_t) get_physaddr(0xC0000000 + (uintptr_t) _read_cr3()));
*/

  // Start by finding which pages are free/used and store the
  // info into a custom structure.

  return;
}

void* get_physaddr(uintptr_t virtualaddr) {
    // first 10 bits of virtual address
    uintptr_t pdindex = virtualaddr >> 22;
    // next 10 bits of virtual address
    uintptr_t ptindex = virtualaddr >> 12 & 0x03FF;
    // last 12 bits of virtual address refer to byte location inside 4KiB chunk

    uintptr_t* pd = (uintptr_t*) 0xFFFFF000;
    // Here you need to check whether the PD entry is present.

    // 0xFFC00000 will by at the last PDE (where we self-mapped the PD)
    // thus, each page table's virtual address will be:
    // 0xFFC00000 + idx (of page table in the PD) * 0x1000
    // (here, we are jumping 1024 PTE per index instead which achieves
    // the same result (using pointer instead of value))
    uintptr_t* pt = ((uintptr_t*) 0xFFC00000) + (pdindex << 10);
    // Here you need to check whether the PT entry is present.

    return (void*) ((pt[ptindex] & ~0xFFF) + ((uintptr_t) virtualaddr & 0xFFF));
}

void map_page(uintptr_t* physaddr, uintptr_t* virtualaddr, uintptr_t flags) {
    // Make sure that both addresses are page-aligned.

    uintptr_t pdindex = (uintptr_t) virtualaddr >> 22;
    uintptr_t ptindex = (uintptr_t) virtualaddr >> 12 & 0x03FF;

    uintptr_t* pd = (uintptr_t*) 0xFFFFF000;
    // Here you need to check whether the PD entry is present.
    // When it is not present, you need to create a new empty PT and
    // adjust the PDE accordingly.

    uintptr_t* pt = ((uintptr_t*) 0xFFC00000) + (0x400 * pdindex);
    // Here you need to check whether the PT entry is present.
    // When it is, then there is already a mapping present. What do you do now?

    pt[ptindex] = ((uintptr_t) physaddr | (flags & 0xFFF) | 0x01); // Present

    // Now you need to flush the entry in the TLB
    // or you might not notice the change.
}
