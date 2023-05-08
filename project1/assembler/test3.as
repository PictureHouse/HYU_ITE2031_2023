       lw     0      1      first
       lw     0      2      second
       lw     0      3      one
       beq    1      2      end
       nor    0      1      1
       add    1      3      1
       nor    0      2      2
       add    2      3      2
       sw     0      1      first
       sw     0      2      second
end    halt
first  .fill  3
second .fill  5
one    .fill  1
