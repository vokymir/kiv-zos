# zkousim si veci z prednasky

je to hlavne forkovani, jak to tak nejak funguje

## zajimavy exponencialni

kdyz for-looopuju a pokazdy fork, tak je to 2^(n+1) procesu,
kde n je strop v loopu (while: i < n)

pustit takhle:
> gcc fork_exp.c -o fork
> ./fork | sort | nl

a hned je videt, kolik radek - je to proste binarni strom.
