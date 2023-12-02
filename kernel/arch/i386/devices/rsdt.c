#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <kernel/rsdt.h>
#include <kernel/memory_access.h>

#define KERNEL_MAP_START_LOCATION 0xC0000000
#define ADDR_NUMBER_OF_KIB_BEFORE_EBDA (KERNEL_MAP_START_LOCATION + 0x413)


uintptr_t find_RSDT() {
  // Look for RSDP at first 1KiB of EBDA
  void* EBDA_location = (void*) (
    (read_word_at_address((uintptr_t*) ADDR_NUMBER_OF_KIB_BEFORE_EBDA) << 10)
    + KERNEL_MAP_START_LOCATION
  );

  printf("Looking for RSDP at EBDA; starting at %x\n", EBDA_location);

  for (uint32_t d = 0; d < 0x1000; d += 0x10) {
    void* cur_location = (void*) ((uint32_t) EBDA_location + d);

    if (!strncmp((char*) cur_location, "RSD PTR ", 8)) {
      printf("Found RSDP at %x\n", cur_location);
      return (uintptr_t) cur_location;
    }
  }


  // Look for RSDP from 0xE0000 to 0xFFFFF physical
  printf("Looking for RSDP after 0xE0000 starting at %x\n", 0xC00E0000);
    
  for (uint32_t d = 0xC00E0000; d < 0xC00FFFFF; d += 0x10) {
    void* cur_location = (void*) d;

    if (!strncmp((char*) cur_location, "RSD PTR ", 8)) {\
      printf("Found RSDP at %x\n", (uintptr_t) cur_location);
      return (uintptr_t) cur_location;
    }
  }

  printf("Failed to find RSDP\n");
  return 0;
}


bool doChecksum(ACPISDTHeader *tableHeader) {
  unsigned char sum = 0;

  for (uint32_t i = 0; i < tableHeader->Length; i++) {
    sum += ((char *) tableHeader)[i];
  }

  return sum == 0;
}

void *findFACP(void *RootSDT) {
  RSDT *rsdt = (RSDT *) RootSDT;
  uint32_t entries = (rsdt->h.Length - sizeof(rsdt->h)) / 4;

  for (uint32_t i = 0; i < entries; i++) {
    ACPISDTHeader *h = (ACPISDTHeader *) rsdt->PointerToOtherSDT[i];
    if (!strncmp(h->Signature, "FACP", 4))
      return (void *) h;
  }

  // No FACP found
  return NULL;
}
