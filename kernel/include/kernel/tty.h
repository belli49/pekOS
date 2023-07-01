#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stddef.h>
#include <stdint.h>

// _____ CURSOR ______
void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
void disable_cursor();
void update_cursor(int x, int y);
uint16_t get_cursor_position(void);
void cursor_next();

// _____ TERMINAL _____
void terminal_initialize(void);
void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
void terminal_handle_position(void);
void terminal_scroll_down_by_amount(int d);
void terminal_new_row(void);
void terminal_scroll_down(int d);

#endif
