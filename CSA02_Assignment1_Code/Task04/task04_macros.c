#include <stdio.h>

#define RUPEES_TO_PAISE(rs) ((long)((rs) * 100.0 + 0.5))
#define SQUARE_UNSAFE(x)    x * x
#define SQUARE_SAFE(x)      ((x) * (x))
#define MAX_UNSAFE(a, b)    ((a) > (b) ? (a) : (b))
#define MAX_SAFE(a, b)      ((a) > (b) ? (a) : (b))

static inline int max_inline(int a, int b)
{
    return (a > b) ? a : b;
}

int main(void)
{
    int a = 4;
    printf("RUPEES_TO_PAISE(162.50) = %ld paise\n", RUPEES_TO_PAISE(162.50));

    /* 1. Operator precedence pitfall */
    printf("SQUARE_UNSAFE(4 + 1) = %d (Expected 25, evaluated 4 + 1*4 + 1 = 9)\n",
           SQUARE_UNSAFE(a + 1));
    printf("SQUARE_SAFE(4 + 1)   = %d (Evaluated (4 + 1) * (4 + 1) = 25)\n",
           SQUARE_SAFE(a + 1));

    /* 2. Double-evaluation side-effect pitfall */
    int i1 = 5, j1 = 3;
    int m1 = MAX_UNSAFE(i1++, j1);
    printf("MAX_UNSAFE(5++, 3)   = %d, i1 became %d (incremented twice!)\n", m1, i1);

    int i2 = 5, j2 = 3;
    int m2 = max_inline(i2++, j2);
    printf("max_inline(5++, 3)   = %d, i2 became %d (incremented once safely)\n", m2, i2);

    return 0;
}
