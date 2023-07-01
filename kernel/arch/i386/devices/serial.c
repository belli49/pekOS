#include <stdio.h>
#include <stdint.h>

#include <kernel/serial.h>
#include <kernel/tty.h>
#include <kernel/iox.h>

#define PORT 0x3f8          // COM1
#define BAUD_RATE 0x0C      // Baud rate; 0x0C is default for QEMU

// _____ INIT _____

int init_serial() {
  outb(PORT + 1, 0x00);      // Disable all interrupts
  outb(PORT + 3, 0x80);      // Enable DLAB (set baud rate divisor)
  outb(PORT + 0, BAUD_RATE); // Set divisor to 12 (lo byte) 9600 baud (default for QEMU)
  outb(PORT + 1, 0x00);      //                  (hi byte)
  outb(PORT + 3, 0x03);      // 8 bits, no parity, one stop bit
  outb(PORT + 2, 0xC7);      // Enable FIFO, clear them, with 14-byte threshold
  outb(PORT + 4, 0x0B);      // IRQs enabled, RTS/DSR set
  outb(PORT + 4, 0x1E);      // Set in loopback mode, test the serial chip
  outb(PORT + 0, 0xAE);      // Test serial chip (send byte 0xAE and check if serial returns same byte)

  // Check if serial is faulty (i.e: not same byte as sent)
  if (inb(PORT + 0) != 0xAE) {
    return 1;
  }
 
  // If serial is not faulty set it in normal operation mode
  // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
  outb(PORT + 4, 0x0F);

  printf("IO to serial port enabled\n");
  write_string_serial("IO to serial port enabled\n");
  return 0;
}

// _____ I/O _____

// SEND
int is_transmit_empty() {
  return inb(PORT + 5) & 0x20;
}

void write_serial(char a) {
  while (is_transmit_empty() == 0);

  outb(PORT,a);
}

void write_string_serial(char str[]) {
  char *ptr = str;

  while (*ptr != '\0') {
    write_serial(*ptr);
    ptr++;
  }
}

// RECEIVE
int serial_received() {
  return inb(PORT + 5) & 1;
}

char read_serial() {
  while (serial_received() == 0);

  return inb(PORT);
}
