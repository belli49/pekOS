#include <stdint.h>
#include <stdbool.h>

typedef struct ACPISDTHeader_struct ACPISDTHeader;
typedef struct RSDT_struct RSDT;
typedef struct XSDT_struct XSDT;

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

struct RSDP_t {
 char Signature[8];
 uint8_t Checksum;
 char OEMID[6];
 uint8_t Revision;
 uint32_t RsdtAddress;
} __attribute__ ((packed));

struct XSDP_t {
 char Signature[8];
 uint8_t Checksum;
 char OEMID[6];
 uint8_t Revision;
 uint32_t RsdtAddress;      // deprecated since version 2.0
 
 uint32_t Length;
 uint64_t XsdtAddress;
 uint8_t ExtendedChecksum;
 uint8_t reserved[3];
} __attribute__ ((packed));

bool doChecksum(ACPISDTHeader *tableHeader);
void *findFACP(void *RootSDT);