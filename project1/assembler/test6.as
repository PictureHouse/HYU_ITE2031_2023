        lw      0       1       one
        lw      0       3       three
        lw      0       5       five
        lw      0       7       seven
        lw      0       9       nine    register outside the range [0,7]
        add     1       3       3
        add     3       5       5
        add     5       7       7
        add     7       9       9       register outside the range [0,7]
done    halt
one     .fill   1
three   .fill   3
five    .fill   5
seven   .fill   7
nine    .fill   9
