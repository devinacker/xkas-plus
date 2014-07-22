arch snes.cpu; lorom

org $008000; fill $008000
org $008020; base $7e2000
  nop #2
test:
  jml test

