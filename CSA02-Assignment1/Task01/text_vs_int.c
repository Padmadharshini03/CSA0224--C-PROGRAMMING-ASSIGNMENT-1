/* Task 1 - Alternative concept: meter readings stored as TEXT instead of integers.
 *
 * Part A shows what the solution looks like with text readings ("01530").
 * Part B measures the cost of converting / comparing d-digit readings and
 *        prints the space used per reading in both representations.
 *
 * Compile: gcc -Wall -Wextra -O1 text_vs_int.c -o text_vs_int
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef unsigned short meter_t;

/* O(d): convert a string of d digits to a number */
static unsigned long long text_to_number(const char *s)
{
    unsigned long long v = 0;
    while (*s) {
        v = v * 10u + (unsigned)(*s - '0');
        s++;
    }
    return v;
}

/* O(d): compare two equal-length zero-padded digit strings
   (lexicographic order == numeric order for equal-length strings)   */
static int text_compare(const char *a, const char *b)
{
    while (*a && *a == *b) {
        a++;
        b++;
    }
    return (unsigned char)*a - (unsigned char)*b;
}

/* Consumption with text readings: two conversions O(d) + O(1) subtraction */
static unsigned consumption_text(const char *prev, const char *curr)
{
    meter_t p = (meter_t)text_to_number(prev);
    meter_t c = (meter_t)text_to_number(curr);
    return (meter_t)(c - p);
}

static double seconds(clock_t a, clock_t b)
{
    return (double)(b - a) / CLOCKS_PER_SEC;
}

int main(void)
{
    /* ---------- Part A : the solution with text readings ---------- */
    const char *prev = "65400", *curr = "00120";
    printf("PART A - readings kept as text\n");
    printf("  prev = \"%s\"  curr = \"%s\"\n", prev, curr);
    printf("  text_compare(curr, prev) = %d  (negative => register rolled over)\n",
           text_compare(curr, prev));
    printf("  consumption_text()       = %u units\n", consumption_text(prev, curr));
    printf("  consumption_text(\"01200\",\"01530\") = %u units\n\n",
           consumption_text("01200", "01530"));

    printf("  Space per reading (d = 5 digits):\n");
    printf("    text   : char[6]  = %zu bytes (5 digits + '\\0')\n", sizeof(char[6]));
    printf("    binary : meter_t  = %zu bytes\n\n", sizeof(meter_t));

    /* ---------- Part B : measured time vs number of digits d ---------- */
    printf("PART B - time per operation (ns) as the number of digits d grows\n");
    printf("%6s %10s %12s %14s %14s %14s\n", "d", "text bytes", "binary bytes",
           "convert (ns)", "cmp text (ns)", "cmp int (ns)");

    static const int digits[] = { 5, 50, 500, 5000 };
    volatile unsigned long long sink = 0;

    for (size_t k = 0; k < sizeof digits / sizeof digits[0]; k++) {
        int d = digits[k];
        long iters = 200000000L / d;      /* same total number of digit steps */
        char *s1 = malloc((size_t)d + 1);
        char *s2 = malloc((size_t)d + 1);
        memset(s1, '1', (size_t)d); s1[d] = '\0';
        memset(s2, '1', (size_t)d); s2[d] = '\0';

        clock_t t0 = clock();
        for (long i = 0; i < iters; i++) {
            s2[d - 1] = (char)('0' + i % 10);
            sink += text_to_number(s2);
        }
        clock_t t1 = clock();
        double conv_ns = seconds(t0, t1) / (double)iters * 1e9;

        t0 = clock();
        for (long i = 0; i < iters; i++) {
            s2[d - 1] = (char)('0' + i % 10);
            sink += (unsigned long long)(unsigned)text_compare(s1, s2);
        }
        t1 = clock();
        double cmp_ns = seconds(t0, t1) / (double)iters * 1e9;

        t0 = clock();
        for (long i = 0; i < iters; i++) {
            unsigned x = (unsigned)i, y = x ^ 3u;
            __asm__ volatile("" : "+r"(y));       /* stop the compiler folding it */
            sink += (x < y);
        }
        t1 = clock();
        double int_ns = seconds(t0, t1) / (double)iters * 1e9;

        int bin_bytes = (int)((d * 3.3219281 + 7.999) / 8.0);   /* ceil(d*log2(10)/8) */
        printf("%6d %10d %12d %14.2f %14.2f %14.2f\n", d, d + 1, bin_bytes,
               conv_ns, cmp_ns, int_ns);
        free(s1);
        free(s2);
    }
    printf("\n(sink = %llu, printed only so the loops are not optimised away)\n", sink);
    return 0;
}
