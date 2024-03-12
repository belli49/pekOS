#include <stdbool.h>
#include <stdint.h>

#ifndef ACPISDT_HEADER_DEFINED
#define ACPISDT_HEADER_DEFINED

#include <kernel/rsdt.h>

#endif

uintptr_t find_MADT_entry_by_type(uint32_t type);
void init_apic(void);
bool check_apic(void);
void cpu_set_apic_base(uintptr_t apic);
uintptr_t cpu_get_apic_base(void);
void enable_apic(void);
void apic_start_timer(void);
void apic_send_eoi(void);

// apic
uint32_t read_register(uintptr_t register_offset);
void write_register(uintptr_t register_offset, uint32_t value);

// ioapic
uint32_t read_ioapic_register(const uintptr_t apic_base, const uint8_t offset);
void write_ioapic_register(const uintptr_t apic_base, const uint8_t offset, const uint32_t val);



// MADT
/* after the MADT struct there are a number of
 * variable-length records
 */
typedef struct MADT_struct {
  ACPISDTHeader header;
  uint32_t local_apic_address;
  uint32_t flags;
} MADT;

typedef struct MADT_record_header_struct {
  uint8_t entry_type;
  uint8_t record_length;
} MADT_record_header;

typedef struct proc_local_apic_struct {
  uint8_t entry_type;
  uint8_t record_length;
  uint8_t ACPI_processor_id;
  uint8_t APIC_id;
  uint32_t flags;
} proc_local_apic;

typedef struct io_apic_struct {
  uint8_t entry_type;
  uint8_t record_length;
  uint8_t io_apic_id;
  uint8_t reserved_0;
  uint32_t io_apic_address;
  uint32_t global_system_interrupt_base;
} io_apic;


typedef struct io_apic_interrupt_source_overdrive_struct {
  uint8_t entry_type;
  uint8_t record_length;
  uint8_t bus_source;
  uint8_t irq_source;
  uint32_t global_system_interrupt;
  uint16_t flags;
} io_apic_interrupt_source_overdrive;

typedef struct io_apic_non_maskable_interrupt_source_struct {
  uint8_t entry_type;
  uint8_t record_length;
  uint8_t nmi_source;
  uint8_t reserved;
  uint16_t flags;
  uint32_t global_system_interrupt;
} io_apic_non_maskable_interrupt_source;

typedef struct local_apic_non_maskable_interrupts_struct {
  uint8_t entry_type;
  uint8_t record_length;
  uint8_t acpi_processor_id;
  uint16_t flags;
  uint8_t lint;
} local_apic_non_maskable_interrupts;

typedef struct local_apic_address_override_struct {
  uint8_t entry_type;
  uint8_t record_length;
  uint16_t reserved;
  uint32_t phys_address_of_local_apic_low; // low and high order correct?
  uint32_t phys_address_of_local_apic_high;
} local_apic_address_override;

typedef struct processor_local_x2apic_struct {
  uint8_t entry_type;
  uint8_t record_length;
  uint16_t reserved;
  uint32_t processors_local_x2apic_id;
  uint32_t flags;
  uint32_t ACPI_id;
} processor_local_x2apic;
