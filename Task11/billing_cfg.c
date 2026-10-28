/* Task 11 alternative - tariff read from a configuration file at start-up (instead of #define).
 * Compile: gcc -Wall -Wextra billing_cfg.c -o billing_cfg
 * Run    : ./billing_cfg tariff.cfg 150 350 ...          (prints the bill for each unit count)  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef MAX_SLABS
#define MAX_SLABS 64
#endif

typedef struct {
    long fixed_paise;
    int  n;
    unsigned long limit[MAX_SLABS];    /* cumulative upper limit in units; ULONG_MAX for the last */
    long rate[MAX_SLABS];              /* paise per unit */
} Tariff;

/* returns 0 on success, otherwise a negative code and a message in err */
int load_tariff(const char *path, Tariff *t, char *err, size_t errn)
{
    FILE *fp = fopen(path, "r");
    if (!fp) { snprintf(err, errn, "cannot open %s", path); return -1; }
    char line[200];
    int lineno = 0, have_fixed = 0;
    t->n = 0;
    while (fgets(line, sizeof line, fp)) {
        lineno++;
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '#' || *p == '\n' || *p == '\0') continue;
        long fx; unsigned long lim; long rt;
        if (sscanf(p, "fixed_charge_paise = %ld", &fx) == 1) {
            if (fx < 0) { snprintf(err, errn, "line %d: fixed charge negative", lineno); fclose(fp); return -2; }
            t->fixed_paise = fx; have_fixed = 1;
        } else if (sscanf(p, "slab = %lu , %ld", &lim, &rt) == 2) {
            if (rt < 0) { snprintf(err, errn, "line %d: negative rate", lineno); fclose(fp); return -3; }
            if (t->n >= MAX_SLABS) { snprintf(err, errn, "line %d: too many slabs", lineno); fclose(fp); return -4; }
            if (t->n > 0 && t->limit[t->n - 1] == (unsigned long)-1) { snprintf(err, errn, "line %d: slab after the unlimited slab", lineno); fclose(fp); return -5; }
            if (lim == 0) lim = (unsigned long)-1;
            if (t->n > 0 && lim <= t->limit[t->n - 1]) { snprintf(err, errn, "line %d: limits must increase", lineno); fclose(fp); return -6; }
            t->limit[t->n] = lim; t->rate[t->n] = rt; t->n++;
        } else { snprintf(err, errn, "line %d: not understood: %.30s", lineno, p); fclose(fp); return -7; }
    }
    fclose(fp);
    if (!have_fixed) { snprintf(err, errn, "fixed_charge_paise missing"); return -8; }
    if (t->n == 0 || t->limit[t->n - 1] != (unsigned long)-1) { snprintf(err, errn, "last slab must be unlimited (limit 0)"); return -9; }
    return 0;
}

long bill_paise(const Tariff *t, unsigned long units)
{
    long paise = t->fixed_paise;
    unsigned long lower = 0;
    for (int k = 0; k < t->n && units > lower; k++) {
        unsigned long upper = t->limit[k] < units ? t->limit[k] : units;
        paise += (long)(upper - lower) * t->rate[k];
        lower = t->limit[k];
    }
    return paise;
}

#ifndef NO_MAIN
int main(int argc, char **argv)
{
    if (argc < 3) { fprintf(stderr, "usage: %s tariff.cfg units...\n", argv[0]); return 1; }
    Tariff t; char err[120];
    if (load_tariff(argv[1], &t, err, sizeof err) != 0) { fprintf(stderr, "tariff error: %s\n", err); return 2; }
    printf("tariff loaded: %d slabs, fixed Rs %ld.%02ld\n", t.n, t.fixed_paise / 100, t.fixed_paise % 100);
    for (int i = 2; i < argc; i++) {
        unsigned long u = strtoul(argv[i], NULL, 10);
        long p = bill_paise(&t, u);
        printf("%4lu units -> Rs %ld.%02ld\n", u, p / 100, p % 100);
    }
    return 0;
}
#endif
