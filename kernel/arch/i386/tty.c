#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/tty.h>
#include <kernel/iox.h>

#include "vga.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xC03FF000;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

// _____ CURSOR _____

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end) {
  outb(0x3D4, 0x0A);
  outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);

  outb(0x3D4, 0x0B);
  outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

void disable_cursor() {
  outb(0x3D4, 0x0A);
  outb(0x3D5, 0x20);
}

void update_cursor(int x, int y) {
  uint16_t pos = y * VGA_WIDTH + x;

  outb(0x3D4, 0x0F);
  outb(0x3D5, (uint8_t) (pos & 0xFF));
  outb(0x3D4, 0x0E);
  outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

uint16_t get_cursor_position(void) {
  uint16_t pos = 0;
  outb(0x3D4, 0x0F);
  pos |= inb(0x3D5);
  outb(0x3D4, 0x0E);
  pos |= ((uint16_t)inb(0x3D5)) << 8;
  return pos;
}

void cursor_next() {
  uint16_t pos = get_cursor_position();
  pos++;
  update_cursor(pos / VGA_WIDTH, pos % VGA_WIDTH);
}

// _____ TERMINAL _____

void terminal_initialize(void) {
  terminal_row = 0;
  terminal_column = 0;
  terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
  terminal_buffer = VGA_MEMORY;
  for (size_t y = 0; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      const size_t index = y * VGA_WIDTH + x;
      terminal_buffer[index] = vga_entry(' ', terminal_color);
    }
  }

  //enable_cursor(0, 15);
}

void terminal_setcolor(uint8_t color) {
  terminal_color = color;
}

void terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) {
  const size_t index = y * VGA_WIDTH + x;
  terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c) {
  unsigned char uc = c;

  if (uc == '\n') {
    terminal_clear_rest_of_line();
    return;
  }

  terminal_putentryat(uc, terminal_color, terminal_column, terminal_row);

  terminal_column++;
  terminal_handle_position();
}

void terminal_write(const char* data, size_t size) {
  for (size_t i = 0; i < size; i++) {
    terminal_putchar(data[i]);
  }

  update_cursor(terminal_column, terminal_row);
}

void terminal_writestring(const char* data) {
  terminal_write(data, strlen(data));
}

void terminal_handle_position(void) {
  if (terminal_column == VGA_WIDTH) {
    terminal_column = 0;
    terminal_row++;
  }

  if (terminal_row == VGA_HEIGHT) {
    terminal_row = 0;
    terminal_column = 0;
  }
}

void terminal_scroll_down_by_amount(int d) {
  for (size_t i = 0; i < VGA_HEIGHT - d; ++i) {
    for (size_t j = 0; j < VGA_WIDTH; j++) {
      size_t index = i * VGA_WIDTH + j;
      size_t copyIndex = (i + d) * VGA_WIDTH + j;
      terminal_buffer[index] = terminal_buffer[copyIndex];
    }
  }
}

void terminal_scroll_down(int d) {
  for (size_t i = 0; i < VGA_HEIGHT - d; ++i) {
    for (size_t j = 0; j < VGA_WIDTH; j++) {
      size_t index = i * VGA_WIDTH + j;
      size_t copyIndex = (i + d) * VGA_WIDTH + j;
      terminal_buffer[index] = terminal_buffer[copyIndex];
    }
  }
}

void terminal_new_row(void) {
  if (++terminal_row == VGA_HEIGHT - 1) {
    terminal_scroll_down(1);
    --terminal_row;
  }
}

void terminal_clear_rest_of_line() {
  while (terminal_column != 0) {
    terminal_putchar(' ');
  }
}
