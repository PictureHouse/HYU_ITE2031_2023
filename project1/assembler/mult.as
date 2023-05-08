        lw      0       2       mcand
        lw      0       3       mplier
        lw      0       4       tmp
        lw      0       5       maxbit
loop    nor     3       4       6
        beq     0       6       result
        beq     0       0       next
result  add     1       2       1
next    lw      0       6       neg1
        add     5       6       5
        beq     0       5       done
        add     2       2       2
        add     4       4       4
        lw      0       6       one
        add     4       6       4       
        beq     0       0       loop
done    halt
mcand   .fill   32766
mplier  .fill   12328
maxbit  .fill   15
one     .fill   1
neg1    .fill   -1
tmp     .fill   -2
