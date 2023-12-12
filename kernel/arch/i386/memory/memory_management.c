#include <stdio.h>
#include <stdint.h>

#include <kernel/tty.h>
#include <kernel/memory_management.h>
#include <kernel/memory_access.h>

#define KERNEL_MAP_START_LOCATION 0xC0000000
#define KERNEL_PD_VIRTUAL_LOCATION 0xFFFFF000

uintptr_t* kernel_PD = (uintptr_t*) KERNEL_PD_VIRTUAL_LOCATION;
// number of pd entries * number of pt entries / sizeof(uint32_t)
uintptr_t virtual_mmap[(uint32_t) (1024 * 32)]; 
uintptr_t cur_virtaddr_idx = 768 * 32;
uintptr_t cur_physaddr = 0x10000000;

// TODO: put physaddr back to 0x10000000
// and make a function to find (0x1000 aligned) 4KiB
// region to put a PT in

void init_mm() {
/*
  // PD is self mapped to last PTE of PT at last PDE
  // It is also mapped at 0xC0000000 + cr3 (PD physical location)
  // We should access it through 0xFFFFF000 because it seems
  // less hacky and more in line with what others devs do (?)

  printf("Physical address of PD; next 2 vals should be the same:\n");
  printf("%x\n", (uintptr_t) get_physaddr(0xFFFFF000));
  printf("%x\n", (uintptr_t) get_physaddr(0xC0000000 + (uintptr_t) _read_cr3()));
*/

  map_virtual_memory(virtual_mmap);

  return;
}

void map_virtual_memory(uintptr_t* virtual_mmap) {
  printf("Mapping virtual memory\n");
  for (uint32_t pdindex = 0; pdindex < 1024; pdindex++) {
    // each pd entry is represented as 32 bytes
    if (kernel_PD[pdindex] & 1) {
      // page is present

      for (uint32_t group = 0; group < 32; group++) {
        uint32_t cnt = 0;
        uintptr_t* pt = (uintptr_t*) (0xFFC00000 + (pdindex << 12));

        for (uint32_t entry = 0; entry < 32; entry++) {
          cnt <<= 1;
          cnt |= (pt[group * 32 + entry] & 1);
        }

        virtual_mmap[pdindex * 32 + group] = cnt;
      }
    } else {
      // page is not present
      // set all the page table as 0

      for (uint32_t group = 0; group < 32; group++) {
        virtual_mmap[pdindex * 32 + group] = 0;
      }
    }
  }

  for (uint32_t i = 0; i < 1024 * 32; i++) {
    if (virtual_mmap[i] != 0) {
      uintptr_t loc = i << 17;
      printf("memory at %x - %x\n", loc, virtual_mmap[i]);
    }
  }
}

void* get_pe_value(uintptr_t virtualaddr) {
    // first 10 bits of virtual address
    uintptr_t pdindex = virtualaddr >> 22;
    // next 10 bits of virtual address
    uintptr_t ptindex = virtualaddr >> 12 & 0x03FF;
    // last 12 bits of virtual address refer to byte location inside 4KiB chunk

    uintptr_t* pd = (uintptr_t*) 0xFFFFF000;
    if (!(pd[pdindex] & 1)) {
      printf("PD entry not present\n");
      return NULL;
    }

    // 0xFFC00000 will by at the last PDE (where we self-mapped the PD)
    // thus, each page table's virtual address will be:
    // 0xFFC00000 + idx (of page table in the PD) * 0x1000
    uintptr_t* pt = (uintptr_t*) (0xFFC00000 + (pdindex << 12));

    if (!(pt[ptindex] & 1)) {
      printf("PT entry not present\n");
    }

    return (void*) (pt[ptindex]);
}

void* get_physaddr(uintptr_t virtualaddr) {
    // first 10 bits of virtual address
    uintptr_t pdindex = virtualaddr >> 22;
    // next 10 bits of virtual address
    uintptr_t ptindex = virtualaddr >> 12 & 0x03FF;
    // last 12 bits of virtual address refer to byte location inside 4KiB chunk

    uintptr_t* pd = (uintptr_t*) 0xFFFFF000;
    // Here you need to check whether the PD entry is present.
    if (!(pd[pdindex] & 1)) {
      printf("PD entry not present\n");
      return NULL;
    }

    // 0xFFC00000 will by at the last PDE (where we self-mapped the PD)
    // thus, each page table's virtual address will be:
    // 0xFFC00000 + idx (of page table in the PD) * 0x1000
    uintptr_t* pt = (uintptr_t*) (0xFFC00000 + (pdindex << 12));
    // Here you need to check whether the PT entry is present.
    if (!(pt[ptindex] & 1)) {
      printf("PT entry not present\n");
      return NULL;
    }

    return (void*) ((pt[ptindex] & ~0xFFF) + ((uintptr_t) virtualaddr & 0xFFF));
}

void map_page(uintptr_t* physaddr, uintptr_t* virtualaddr, uintptr_t flags) {
    // Make sure that both addresses are page-aligned.

    uintptr_t pdindex = (uintptr_t) virtualaddr >> 22;
    uintptr_t ptindex = ((uintptr_t) virtualaddr >> 12) & 0x03FF;

    uintptr_t* pd = (uintptr_t*) 0xFFFFF000;
    printf("pde value: %x\n", pd[pdindex]);
    if (!(pd[pdindex] & 1)) {
      uintptr_t* new_page_phys_addr = find_free_physaddr();
      printf("PD entry not present - creating new PT at phys: %x\n",
          (uintptr_t) new_page_phys_addr);

      pd[pdindex] = ((uintptr_t) new_page_phys_addr | 3);
      printf("%x\n", pd[pdindex]);

      _flush_TLB();
    }

    uintptr_t* pt = (uintptr_t*) (0xFFC00000 + (pdindex << 12));
    // Here you need to check whether the PT entry is present.
    // When it is, then there is already a mapping present. What do you do now?
    printf("pt phys: %x\n", pd[pdindex] & ~0xFFF);
    printf("pt virt: %x\n", (uintptr_t) pt);
    printf("pte value: %x\n", pt[ptindex]);
    if (pt[ptindex] & 1) {
      printf("PTE already present at %x; aborting", ((uintptr_t) virtualaddr & (~0xFFF)));
      return;
    }

    pt[ptindex] = ((uintptr_t) physaddr | (flags & 0xFFF)); // Present
    printf("new pte value: %x\n", pt[ptindex]);

    _flush_TLB();
    printf("pte value after flushing TLB: %x\n", pt[ptindex]);
}

uintptr_t* find_free_virtaddr() {
  // gets 4KiB's
  uintptr_t ptr = cur_virtaddr_idx;

  for(; ptr < 1028 * 32; ptr++) {
    for (uint32_t i = 0; i < 32; i++) {
      if (((virtual_mmap[ptr] >> (31 - i)) & 1) != 1) {
        virtual_mmap[ptr] |= (1 << (31 - i));
        cur_virtaddr_idx = ptr;

        return (uintptr_t*) ((ptr * 32 + i) << 12);
      }
    }
  }

  return NULL;
}

uintptr_t* find_free_physaddr() {
  uintptr_t* temp = (uintptr_t*) cur_physaddr;
  cur_physaddr += 0x1000;
  return temp;
}
