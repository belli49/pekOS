#include <stdint.h>
#include <stdbool.h>

void init_ps2(void);
bool ps2_output_buffer_full();
uint8_t read_ps2_data();
void send_ps2_command(uint8_t command);
void send_device_data(uint8_t command);
uint8_t detect_ps2_device(uint8_t port);
void print_keyboard_scan_codes(void);
