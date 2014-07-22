arch snes.cpu; lorom

// header commands can be abused to insert custom copier headers
noheader
  db "custom copier header", $00

header
// below two fill routines are functionally equivalent
org $8000; fill   $010000, $00 // "fill N [,value]" writes N number of bytes
org $8000; fillto $028000, $00 // "fillto N [,value]" writes until N offset is reached

org $8000; opcode_list:
  brk #$11      //00
  ora ($11,x)   //01
  cop #$11      //02
  ora $11,s     //03
  tsb $11       //04
  ora $11       //05
  asl $11       //06
  ora [$11]     //07
  php           //08
  ora #$11      //09
  asl           //0a
  phd           //0b
  tsb $1122     //0c
  ora $1122     //0d
  asl $1122     //0e
  ora $112233   //0f

  bpl $11       //10
  ora ($11),y   //11
  ora ($11)     //12
  ora ($11,s),y //13
  trb $11       //14
  ora $11,x     //15
  asl $11,x     //16
  ora [$11],y   //17
  clc           //18
  ora $1122,y   //19
  inc           //1a
  tcs           //1b
  trb $1122     //1c
  ora $1122,x   //1d
  asl $1122,x   //1e
  ora $112233,x //1f

  jsr $1122     //20
  and ($11,x)   //21
  jsl $112233   //22
  and $11,s     //23
  bit $11       //24
  and $11       //25
  rol $11       //26
  and [$11]     //27
  plp           //28
  and #$11      //29
  rol           //2a
  pld           //2b
  bit $1122     //2c
  and $1122     //2d
  rol $1122     //2e
  and $112233   //2f

  bmi $11       //30
  and ($11),y   //31
  and ($11)     //32
  and ($11,s),y //33
  bit $11,x     //34
  and $11,x     //35
  rol $11,x     //36
  and [$11],y   //37
  sec           //38
  and $1122,y   //39
  dec           //3a
  tsc           //3b
  bit $1122,x   //3c
  and $1122,x   //3d
  rol $1122,x   //3e
  and $112233,x //3f

  rti           //40
  eor ($11,x)   //41
  wdm #$11      //42
  eor $11,s     //43
  mvp $11,$22   //44
  eor $11       //45
  lsr $11       //46
  eor [$11]     //47
  pha           //48
  eor #$11      //49
  lsr           //4a
  phk           //4b
  jmp $1122     //4c
  eor $1122     //4d
  lsr $1122     //4e
  eor $112233   //4f

  bvc $11       //50
  eor ($11),y   //51
  eor ($11)     //52
  eor ($11,s),y //53
  mvn $11,$22   //54
  eor $11,x     //55
  lsr $11,x     //56
  eor [$11],y   //57
  cli           //58
  eor $1122,y   //59
  phy           //5a
  tcd           //5b
  jml $112233   //5c
  eor $1122,x   //5d
  lsr $1122,x   //5e
  eor $112233,x //5f

  rts           //60
  adc ($11,x)   //61
  per $1122     //62
  adc $11,s     //63
  stz $11       //64
  adc $11       //65
  ror $11       //66
  adc [$11]     //67
  pla           //68
  adc #$11      //69
  ror           //6a
  rtl           //6b
  jmp ($1122)   //6c
  adc $1122     //6d
  ror $1122     //6e
  adc $112233   //6f

  bvs $11       //70
  adc ($11),y   //71
  adc ($11)     //72
  adc ($11,s),y //73
  stz $11,x     //74
  adc $11,x     //75
  ror $11,x     //76
  adc [$11],y   //77
  sei           //78
  adc $1122,y   //79
  ply           //7a
  tdc           //7b
  jmp ($1122,x) //7c
  adc $1122,x   //7d
  ror $1122,x   //7e
  adc $112233,x //7f

  bra $11       //80
  sta ($11,x)   //81
  brl $1122     //82
  sta $11,s     //83
  sty $11       //84
  sta $11       //85
  stx $11       //86
  sta [$11]     //87
  dey           //88
  bit #$11      //89
  txa           //8a
  phb           //8b
  sty $1122     //8c
  sta $1122     //8d
  stx $1122     //8e
  sta $112233   //8f

  bcc $11       //90
  sta ($11),y   //91
  sta ($11)     //92
  sta ($11,s),y //93
  sty $11,x     //94
  sta $11,x     //95
  stx $11,y     //96
  sta [$11],y   //97
  tya           //98
  sta $1122,y   //99
  txs           //9a
  txy           //9b
  stz $1122     //9c
  sta $1122,x   //9d
  stz $1122,x   //9e
  sta $112233,x //9f

  ldy #$11      //a0
  lda ($11,x)   //a1
  ldx #$11      //a2
  lda $11,s     //a3
  ldy $11       //a4
  lda $11       //a5
  ldx $11       //a6
  lda [$11]     //a7
  tay           //a8
  lda #$11      //a9
  tax           //aa
  plb           //ab
  ldy $1122     //ac
  lda $1122     //ad
  ldx $1122     //ae
  lda $112233   //af

  bcs $11       //b0
  lda ($11),y   //b1
  lda ($11)     //b2
  lda ($11,s),y //b3
  ldy $11,x     //b4
  lda $11,x     //b5
  ldx $11,y     //b6
  lda [$11],y   //b7
  clv           //b8
  lda $1122,y   //b9
  tsx           //ba
  tyx           //bb
  ldy $1122,x   //bc
  lda $1122,x   //bd
  ldx $1122,y   //be
  lda $112233,x //bf

  cpy #$11      //c0
  cmp ($11,x)   //c1
  rep #$11      //c2
  cmp $11,s     //c3
  cpy $11       //c4
  cmp $11       //c5
  dec $11       //c6
  cmp [$11]     //c7
  iny           //c8
  cmp #$11      //c9
  dex           //ca
  wai           //cb
  cpy $1122     //cc
  cmp $1122     //cd
  dec $1122     //ce
  cmp $112233   //cf

  bne $11       //d0
  cmp ($11),y   //d1
  cmp ($11)     //d2
  cmp ($11,s),y //d3
  pei ($11)     //d4
  cmp $11,x     //d5
  dec $11,x     //d6
  cmp [$11],y   //d7
  cld           //d8
  cmp $1122,y   //d9
  phx           //da
  stp           //db
  jmp [$1122]   //dc
  cmp $1122,x   //dd
  dec $1122,x   //de
  cmp $112233,x //df

  cpx #$11      //e0
  sbc ($11,x)   //e1
  sep #$11      //e2
  sbc $11,s     //e3
  cpx $11       //e4
  sbc $11       //e5
  inc $11       //e6
  sbc [$11]     //e7
  inx           //e8
  sbc #$11      //e9
  nop           //ea
  xba           //eb
  cpx $1122     //ec
  sbc $1122     //ed
  inc $1122     //ee
  sbc $112233   //ef

  beq $11       //f0
  sbc ($11),y   //f1
  sbc ($11)     //f2
  sbc ($11,s),y //f3
  pea $1122     //f4
  sbc $11,x     //f5
  inc $11,x     //f6
  sbc [$11],y   //f7
  sed           //f8
  sbc $1122,y   //f9
  plx           //fa
  xce           //fb
  jsr ($1122,x) //fc
  sbc $1122,x   //fd
  inc $1122,x   //fe
  sbc $112233,x //ff

  // incsrc / incbin support
  incsrc "test_l1.asm"; incsrc "test_l2.asm"
  incbin "inc.bin"

  // +/- branch support
  -; bcc -
  bcs +; +

  // namespace support
  label1:; db $00
namespace ns1
  label1:; db $00
  .sublabel:; db $00
namespace global
  dw label1, ns1::label1, ns1::label1.sublabel

  // define support
  define add "clc; adc"
  {add} #$11
  define add ""
  {add}

  // immediate opcode repetition support
  asl #4
  inc #2
  nop #2

  // file padding
  db "~ byuu"
  db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
  db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
  db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
  db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00

  print "eof = ", pc, " ..."
