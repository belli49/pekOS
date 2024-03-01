#include <stdint.h>
#include <stdio.h>

#include <kernel/serial.h>
#include <kernel/iox.h>
#include <kernel/fadt.h>


#define PS2_DATA_PORT 0x60
#define PS2_STATUS_REGISTER 0x64
#define PS2_COMMAND_REGISTER 0x64

FADT* fadt;

void init_fadt() {
  fadt = (FADT*) find_by_header("FACP");

  if (!rsdp->revision || fadt->BootArchitectureFlags & 2) {
    // if ACPI 1.0 just assume there is ps/2 controller
    // there is PS/2 controller
    printf("PS/2 available\n");

    // disable ports
    outb(PS2_COMMAND_REGISTER, 0xAD); // disable first IO port
    outb(PS2_COMMAND_REGISTER, 0xA7); // disable second IO port (ignored if single channel)

    // flush output buffer
    while (inb(PS2_STATUS_REGISTER) & 1) {
      if (!inb(PS2_DATA_PORT)) break;
      printf("flushing ps/2 out buffer\n");
    }

    // set controller configuration byte
    outb(PS2_COMMAND_REGISTER, 0x20);
    uint8_t controller_config = inb(PS2_DATA_PORT) & 0xBC;
    outb(PS2_COMMAND_REGISTER, 0x60);
    outb(PS2_DATA_PORT, controller_config);

    // test ps/2 controller
    outb(PS2_COMMAND_REGISTER, 0xAA);
    
      if (inb(PS2_DATA_PORT) != 0x55) {
      printf("didn't work :(\n");

      return;
    } else {
      printf("worked :)\n");
    }


    // TODO: either sth is wrong or QEMU does not support a second channel
    //       try to fix later
    // detect if dual channel and disable second channel if so
    bool is_dual_channel = controller_config & 0x20;
    if (is_dual_channel) {
      printf("Checking if dual channel ps2 supported \n");

      // enable second port
      outb(PS2_COMMAND_REGISTER, 0xA8);

      // read controller configuration byte
      outb(PS2_COMMAND_REGISTER, 0x20);
      uint8_t second_controller_config = inb(PS2_DATA_PORT);
      printf("%x\n", second_controller_config);
      is_dual_channel = second_controller_config & 0x20;

      // disable second port
      if (is_dual_channel) {
        printf("dual channel ps/2 detected\n");
        outb(PS2_COMMAND_REGISTER, 0xA7);
      }
    }

  
  } else {
    // no PS/2 controller

    printf("No PS/2 controller\n");
  }
}
