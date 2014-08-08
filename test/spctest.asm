arch snes.smp
org $0000
base $0200

adc (x), (y)
adc a, #0
adc a, (x)
adc a, (0)+y
adc a, (0+x)
adc.b a, 0
adc.b a, 0+x
adc.w a, 0
adc.w a, 0+x
adc.w a, 0+y
adc 0, 1
adc 0, #$ff
addw ya, 0
and (x), (y)
and a, #$ff
and a, (x)
and a, (0)+y
and a, (0+x)
and.b a, 0
and.b a, 0+x
and.w a, 0
and.w a, 0+x
and.w a, 0+y
and 0, 1
and 0, #$ff
and1 c, /0, 7
and1 c, 0, 7
asl a
asl.b 0
asl 0+x
asl.w 0
-
bbc0 0, -
bbc1 0, -
bbc2 0, -
bbc3 0, -
bbc4 0, -
bbc5 0, -
bbc6 0, -
bbc7 0, -
bbs0 0, -
bbs1 0, -
bbs2 0, -
bbs3 0, -
bbs4 0, -
bbs5 0, -
bbs6 0, -
bbs7 0, -
bcc -
bcs -
beq -
bmi -
bne -
bpl - 
bvc -
bvs -
bra -
brk
call 0
cbne $FF+x, -
cbne 0, -
clr0 0
clr1 0
clr2 0
clr3 0
clr4 0
clr5 0
clr6 0
clr7 0

clrc
clrp
clrv
cmp (x), (y)
cmp a, #0
cmp a, (x)
cmp a, (0+x)
cmp a, (0)+y
cmp.b a, 0
cmp.b a, 0+x
cmp.w a, 0
cmp.w a, 0+x
cmp.w a, 0+y
cmp x, #0
cmp.b x, 0
cmp.w x, 0
cmp y, #0
cmp.b y, 0
cmp.w y, 0
cmp $12, $34
cmp $AA, #0
cmpw ya, $00
daa a
das a
-
dbnz y, -
dbnz $00, -
dec a
dec x
dec y
dec.b 0
dec 0+x
dec.w 0
decw 0
di
div ya, x
ei
eor (x), (y)
eor a, #0
eor a, (x)
eor a, (0)
eor a, (0+x)
eor a, (0)+y
eor.b a, 0
eor.b a, 0+x
eor.w a, 0
eor.w a, 0+x
eor.w a, 0+y
eor $12, $34
eor 0, $FF
eor1 c, -, 0
eor1 c, -, 5
inc a
inc x
inc y
inc.b 0
inc.b 0+x
inc.w 0
incw 0
jmp (0+x)
jmp 0
lsr a
lsr.b 0
lsr 0+x
lsr.w 0
mov (x)+, a
mov (x), a
mov (0+x), a
mov (0)+y, a
mov a, #0
mov a, (x)
mov a, (x)+
mov a, (0+x)
mov a, (0)+y
mov a, x
mov a, y
mov.b a, 0
mov.b a, 0+x
mov.w a, 0
mov.w a, 0+x
mov.w a, 0+y
mov sp, x
mov x, #0
mov x, a
mov x, sp
mov.b x, 0
mov x, 0+y
mov.w x, 0
mov y, #0
mov y, a
mov.b y, 0
mov y, 0+x
mov.w y, 0
mov $12, $34
mov.b 0+x, a
mov.b 0+x, y
mov.b 0+y, x
mov 0, #$ff
mov.b 0, a
mov.b 0, x
mov.b 0, y
mov.w 0+x, a
mov.w 0+y, a
mov.w 0, a
mov.w 0, x
mov.w 0, y

mov1 c, 0, 7
mov1 0, 7, c

movw ya, 0
movw 0, ya
mul ya
nop
not1 0, 7
notc
or (x), (y)
or a, #0
or a, (x)
or a, (0+x)
or a, (0)+y
or.b a, 0
or.b a, 0+x
or.w a, 0
or.w a, 0+x
or.w a, 0+y
or 0, 1
or 0, #0
or1 c, /$200, 7
or1 c, $200, 7
pcall 0
pop a
pop psw
pop x
pop y
push a
push psw
push x
push y
ret
reti
rol a
rol.b 0
rol.b 0+x
rol.w 0
ror a
ror.b 0
ror.b 0+x
ror.w 0
sbc (x), (y)
sbc a, #0
sbc a, (x)
sbc a, (0+x)
sbc a, (0)+y
sbc.b a, 0
sbc.b a, 0+x
sbc.w a, 0
sbc.w a, 0+x
sbc.w a, 0+y
sbc 0, 1
sbc 0, #0
set0 0
set1 0
set2 0
set3 0
set4 0
set5 0
set6 0
set7 0
setc
setp
sleep
stop
subw ya, 0
tcall 0
tcall 1
tcall 2
tcall 3
tcall 4
tcall 5
tcall 6
tcall 7
tcall 8
tcall 9
tcall 10
tcall 11
tcall 12
tcall 13
tcall 14
tcall 15
tclr 0
tset 0
xcn a