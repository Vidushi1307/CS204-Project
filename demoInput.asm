lw x3, 0(x31)
add x1, x2, x3
jal x1, -8
add x1, x2, x3
sub x2, x3, x4
mul x3, x4, x5
div x4, x5, x6
rem x5, x6, x7
and x6, x7, x8
or x7, x8, x9
slt x8, x9, x10
sll x9, x10, x11
srl x10, x11, x12
sra x11, x12, x13
xor x12, x13, x14
.data
.word 10 20 40 69 -1
.byte 10 20 40 69 -1
.string "myString"
.half 10 20 40 69 -1
.double 10 20 40 69 -1
