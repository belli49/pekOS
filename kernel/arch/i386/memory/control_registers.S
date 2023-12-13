.global _flush_TLB
_flush_TLB:
  movl %cr3, %eax
  movl %eax, %cr3
  ret

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

.global _read_cr2
_read_cr2:
  movl %cr2, %eax
  ret

.global _write_cr2
_write_cr2:
  pushl %ebp
  movl %esp, %ebp
  movl 8(%ebp), %eax
  movl %eax, %cr2
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
