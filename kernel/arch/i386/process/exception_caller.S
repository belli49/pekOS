.extern exception_handler
.extern page_fault_handler

.macro isr_err_stub num
isr_stub_\num:
  call exception_handler
  iret
.endm

.macro isr_no_err_stub num
isr_stub_\num:
  call exception_handler
  iret
.endm

isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2
isr_no_err_stub 3
isr_no_err_stub 4
isr_no_err_stub 5
isr_no_err_stub 6
isr_no_err_stub 7
isr_err_stub    8
isr_no_err_stub 9
isr_err_stub    10
isr_err_stub    11
isr_err_stub    12
isr_err_stub    13

# page fault exception (14)
isr_stub_14:
  pushal

  movl %cr2, %eax
  pushl 32(%esp)
  pushl %eax
  cld
  call page_fault_handler
  addl $8, %esp
  
  popal
# is this correct? check later
  addl $4, %esp
  iret

isr_no_err_stub 15
isr_no_err_stub 16
isr_err_stub    17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_no_err_stub 21
isr_no_err_stub 22
isr_no_err_stub 23
isr_no_err_stub 24
isr_no_err_stub 25
isr_no_err_stub 26
isr_no_err_stub 27
isr_no_err_stub 28
isr_no_err_stub 29
isr_err_stub    30
isr_no_err_stub 31



# other IRQs (>= 32)
# pic IRQ0 (32) # PIT?
isr_stub_32:
  pushal
  pushl $32

  cld
  call exception_handler_code

  popl %eax
  popal
  iret

# pic IRQ 0 (33)
isr_stub_33:
  pushal
  pushl $33

  cld
  call test_kbd_interrupt

  popl %eax
  popal
  iret



# macro for PIC (from 34)
.altmacro

.macro isr_other num
isr_stub_\num:
  pushal
  pushl $\num

  cld
  call exception_handler_code

  popl %eax
  popal
  iret
.endm

.macro loop_insert_PIC_irqs
  i = 34
  .rept 14
    isr_other %i
    i = i + 1
  .endr
.endm

loop_insert_PIC_irqs




# apic timer interrupt (48)
isr_stub_48:
  pushal

  cld
  call system_timer_interrupt

  popal
  iret




# interrupt (49)
isr_stub_49:
  pushal
  pushl $49

  cld
  call exception_handler_code

  popl %eax
  popal
  iret


# interrupt (50)
isr_stub_50:
  pushal
  pushl $50

  cld
  call test_kbd_interrupt

  popl %eax
  popal
  iret


# rest of apic (from 51)
.macro loop_insert_apic_irqs
  i = 51
  .rept 14
    isr_other %i
    i = i + 1
  .endr
.endm

loop_insert_apic_irqs



# macro for rest (from 64)
.macro loop_insert_other_irqs
  i = 65
  .rept 191
    isr_other %i
    i = i + 1
  .endr
.endm

loop_insert_other_irqs





.altmacro

.macro create_isr_stub num
  .long isr_stub_\num
.endm

.macro loop_insert_stub
  i = 0
  .rept 256
    create_isr_stub %i
    i = i + 1
  .endr
.endm


.global isr_stub_table
isr_stub_table:
  loop_insert_stub
