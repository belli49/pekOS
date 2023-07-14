.global flush_tss

flush_tss:
	mov $0x28, %ax
	ltr %ax
	ret
