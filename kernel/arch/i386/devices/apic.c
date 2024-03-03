#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include <kernel/apic.h>
#include <kernel/cpuid.h>
#include <kernel/paging.h>


// MSR - APIC related
#define IA32_APIC_BASE_MSR 0x1B
#define IA32_APIC_BASE_MSR_BSP 0x100 // Processor is a BSP
#define IA32_APIC_BASE_MSR_ENABLE 0x800


// IOAPIC related
#define IOAPICID          0x00
#define IOAPICVER         0x01
#define IOAPICARB         0x02
#define IOAPICREDTBL(n)   (0x10 + 2 * n) // lower-32bits (add +1 for upper 32-bits)



MADT* madt;

uintptr_t local_apic1_phys_addr;
uintptr_t local_apic1_virt_addr;

bool msr_available;

// APIC
void init_apic() {
  msr_available = cpu_has_msr();
  madt = find_by_header("APIC");

  if (madt == NULL || (msr_available && !check_apic())) {
    printf("No msr available; aborting apic init\n");
    return;
  }


  // TODO: find spurrious interrupt vector register
  // careful cause it is phys location
  if (msr_available) local_apic1_phys_addr = (uintptr_t) cpu_get_apic_base();
  else local_apic1_phys_addr = (uintptr_t) madt->local_apic_address;


  // map location
  local_apic1_virt_addr = (uintptr_t) find_free_virtaddr();
  map_page((uintptr_t*) local_apic1_phys_addr, (uintptr_t*) local_apic1_virt_addr, 3);

  


  /*
  void* header_start = (void*) (madt + 1);
  MADT_record_header* first_header = (MADT_record_header*) header_start;
  printf("header: %x\n", first_header->entry_type);

  // TODO: programatically check entries in MADT
  if (first_header->entry_type == 0) {
    // first entry is proc_local_apic
    proc_local_apic* apic1 = (proc_local_apic*) (first_header + 1);
    printf("%x %x %x\n", apic1->ACPI_processor_id, apic1->APIC_id, apic1->flags);
  }
  */


  if (msr_available) enable_apic();
  // else enable APIC on some other way? Or it might always be enabled?


  /* Set the Spurious Interrupt Vector Register bit 8 to start receiving interrupts */
  write_register(0xF0, read_register(0xF0) | 0x100);


  printf("Apic enabled\n");



  // SET APIC TIMER
  // set task priority register (tpr)
  // for now use 4-level priority system
  // start with TPR at 3 (lowest priority)
  write_register(0x80, 3);

  // TODO:
  // set local apic timer's divide configuration register
  // configure local apic timer's interrupt vector and umask the timer's IRQ
  // set local apic timer's initial count

  

  // TODO: set io apic and keyboard interrupts
  // find io apic registers
  MADT_record_header* io_apic1_head = (MADT_record_header*) find_MADT_entry_by_type(1);

  if (io_apic1_head == 0) {
    printf("No io apic available\n");
    return;
  }

  // found first io apic (virt) address
  // TODO: parse other io apics
  io_apic* io_apic1 = (io_apic*) (io_apic1_head + 1);
  printf("io apic addr: %x\n", io_apic1->io_apic_address);


  // check/change IRQ indexes of ioapic
}

uintptr_t find_MADT_entry_by_type(uint32_t type) {
  uintptr_t addr = 0;
  uintptr_t start = (uintptr_t) (madt + 1);
  uint32_t len = madt->header.length - sizeof(MADT);

  for (uint32_t i = 0; i < len;) {
    MADT_record_header* cur = (MADT_record_header*) ((uintptr_t) start + i);

    if (cur->entry_type == type) {
      addr = (uintptr_t) cur;
      break;
    }

    i += cur->record_length;
  }

  return addr;
}


/** returns a 'true' value if the CPU supports APIC
 *  and if the local APIC hasn't been disabled in MSRs
 *  note that this requires CPUID to be supported.
 */
bool check_apic() {
   uint32_t eax, edx;
   cpuid(1, &eax, &edx);
   return edx & CPUID_FEAT_EDX_APIC;
}

/* Set the physical address for local APIC registers */
void cpu_set_apic_base(uintptr_t apic) {
   uint32_t edx = 0;
   uint32_t eax = (apic & 0xfffff0000) | IA32_APIC_BASE_MSR_ENABLE;
 
#ifdef __PHYSICAL_MEMORY_EXTENSION__
   edx = (apic >> 32) & 0x0f;
#endif
 
   cpu_set_msr(IA32_APIC_BASE_MSR, eax, edx);
}

/**
 * Get the physical address of the APIC registers page
 * make sure you map it to virtual memory ;)
 */
uintptr_t cpu_get_apic_base() {
   uint32_t eax, edx;
   cpu_get_msr(IA32_APIC_BASE_MSR, &eax, &edx);

#ifdef __PHYSICAL_MEMORY_EXTENSION__
   return (eax & 0xfffff000) | ((edx & 0x0f) << 32);
#else
   return (eax & 0xfffff000);
#endif
}

void enable_apic() {
    /* Section 11.4.1 of 3rd volume of Intel SDM recommends mapping the base address page as strong uncacheable for correct APIC operation. */
 
    /* Hardware enable the Local APIC if it wasn't enabled */
    cpu_set_apic_base(cpu_get_apic_base());
}



uint32_t read_register(uintptr_t register_offset) {
  return *((uint32_t*) (local_apic1_virt_addr + register_offset));
}

void write_register(uintptr_t register_offset, uint32_t value) {
  *((uint32_t*) (local_apic1_virt_addr + register_offset)) = value;
}



// IOAPIC
// 'apic_base' is the memory base address for a selected IOAPIC
void write_ioapic_register(const uintptr_t apic_base, const uint8_t offset, const uint32_t val) {
    /* tell IOREGSEL where we want to write to */
    *(volatile uint32_t*)(apic_base) = offset;
    /* write the value to IOWIN */
    *(volatile uint32_t*)(apic_base + 0x10) = val; 
}
 
uint32_t read_ioapic_register(const uintptr_t apic_base, const uint8_t offset) {
    /* tell IOREGSEL where we want to read from */
    *(volatile uint32_t*)(apic_base) = offset;
    /* return the data from IOWIN */
    return *(volatile uint32_t*)(apic_base + 0x10);
}
