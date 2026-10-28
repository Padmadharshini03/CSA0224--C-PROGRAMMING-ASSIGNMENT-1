#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "smart.h"

/* ------------------------------------------------------------------ helpers */
uint64_t round_micro_to_paise(uint64_t micro)             /* round half up, integer only */
{
    return (micro + MICRO_PER_PAISE / 2) / MICRO_PER_PAISE;
}

int is_peak_slot(const Tariff *t, uint64_t slot_index)
{
    uint64_t minute = (slot_index * t->interval_min) % MINUTES_PER_DAY;   /* start of the interval */
    return minute >= t->peak_start_min && minute < t->peak_end_min;
}

static int parse_hhmm(const char *s, uint32_t *out)
{
    unsigned h, m; char extra;
    if (sscanf(s, "%u:%u %c", &h, &m, &extra) != 2 || h > 24 || m > 59 || (h == 24 && m != 0)) return -1;
    *out = h * 60u + m;
    return 0;
}

/* ------------------------------------------------------------------ tariff file */
int tariff_load(const char *path, Tariff *t, char *err, size_t errn)
{
    FILE *fp = fopen(path, "r");
    if (!fp) { snprintf(err, errn, "cannot open tariff file %s", path); return -1; }
    memset(t, 0, sizeof *t);
    t->interval_min = 15; t->peak_percent = 100; t->max_interval_wh = 0;
    int have_fixed = 0, have_ps = 0, have_pe = 0;
    char line[200];
    int ln = 0;
#define FAIL(code, ...) do { snprintf(err, errn, __VA_ARGS__); fclose(fp); return (code); } while (0)
    while (fgets(line, sizeof line, fp)) {
        ln++;
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '#' || *p == '\n' || *p == '\r' || *p == '\0') continue;
        unsigned long long a; unsigned long long b; char s[32];
        if (sscanf(p, "fixed_charge_paise = %llu", &a) == 1)        { if (a > 100000000ull) FAIL(-2, "line %d: fixed charge too large", ln); t->fixed_paise = (uint32_t)a; have_fixed = 1; }
        else if (sscanf(p, "interval_minutes = %llu", &a) == 1)     { if (a < 1 || a > MINUTES_PER_DAY || MINUTES_PER_DAY % a) FAIL(-3, "line %d: interval_minutes must divide 1440", ln); t->interval_min = (uint32_t)a; }
        else if (sscanf(p, "peak_start = %31s", s) == 1)            { if (parse_hhmm(s, &t->peak_start_min)) FAIL(-4, "line %d: bad time '%s' (use HH:MM)", ln, s); have_ps = 1; }
        else if (sscanf(p, "peak_end = %31s", s) == 1)              { if (parse_hhmm(s, &t->peak_end_min)) FAIL(-4, "line %d: bad time '%s' (use HH:MM)", ln, s); have_pe = 1; }
        else if (sscanf(p, "peak_percent = %llu", &a) == 1)         { if (a < 100 || a > 1000) FAIL(-5, "line %d: peak_percent must be 100..1000", ln); t->peak_percent = (uint32_t)a; }
        else if (sscanf(p, "max_interval_wh = %llu", &a) == 1)      { if (a < 1 || a > 4000000000ull) FAIL(-6, "line %d: max_interval_wh out of range", ln); t->max_interval_wh = (uint32_t)a; }
        else if (sscanf(p, "slab = %llu , %llu", &a, &b) == 2) {
            if (t->nslabs >= SMART_MAX_SLABS) FAIL(-7, "line %d: too many slabs", ln);
            if (b > 100000) FAIL(-8, "line %d: rate too large", ln);
            if (t->nslabs > 0 && t->slab_limit_wh[t->nslabs - 1] == SMART_UNLIMITED) FAIL(-9, "line %d: slab after the unlimited slab", ln);
            uint64_t lim = (a == 0) ? SMART_UNLIMITED : a * WH_PER_KWH;
            if (t->nslabs > 0 && lim <= t->slab_limit_wh[t->nslabs - 1]) FAIL(-10, "line %d: slab limits must increase", ln);
            t->slab_limit_wh[t->nslabs] = lim; t->slab_rate[t->nslabs] = (uint32_t)b; t->nslabs++;
        } else FAIL(-11, "line %d: not understood: %.30s", ln, p);
    }
    fclose(fp);
