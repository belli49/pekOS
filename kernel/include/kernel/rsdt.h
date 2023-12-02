#include <stdint.h>
#include <stdbool.h>

typedef struct ACPISDTHeader_struct ACPISDTHeader;
typedef struct RSDT_struct RSDT;
typedef struct XSDT_struct XSDT;
typedef struct RSDP_struct RSDP;
typedef struct XSDP_struct XSDP;

struct ACPISDTHeader_struct {
  char Signature[4];
  uint32_t Length;
  uint8_t Revision;
  uint8_t Checksum;
  char OEMID[6];
  char OEMTableID[8];
  uint32_t OEMRevision;
  uint32_t CreatorID;
  uint32_t CreatorRevision;
} __attribute__ ((packed));

struct RSDT_struct {
  ACPISDTHeader h;
  // uint32_t PointerToOtherSDT[(h.Length - sizeof(h)) / 4];
  uint32_t* PointerToOtherSDT;
};

struct XSDT_struct {
  ACPISDTHeader h;
  // uint64_t PointerToOtherSDT[(h.Length - sizeof(h)) / 8];
  uint64_t* PointerToOtherSDT;
};

struct RSDP_struct {
 char Signature[8];
 uint8_t Checksum;
 char OEMID[6];
 uint8_t Revision;
 uint32_t RsdtAddress;
};

struct XSDP_struct {
 char Signature[8];
 uint8_t Checksum;
 char OEMID[6];
 uint8_t Revision;
 uint32_t RsdtAddress;      // deprecated since version 2.0
 
 uint32_t Length;
 uint64_t XsdtAddress;
 uint8_t ExtendedChecksum;
 uint8_t reserved[3];
};

uintptr_t find_RSDT(void);
void *findFACP(void *RootSDT);
bool do_checksum_ACPISDT(ACPISDTHeader *tableHeader);
bool do_checksum_RSDP(RSDP* rsdp);
