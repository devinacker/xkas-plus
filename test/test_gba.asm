arch gba.thumb

//comment
org $08000000
  ldr r0,=#0x12345678
  nop; ldr r0,=#0x12345678

  lsr r0,r1,#4
  add r0,r1,#4
  mov r0,r1
  cmp r0,#4
  eor r0,r1
  add r0,r8
  nop
  bx lr
  ldr r0,[pc,#4]
  str r0,[r1,r2]
  ldsb r0,[r1,r2]
  ldrb r0,[r1,#4]
  strh r0,[r1,#4]
  str r0,[sp,#4]
  add r0,sp,#4
  add sp,#-4
  push {r0-r3,r7,lr}
  stmia r0!,{r0-r3,r7}
  -; beq -
  bne +; +
  swi #4
  bkpt #4
  -; b -
  -; bl -
