/* Task 6 - deliberately WRONG format specifiers (undefined behaviour).
 * Compile WITH warnings to see what gcc -Wall reports:  gcc -Wall -Wextra wrong_specifier.c -o wrong_specifier */
#include <stdio.h>

int main(void)
{
    long      l  = 3000000000L;
    int       i  = -42;
    double    d  = 162.5;
    long long ll = 9000000000000000000LL;
    unsigned  u  = 7;

    printf("1) long 3000000000 with %%d     : %d\n", l);        /* 64-bit value, 32-bit specifier */
    printf("2) int -42 with %%u             : %u\n", i);         /* sign reinterpreted             */
    printf("3) double 162.5 with %%d        : %d\n", d);         /* wrong register / garbage       */
    printf("4) long long 9e18 with %%ld     : %ld\n", ll);       /* same size here, NOT portable   */
    printf("5) unsigned 7 with %%f          : %f\n", u);         /* garbage                        */
    printf("6) missing argument for %%d     : %d\n");            /* reads whatever is there        */
    return 0;
}
