#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/paging.h>

extern void _read_cr0();

void paging_init(void) {
    _read_cr0();
    printf("ok\n");

    return;
}

