/* Task 6 alternative - read each line with fgets(), convert with strtol()/sscanf().
 * Compile: gcc -Wall -Wextra fgets_check.c -o fgets_check                                   */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "tariff.h"

#define LINE_MAX_LEN 256                     /* buffer space: L + 1 bytes for a line of length L */

unsigned int consumption(meter_t previous, meter_t current) { return (meter_t)(current - previous); }
long energy_charge(unsigned int units)
{
    long p = 0;
    if (units > SLAB2_UNITS) p += (long)(units - SLAB2_UNITS) * RATE3_PAISE;
    if (units > SLAB1_UNITS) p += (long)((units > SLAB2_UNITS ? SLAB2_UNITS : units) - SLAB1_UNITS) * RATE2_PAISE;
    return p + (long)(units > SLAB1_UNITS ? SLAB1_UNITS : units) * RATE1_PAISE;
}
long total_bill(unsigned int units) { return FIXED_CHARGE_PAISE + energy_charge(units); }

/* parse one long with full error detection; returns 0 on success */
static int parse_long(char **p, long *out)
{
    char *end;
    errno = 0;
    long v = strtol(*p, &end, 10);
    if (end == *p || errno == ERANGE) return -1;      /* no digits, or overflow */
    *p = end;
    *out = v;
    return 0;
}

int main(void)
{
    char buf[LINE_MAX_LEN];
    int line = 0, ok = 0, bad = 0;
    while (fgets(buf, sizeof buf, stdin)) {          /* the whole line is consumed: no stuck input */
        line++;
        char *p = buf;
        long house, prev, curr;
        if (parse_long(&p, &house) || parse_long(&p, &prev) || parse_long(&p, &curr)) {
            printf("line %d: INVALID INPUT - line skipped\n", line);
            bad++; continue;
        }
        while (*p == ' ' || *p == '\t') p++;
        if (*p != '\n' && *p != '\0') {              /* trailing garbage such as "150abc" */
            printf("line %d: INVALID INPUT (extra characters) - line skipped\n", line);
            bad++; continue;
        }
        if (prev < 0 || prev > METER_MAX_READING || curr < 0 || curr > METER_MAX_READING) {
            printf("line %d: reading out of range 0..%d - line skipped\n", line, METER_MAX_READING);
            bad++; continue;
        }
        unsigned int u = consumption((meter_t)prev, (meter_t)curr);
        long t = total_bill(u);
        printf("line %d: house %ld units %u -> Rs %ld.%02ld\n", line, house, u, t / 100, t % 100);
        ok++;
    }
    printf("\n%d valid, %d rejected\n", ok, bad);
    return 0;
}
