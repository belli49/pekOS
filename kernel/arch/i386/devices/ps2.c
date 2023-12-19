#include <kernel/rsdt.h>

ACPISDTHeader* FADT;

void init_fadt() {
  FADT = find_by_header("FACP");
}
