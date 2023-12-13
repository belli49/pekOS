#include <stdint.h>

#include <kernel/memory_access.h>

typedef struct page_table_struct page_table_t;

struct page_table_struct {
  uintptr_t pte[1024];
};

void init_page_allocation(void);
void map_virtual_memory(uintptr_t* virtual_mm);
void* get_pe_value(uintptr_t virtualaddr);
void* get_physaddr(uintptr_t virtualaddr);
void map_page(uintptr_t* physaddr, uintptr_t* virtualaddr, uintptr_t flags);
uintptr_t* find_free_virtaddr(void);
uintptr_t* find_free_physaddr(void);
uintptr_t* allocate_pt(void);
