#include <stdint.h>
#include <stdbool.h>

typedef struct ACPISDTHeader_struct ACPISDTHeader;
typedef struct RSDT_struct RSDT;
typedef struct XSDT_struct XSDT;
typedef struct RSDP_struct RSDP;
typedef struct XSDP_struct XSDP;

struct ACPISDTHeader_struct {
  char signature[4];
  uint32_t length;
  uint8_t revision;
  uint8_t checksum;
  char OEMID[6];
  char OEMTableID[8];
  uint32_t OEMRevision;
  uint32_t creatorID;
  uint32_t creator_revision;
} __attribute__ ((packed));

struct RSDT_struct {
  ACPISDTHeader h;
  // uint32_t PointerToOtherSDT[(h.Length - sizeof(h)) / 4];
  uint32_t* pointer_to_other_SDT;
};

struct XSDT_struct {
  ACPISDTHeader h;
  // uint64_t PointerToOtherSDT[(h.Length - sizeof(h)) / 8];
  uint64_t* pointer_to_other_SDT;
};

struct RSDP_struct {
 char signature[8];
 uint8_t checksum;
 char OEMID[6];
 uint8_t revision;
 uint32_t rsdt_address;
} __attribute__ ((packed));

struct XSDP_struct {
 char signature[8];
 uint8_t checksum;
 char OEMID[6];
 uint8_t revision;
 uint32_t rsdt_address;      // deprecated since version 2.0
 
 uint32_t length;
 uint64_t xsdt_address;
 uint8_t extended_checksum;
 uint8_t reserved[3];
} __attribute__ ((packed));


void init_rsdt(void);
void* find_RSDT(void);
RSDP* find_RSDP(void);
void *findFACP(void *RootSDT);
bool do_checksum_ACPISDT(ACPISDTHeader *tableHeader);
bool do_checksum_RSDP(RSDP* rsdp);
ACPISDTHeader* find_by_header(char head[5]);


extern bool ACPI_version;
extern RSDP* rsdp;
extern RSDT* rsdt;
extern XSDT* xsdt;
