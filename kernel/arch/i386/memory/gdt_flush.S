.global flush_gdt
.global reload_segments

.gdt_descriptor:
  .word 0 // gdt_size - 1
  .long 0 // gdt_address

flush_gdt:
  movl 4(%esp), %eax // gdt_address
  movw 8(%esp), %bx // gdt_size

  movl %eax, (.gdt_descriptor + 2)

  subw $1, %bx
  movw %bx, (.gdt_descriptor)

  lgdt (.gdt_descriptor)
  ret

reload_segments:
   // Reload CS register containing code selector:
   jmp $0x08, $.reload_CS   # 0x08 -> code segment

.reload_CS:
   // Reload data segment registers:
   mov $0x10, %ax         # 0x10 -> data segment
   mov %ax, %ds
   mov %ax, %es
   mov %ax, %fs
   mov %ax, %gs
   mov %ax, %ss
   ret
