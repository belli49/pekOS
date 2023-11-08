.extern exception_handler

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
isr_err_stub    14
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



.altmacro

.macro create_isr_stub num
  .long isr_stub_\num
.endm

.macro loop_insert_stub
    i = 0
    .rept 32
      create_isr_stub %i
      i = i + 1
    .endr
.endm


.global isr_stub_table
isr_stub_table:
  loop_insert_stub
