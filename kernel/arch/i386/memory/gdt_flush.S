.global gdt
.global flush_gdt
.global gdt_size

gdt_end:
  .long

gdtr:
  .short 0    // For limit storage
  .long 0    // For base storage

flush_gdt:
  // calculate gdt_end
  movl $gdt_size, %eax
  leal gdt(%eax), %edx
  movl %edx, gdt_end

  cli
  xorl %eax, %eax
  movw %ds, %ax
  shl $4, %eax
  addl $gdt, %eax
  movl %eax, gdtr + 2
  subl $gdt, gdt_end
  movl $gdt_end, %eax
  movw %ax, gdtr
  lgdt gdtr
  ret
