#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <kernel/rsdt.h>
#include <kernel/memory_access.h>

#define KERNEL_MAP_START_LOCATION 0xC0000000
#define ADDR_NUMBER_OF_KIB_BEFORE_EBDA (KERNEL_MAP_START_LOCATION + 0x413)


bool ACPI_version;
RSDP* rsdp;
RSDT* rsdt;
XSDT* xsdt;

void init_rsdt() {
  rsdp = find_RSDP();
  
  if (ACPI_version) {
    // ACPI version 2.0
    xsdt = (XSDT*) find_RSDT();
  } else {
    // ACPI version 1.0
    rsdt = (RSDT*) find_RSDT();

    // TODO: add basic memory mapping functionality?
    // We'll have to memory map quite a lot of stuff
    // before we get to the point of working with 
    // process, so adding basic memory mapping functionality
    // seems reasonable (apparently a there's a lot of MMIO
    // mapped close to 0xC0000000 physical)
    // else we'd have to keep relying on assembly
    printf("%x\n", (uintptr_t) rsdt);
    // printf("%s\n", rsdt->h.Signature);
  }
}

void* find_RSDT() {
  if (!rsdp->Revision) {
    // ACPI 1.0 -> use RSDT
    return (void*) rsdp->RsdtAddress;
  }

  // ACPI 2.0 -> use XSDT
  return (void*) ((uint32_t) ((XSDP*) rsdp)->XsdtAddress);
}

RSDP* find_RSDP() {
  // Look for RSDP at first 1KiB of EBDA
  void* EBDA_location = (void*) (
    (read_word_at_address((uintptr_t*) ADDR_NUMBER_OF_KIB_BEFORE_EBDA) << 10)
    + KERNEL_MAP_START_LOCATION
  );

  printf("Looking for RSDP at EBDA; starting at %x\n", EBDA_location);

  for (uint32_t d = 0; d < 0x1000; d += 0x10) {
    void* cur_location = (void*) ((uint32_t) EBDA_location + d);

    if (!strncmp((char*) cur_location, "RSD PTR ", 8)) {
      printf("Found possible RSDP at %x\n", (uintptr_t) cur_location);

      if (!do_checksum_RSDP(cur_location)) {
        printf("Failed checksum; continuing search\n");
      } else {
        printf("Checksum successful: Found RSDP at %x\n", (uintptr_t) cur_location);
        return (RSDP*) cur_location;
      }
    }
  }


  // Look for RSDP from 0xE0000 to 0xFFFFF physical
  printf("Looking for RSDP after 0xE0000 starting at %x\n", 0xC00E0000);
    
  for (uint32_t d = 0x000E0000; d < 0x000FFFFF; d += 0x10) {
    void* cur_location = (void*) (KERNEL_MAP_START_LOCATION + d);

    if (!strncmp((char*) cur_location, "RSD PTR ", 8)) {
      printf("Found possible RSDP at %x\n", (uintptr_t) cur_location);

      if (!do_checksum_RSDP(cur_location)) {
        printf("Failed checksum; continuing search\n");
      } else {
        printf("Checksum successful: found RSDP at %x\n", (uintptr_t) cur_location);
        return (RSDP*) cur_location;
      }
    }
  }

  printf("Failed to find RSDP\n");
  return NULL;
}


bool do_checksum_ACPISDT(ACPISDTHeader *tableHeader) {
  unsigned char sum = 0;

  for (uint32_t i = 0; i < tableHeader->Length; i++) {
    sum += ((char *) tableHeader)[i];
  }

  return sum == 0;
}

bool do_checksum_RSDP(RSDP* rsdp) {
  unsigned char sum = 0;
  ACPI_version = rsdp->Revision;

  if (ACPI_version) printf("RSDP using ACPI version 2\n");
  else printf("RSDP using ACPI version 1\n");

  // if version 1.0 size is 8 + 1 + 6 + 1 + 4 = 20 bytes (size of RSDP)
  // if version 2.0 size is 20 + 4 + 8 + 1 + 3 = 36 bytes (size of XSDP)
  for (uint32_t i = 0; i < (ACPI_version ? 36 : 20); i++) {
    sum += ((char*) rsdp)[i];
  }

  return sum == 0;
}

void *find_entry_in_RSDT(char* signature) {
  uint32_t entries = (rsdt->h.Length - sizeof(rsdt->h)) / 4;

  for (uint32_t i = 0; i < entries; i++) {
    ACPISDTHeader *h = (ACPISDTHeader *) rsdt->PointerToOtherSDT[i];
    if (!strncmp(h->Signature, signature, 4))
      return (void *) h;
  }

  // No FACP found
  return NULL;
}
