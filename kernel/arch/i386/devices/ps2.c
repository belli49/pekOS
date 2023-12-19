#include <stdio.h>
#include <kernel/fadt.h>

FADT* fadt;

void init_fadt() {
  fadt = (FADT*) find_by_header("FACP");

  if (fadt->BootArchitectureFlags & 2) {
    // there is PS/2 controller
    printf("PS/2 ok\n");
  } else {
    // no PS/2 controller

    printf("PS/2 ok\n");
  }
}