#undef FAIL
    if (!have_fixed) { snprintf(err, errn, "fixed_charge_paise missing"); return -12; }
    if (t->nslabs == 0 || t->slab_limit_wh[t->nslabs - 1] != SMART_UNLIMITED) { snprintf(err, errn, "last slab must be unlimited (limit 0)"); return -13; }
    if (have_ps != have_pe) { snprintf(err, errn, "peak_start and peak_end must both be given"); return -14; }
    if (have_ps && t->peak_start_min >= t->peak_end_min) { snprintf(err, errn, "peak_start must be earlier than peak_end"); return -15; }
    if (!have_ps) { t->peak_start_min = t->peak_end_min = 0; t->peak_percent = 100; }
    if (t->max_interval_wh == 0) t->max_interval_wh = 20000;       /* default plausibility limit */
    return 0;
}

/* ------------------------------------------------------------------ readings file */
int readings_load(const char *path, uint32_t **reg, size_t *n, char *err, size_t errn)
{
    FILE *fp = fopen(path, "r");
    if (!fp) { snprintf(err, errn, "cannot open readings file %s", path); return -1; }
    size_t cap = 4096, cnt = 0;
    uint32_t *a = malloc(cap * sizeof *a);
    char line[128]; int ln = 0;
    while (a && fgets(line, sizeof line, fp)) {
        ln++;
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '#' || *p == '\n' || *p == '\r' || *p == '\0') continue;
        char *end; errno = 0;
        if (*p == '-') { snprintf(err, errn, "line %d: negative reading", ln); free(a); fclose(fp); return -2; }
        unsigned long long v = strtoull(p, &end, 10);
        while (*end == ' ' || *end == '\t' || *end == '\r') end++;
        if (end == p || errno == ERANGE || (*end != '\n' && *end != '\0')) { snprintf(err, errn, "line %d: not a number", ln); free(a); fclose(fp); return -3; }
        if (v > UINT32_MAX) { snprintf(err, errn, "line %d: reading exceeds 32-bit register", ln); free(a); fclose(fp); return -4; }
        if (cnt == cap) { cap *= 2; uint32_t *b = realloc(a, cap * sizeof *a); if (!b) { free(a); fclose(fp); snprintf(err, errn, "out of memory"); return -5; } a = b; }
        a[cnt++] = (uint32_t)v;
    }
    fclose(fp);
    if (!a) { snprintf(err, errn, "out of memory"); return -5; }
    *reg = a; *n = cnt;
    return 0;
}

/* ------------------------------------------------------------------ the billing core */
int bill_month(const Tariff *t, const uint32_t *reg, size_t nreg, MonthBill *out, char *err, size_t errn)
{
    memset(out, 0, sizeof *out);
    if (nreg < 2) { snprintf(err, errn, "need at least 2 readings (opening + one interval)"); return -1; }

    uint64_t cum_wh = 0;
    int k = 0;
    for (size_t i = 1; i < nreg; i++) {
        uint32_t wh = (uint32_t)(reg[i] - reg[i - 1]);          /* unsigned 32-bit subtraction handles register roll-over */
        if (wh > t->max_interval_wh) {
            snprintf(err, errn, "interval %zu: consumption %u Wh exceeds plausible maximum %u Wh (meter fault or reading error)",
                     i, wh, t->max_interval_wh);
            return -2;
        }
        int peak = is_peak_slot(t, i - 1);
        uint64_t mult = peak ? t->peak_percent : PERCENT_BASE;
        uint64_t remaining = wh;
        while (remaining > 0) {
            while (k < t->nslabs - 1 && cum_wh >= t->slab_limit_wh[k]) k++;
            uint64_t room = t->slab_limit_wh[k] - cum_wh;        /* Wh left in the current slab */
            uint64_t take = remaining < room ? remaining : room;
            out->slab_wh[k] += take;
            out->base_u     += take * t->slab_rate[k] * PERCENT_BASE;
            out->total_u    += take * t->slab_rate[k] * mult;
            cum_wh    += take;
            remaining -= take;
        }
        out->total_wh += wh;
        if (peak) out->peak_wh += wh; else out->offpeak_wh += wh;
        out->intervals++;
    }
    out->energy_paise    = round_micro_to_paise(out->total_u);
    out->base_paise      = round_micro_to_paise(out->base_u);
    out->surcharge_paise = out->energy_paise - out->base_paise;
    out->fixed_paise     = t->fixed_paise;
    out->total_paise     = out->energy_paise + out->fixed_paise;
    return 0;
}
