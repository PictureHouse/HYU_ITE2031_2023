        lw      0       1       five
        lw      1       2       3
        add     1       2       1       
        beq     0       1       2
        beq     0       0       start   use of undefined label
        noop
done    halt
five    .fill   5
neg1    .fill   -1
stAddr  .fill   start
