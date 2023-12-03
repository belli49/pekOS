# Declare constants for the multiboot header.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

# Declare a multiboot header that marks the program as a kernel.
.section .multiboot.data, "aw"
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Allocate the initial stack.
.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

# Preallocate pages used for paging. Don't hard-code addresses and assume they
# are available, as the bootloader might have loaded its multiboot structures or
# modules there. This lets the bootloader know it must avoid the addresses.
.section .bss, "aw", @nobits
  .align 4096
boot_page_directory:
  .skip 4096
# First kernel page table
boot_page_table1:
  .skip 4096
# Added new table for testing
boot_page_table2:
  .skip 4096
# Further page tables may be required if the kernel grows beyond 3 MiB.

# The kernel entry point.
.section .multiboot.text, "a"
.global _start
.type _start, @function
_start:
# Physical address of boot_page_table1.
  movl $(boot_page_table1 - 0xC0000000), %edi
# First address to map is address 0.
# TODO: Mapping everything from address 0 until end of kernel.
#       Alternatively map the first
#       1 MiB as it can be generally useful, and there's no need to
#       specially map the VGA buffer.
  movl $0, %esi
# Map 1023 pages. The 1024th will be the VGA text buffer.
  movl $1023, %ecx

1:
/*
# Only map the kernel.
  cmpl $_kernel_start, %esi
  jl 2f
*/

  cmpl $(_kernel_end - 0xC0000000), %esi
  jge 3f

# Map physical address as "present, writable". Note that this maps
# .text and .rodata as writable. Mind security and map them as non-writable.
  movl %esi, %edx
  orl $0x003, %edx
  movl %edx, (%edi)

2:
# Size of page is 4096 bytes.
  addl $4096, %esi
# Size of entries in boot_page_table1 is 4 bytes.
  addl $4, %edi
# Loop to the next entry if we haven't finished.
  loop 1b

3:
# Map VGA video memory to 0xC03FF000 as "present, writable".
  movl $(0x000B8000 + 0x003), boot_page_table1 - 0xC0000000 + 1023 * 4

# The page table is used at both page directory entry 0 (virtually from 0x0
# to 0x3FFFFF) (thus identity mapping the kernel) and page directory entry
# 768 (virtually from 0xC0000000 to 0xC03FFFFF) (thus mapping it in the
# higher half). The kernel is identity mapped because enabling paging does
# not change the next instruction, which continues to be physical. The CPU
# would instead page fault if there was no identity mapping.

# Map the page table to both virtual addresses 0x00000000 and 0xC0000000.
  movl $(boot_page_table1 - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 0
  movl $(boot_page_table1 - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 768 * 4

# Map 0x80000000 to 0x80000000 for testing purposes
  movl $(0x80000000 + 0x003), boot_page_table2 - 0xC0000000
  movl $(boot_page_table2 - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 512 * 4

# Self map last PD to its last PDE so that the PD looks like a PT to the system;
# this way, you can access PD at 0xFFFFF000.
# Why? Because 0xFFFFF000 is the last PDE (idx 1023) of the PD. 
# When trying to access it's value (virtually, PE), the system will check
# the last PDE of the PD (because first 10 bits of address == 0x3FF),
# which will point it to the supposed PT (here, the physical address of the PD).
# Then, it will look to the last PTE (because next 10 bits of address == 0x3FF),
# which will point it to the physical address (again, the physical address of the PD),
# thus accessing the PD successfully.
  movl $(boot_page_directory - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 1023 * 4

# Set cr3 to the address of the boot_page_directory.
  movl $(boot_page_directory - 0xC0000000), %ecx
  movl %ecx, %cr3

# Enable paging and the write-protect bit (and the protected mode bit, in this order from left to right).
  movl %cr0, %ecx
  orl $0x80010001, %ecx
  movl %ecx, %cr0

# Jump to higher half with an absolute jump. 
  lea 4f, %ecx
  jmp *%ecx

.section .text

4:
# At this point, paging is fully set up and enabled.

# Unmap the identity mapping as it is now unnecessary. 
  movl $0, boot_page_directory + 0

# Reload crc3 to force a TLB flush so the changes to take effect.
  movl %cr3, %ecx
  movl %ecx, %cr3

# Set up the stack.
  mov $stack_top, %esp

# Call global constructors
  call _init

# Enter the high-level kernel.
  call kernel_main

# Infinite loop if the system has nothing more to do.
  cli
1:
  hlt
  jmp 1b
