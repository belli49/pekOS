#include <stdint.h>
#include <string.h>

#include <kernel/rsdt.h>

bool doChecksum(ACPISDTHeader *tableHeader) {
  unsigned char sum = 0;

  for (int i = 0; i < tableHeader->Length; i++) {
    sum += ((char *) tableHeader)[i];
  }

  return sum == 0;
}

void *findFACP(void *RootSDT) {
  RSDT *rsdt = (RSDT *) RootSDT;
  int entries = (rsdt->h.Length - sizeof(rsdt->h)) / 4;

  for (int i = 0; i < entries; i++) {
    ACPISDTHeader *h = (ACPISDTHeader *) rsdt->PointerToOtherSDT[i];
    if (!strncmp(h->Signature, "FACP", 4))
      return (void *) h;
  }

  // No FACP found
  return NULL;
}
