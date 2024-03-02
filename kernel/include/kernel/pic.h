#include <stdint.h>


void init_pic(void);
void mask_pic_interrupts(void);
void PIC_remap(int offset1, int offset2);
void PIC_sendEOI(unsigned char irq);
void IRQ_set_mask(unsigned char IRQline);
void IRQ_clear_mask(unsigned char IRQline);
uint16_t pic_get_irr(void);
uint16_t pic_get_isr(void);
