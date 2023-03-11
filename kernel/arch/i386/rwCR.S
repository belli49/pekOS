.global _read_cr0
_read_cr0:
  movl %cr0, %eax
  ret

.global _write_cr0
_write_cr0:
  pushl %ebp
  movl %esp, %ebp
  movl 8(%ebp), %eax
  movl %eax, %cr0
  popl %ebp
  retl

.global _read_cr3
_read_cr3:
  movl %cr3, %eax
  retl

.global _write_cr3
_write_cr3:
  pushl %ebp
  movl %esp, %ebp
  movl 8(%ebp), %eax
  movl %eax, %cr3
  popl %ebp
  ret

