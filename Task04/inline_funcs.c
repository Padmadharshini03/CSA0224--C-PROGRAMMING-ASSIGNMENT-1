/* Task 4 alternative: static inline functions instead of macros */
#include <stdio.h>

static inline int  square_i(int x)        { return x * x; }
static inline int  max_i(int a, int b)    { return (a > b) ? a : b; }
static inline long rupees_to_paise(long r){ return r * 100L; }

int main(void)
{
    int a = 4, i = 5, j = 3;
    printf("square_i(a+1)   = %d  (25, argument evaluated once, type-checked)\n", square_i(a + 1));
    int m = max_i(i++, j);
    printf("max_i(i++, j)   -> m = %d, i = %d  (m=5, i=6: i++ evaluated exactly once)\n", m, i);
    printf("rupees_to_paise(1+1) = %ld\n", rupees_to_paise(1 + 1));
    return 0;
}
