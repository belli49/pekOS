.global flush_tss

flush_tss:
	movl $0x28, %eax
	ltr %ax
	ret
