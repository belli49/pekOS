#include <stdint.h>
#include <stdio.h>

#include <kernel/serial.h>
#include <kernel/iox.h>
#include <kernel/fadt.h>
#include <kernel/ps2.h>


#define PS2_DATA_PORT 0x60
#define PS2_STATUS_REGISTER 0x64
#define PS2_COMMAND_REGISTER 0x64

FADT* fadt;

bool first_controller_works = false;
bool second_controller_works = false;

uint8_t device_1;
uint8_t device_2;

uint8_t command_res;

void init_ps2() {
  fadt = (FADT*) find_by_header("FACP");

  // check is ps/2 controller available
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
    send_ps2_command(0x20);
    uint8_t controller_config = read_ps2_data() & 0xBC;
    send_ps2_command(0x60);
    outb(PS2_DATA_PORT, controller_config);



    // test ps/2 controller
    send_ps2_command(0xAA);
    uint8_t response = read_ps2_data();
    
    if (response != 0x55) {
      printf("didn't work :(\n");
      return;
    } else {
      printf("worked :)\n");
    }

    // restore controller config byte (some hardware needs this behaviour)
    send_ps2_command(0x60);
    outb(PS2_DATA_PORT, controller_config);



    // detect if dual channel and disable second channel if so
    bool is_dual_channel = controller_config & 0x20;
    if (is_dual_channel) {
      printf("Checking if dual channel ps2 supported \n");

      // enable second port
      send_ps2_command(0xA8);

      // read controller configuration byte
      send_ps2_command(0x20);
      is_dual_channel = !(read_ps2_data() & 0x20); // if bit is clear it is dual channel

      // disable second port
      if (is_dual_channel) {
        printf("Dual channel ps/2 detected\n");
        send_ps2_command(0xA7);
      }
    }


  
    // interface tests
    // first port
    send_ps2_command(0xAB);
    first_controller_works = !read_ps2_data();
    if (!first_controller_works) printf("Interface test failed for port 1\n");

    // second port
    send_ps2_command(0xA9);
    second_controller_works = !read_ps2_data();
    if (!second_controller_works) printf("Interface test failed for port 2\n");



    // enable controllers
    if (first_controller_works) send_ps2_command(0xAE);
    if (second_controller_works) send_ps2_command(0xA8);



    /*
     * TODO: first need to set up APIC
    // enable interrupts
    outb(PS2_COMMAND_REGISTER, 0x20);
    controller_config = inb(PS2_DATA_PORT) | 3;
    outb(PS2_COMMAND_REGISTER, 0x60);
    outb(PS2_DATA_PORT, controller_config);
    */




    // reset devices
    // first port
    send_device_data(0xFF);
    uint8_t reset_res = read_ps2_data();
    if (reset_res == 0xFA) {
      printf("reset 1 res: ACK\n");

      reset_res = read_ps2_data();
      if (reset_res == 0xAA) printf("Reset successful for port 1\n");
    }

    // second port
    send_ps2_command(0xD4);
    send_device_data(0xFF);
    reset_res = read_ps2_data();
    if (reset_res == 0xFA) {
      printf("reset 2 res: ACK\n");

      reset_res = read_ps2_data();
      if (reset_res == 0xAA) printf("Reset successful for port 2\n");
    }



    // detect ps2 device
    // port 1
    if (first_controller_works) device_1 = detect_ps2_device(1);

    // port 2
    if (second_controller_works) device_2 = detect_ps2_device(2);
    



    // test device 1 by polling
    // enable scanning
    send_device_data(0xF4);
    command_res = read_ps2_data();
    if (command_res == 0xFA) printf("ACK\n");


    print_keyboard_scan_codes();

  } else {
    // no PS/2 controller

    printf("No PS/2 controller\n");
  }
}



bool ps2_output_buffer_full() {
  return inb(PS2_STATUS_REGISTER) & 1;
}



uint8_t read_ps2_data() {
  while (!ps2_output_buffer_full()); // wait until buffer full
  return inb(PS2_DATA_PORT);
}



void send_ps2_command(uint8_t command) {
  while (inb(PS2_STATUS_REGISTER) & 2); // wait until ps/2 is ready
  outb(PS2_COMMAND_REGISTER, command);
}


void send_device_data(uint8_t command) {
  while (inb(PS2_STATUS_REGISTER) & 2); // wait until ps/2 is ready
  outb(PS2_DATA_PORT, command);
}



uint8_t detect_ps2_device(uint8_t port) {
  uint8_t res, res2;

  // disable scanning
  if (port == 2) send_ps2_command(0xD4);
  send_device_data(0xF5);
  res = read_ps2_data();
  if (res == 0xFA) printf("ACK\n");

  // Send command to read device ID
  send_device_data(0xF2);
  res = read_ps2_data();
  if (res == 0xFA) printf("ACK\n");

  // Read response from the PS/2 device
  res = read_ps2_data();

  io_wait();
  res2 = read_ps2_data();
  printf("%x, %x\n", res, res2);

  // Determine device type based on response
  if (res == 0xAB) 
    printf("PS/2 device connected to the port: Keyboard\n");
  else if (res == 0x00)
    printf("PS/2 device connected to the port: Mouse\n");
  else
    printf("PS/2 device connected to the port: Unknown (ID: %x)\n", res);

  return res;
}



void print_keyboard_scan_codes() {
    uint8_t scan_code;

    while (1) {
        scan_code = read_ps2_data();
        printf("Scan code received: %x\n", scan_code);
    }
}
