/* Task 4 - Use macros safely.  Compile: gcc -Wall -Wextra macros.c -o macros */
#include <stdio.h>

/* ---------- WRONG (unparenthesised) versions, kept only to demonstrate the pitfalls ---------- */
#define SQUARE_BAD(x)  x*x
#define MAX_BAD(a, b)  a > b ? a : b
#define RUPEES_TO_PAISE_BAD(r)  r * 100

/* ---------- CORRECT, fully parenthesised versions ---------- */
/* r must be a WHOLE number of rupees; fractional rupees must never be converted by a cast
   (that truncates) - amounts are kept in paise from the start instead.                   */
#define RUPEES_TO_PAISE(r)  ((long)(r) * 100L)
#define SQUARE(x)           ((x) * (x))
#define MAX(a, b)           (((a) > (b)) ? (a) : (b))
#define MIN(a, b)           (((a) < (b)) ? (a) : (b))

int main(void)
{
    int a = 4;
    printf("1) SQUARE_BAD(a+1) with a=4 : %d   (expected 25)\n", SQUARE_BAD(a + 1));
    printf("   SQUARE(a+1)     with a=4 : %d\n", SQUARE(a + 1));

    printf("2) 100 / SQUARE_BAD(2)      : %d   (expected 25)\n", 100 / SQUARE_BAD(2));
    printf("   100 / SQUARE(2)          : %d\n", 100 / SQUARE(2));

    printf("3) RUPEES_TO_PAISE_BAD(1+1) : %d   (expected 200)\n", RUPEES_TO_PAISE_BAD(1 + 1));
    printf("   RUPEES_TO_PAISE(1+1)     : %ld\n", RUPEES_TO_PAISE(1 + 1));

    int x = 3, y = 5;
    printf("4) 10 + MAX_BAD(x,y)        : %d   (expected 15)\n", 10 + MAX_BAD(x, y));
    printf("   10 + MAX(x,y)            : %d\n", 10 + MAX(x, y));

    int i = 5, j = 3, m;
    m = MAX(i++, j);                /* i++ is written once but evaluated TWICE */
    printf("5) m = MAX(i++, j) with i=5, j=3 -> m = %d, i = %d   (expected m=5, i=6)\n", m, i);

    printf("6) RUPEES_TO_PAISE(162)     : %ld paise\n", RUPEES_TO_PAISE(162));
    printf("   MIN(7, 2) = %d, MAX(7, 2) = %d\n", MIN(7, 2), MAX(7, 2));
    return 0;
}
