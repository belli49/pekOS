#include <stdint.h>

uint8_t read_byte_at_address(uintptr_t* addr);
uint16_t read_word_at_address(uintptr_t* addr);
uint32_t read_at_address(uintptr_t* addr);

void print_byte_at_address(uintptr_t* addr);
void print_word_at_address(uintptr_t* addr);
void print_at_address(uintptr_t* addr);
