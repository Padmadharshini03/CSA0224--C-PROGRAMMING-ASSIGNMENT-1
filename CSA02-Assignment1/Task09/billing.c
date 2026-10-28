/* Grama Jyothi Rural Electricity Co-operative - monthly billing program (Task 9)
 *
 * Input  (stdin or a file), one household per line:
 *        household_no  previous_reading  current_reading  category  rollover_flag
 *        e.g.   104   65400   124   D   Y
 *          category      : D = domestic (the only tariff defined in the assignment)
 *          rollover_flag : Y = the meter register passed 65535 -> 0 during the month
 *                          N = it did not.  (The meter reader records it; without it a
 *                              current reading below the previous one could not be told
 *                              apart from a wrong entry.)
 *        Blank lines and lines starting with '#' are ignored.
 *
 * Output : one bill per valid line (units, energy charge, fixed charge, total, all exact to
 *          the paisa) and a summary.  Invalid lines are reported with the reason and skipped.
 *
 * Compile: gcc -Wall -Wextra billing.c -o billing          (add -DDEBUG for trace output)
 * Run    : ./billing < readings.txt                                                        */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "tariff.h"

#define LINE_LEN 256

/* ------------------------------------------------------------------ error codes */
enum parse_status {
    ST_OK = 0,
    ST_SYNTAX,          /* letters, missing or extra fields                    */
    ST_RANGE,           /* reading negative or above 65535                     */
    ST_CATEGORY,        /* unknown category                                    */
    ST_FLAG,            /* rollover flag not Y or N                            */
    ST_LESS_NO_ROLL,    /* current < previous but no roll-over declared        */
    ST_ROLL_BUT_HIGHER, /* roll-over declared but current >= previous          */
    ST_HOUSE            /* household number not positive                       */
};
static const char *status_text[] = {
    "ok",
    "invalid input (not numbers / wrong number of fields)",
    "reading out of range 0..65535",
    "unknown category (only D is defined)",
    "rollover flag must be Y or N",
    "current reading is less than previous and no roll-over declared",
    "roll-over declared but current reading is not below previous",
    "household number must be positive"
};

typedef struct {
    long   household;
    meter_t previous, current;
    char   category, rollover;
} Record;

typedef struct {
    unsigned int units;
    long energy, fixed, total;       /* all in paise */
} Bill;

/* ------------------------------------------------------------------ billing logic */
unsigned int consumption(meter_t previous, meter_t current)
{
    return (meter_t)(current - previous);          /* 16-bit unsigned arithmetic handles roll-over */
}

long energy_charge(unsigned int units)
{
    long paise = 0;
    if (units > SLAB2_UNITS)
        paise += (long)(units - SLAB2_UNITS) * RATE3_PAISE;
    if (units > SLAB1_UNITS)
        paise += (long)((units > SLAB2_UNITS ? SLAB2_UNITS : units) - SLAB1_UNITS) * RATE2_PAISE;
    paise += (long)(units > SLAB1_UNITS ? SLAB1_UNITS : units) * RATE1_PAISE;
    DBG("energy_charge: units=%u -> %ld paise\n", units, paise);
    return paise;
}

long total_bill(unsigned int units)
{
    return FIXED_CHARGE_PAISE + energy_charge(units);
}

static Bill make_bill(const Record *r)
{
    Bill b;
    b.units  = consumption(r->previous, r->current);
    b.energy = energy_charge(b.units);
    b.fixed  = FIXED_CHARGE_PAISE;
    b.total  = b.fixed + b.energy;
    return b;
}

/* ------------------------------------------------------------------ input handling */
static enum parse_status parse_line(const char *line, Record *rec)
{
    long house, prev, curr;
    char cat, flag, extra;
    int n = sscanf(line, "%ld %ld %ld %c %c %c", &house, &prev, &curr, &cat, &flag, &extra);
    if (n != 5)
        return ST_SYNTAX;                          /* 6 would mean trailing garbage */
    if (house <= 0)
        return ST_HOUSE;
    if (prev < 0 || prev > METER_MAX_READING || curr < 0 || curr > METER_MAX_READING)
        return ST_RANGE;
    cat  = (char)toupper((unsigned char)cat);
    flag = (char)toupper((unsigned char)flag);
    if (cat != 'D')
        return ST_CATEGORY;
    if (flag != 'Y' && flag != 'N')
        return ST_FLAG;
    if (flag == 'N' && curr < prev)
        return ST_LESS_NO_ROLL;
    if (flag == 'Y' && curr >= prev)
        return ST_ROLL_BUT_HIGHER;
    rec->household = house;
    rec->previous  = (meter_t)prev;
    rec->current   = (meter_t)curr;
    rec->category  = cat;
    rec->rollover  = flag;
    return ST_OK;
}

/* format paise as "rupees.paise" into buf, e.g. 16250 -> "162.50" */
static const char *rs(long paise, char *buf, size_t n)
{
    snprintf(buf, n, "%ld.%02ld", paise / 100, paise % 100);
    return buf;
}

int main(void)
{
    char line[LINE_LEN];
    int  lineno = 0, ok = 0, bad = 0;
    long long grand_total = 0, sum_parts = 0;

    printf("%-6s %-9s %-6s %-12s %-10s %-12s\n", "Line", "House", "Units", "Energy(Rs)", "Fixed(Rs)", "Total(Rs)");
    while (fgets(line, sizeof line, stdin)) {
        lineno++;
        const char *p = line;
        while (isspace((unsigned char)*p)) p++;
        if (*p == '\0' || *p == '#')
            continue;

        Record rec;
        enum parse_status st = parse_line(line, &rec);
        if (st != ST_OK) {
            line[strcspn(line, "\r\n")] = '\0';
            printf("%-6d REJECTED: %s  [\"%s\"]\n", lineno, status_text[st], line);
            bad++;
            continue;
        }
        Bill b = make_bill(&rec);
        char e[24], f[24], t[24];
        printf("%-6d %-9ld %-6u %-12s %-10s %-12s\n", lineno, rec.household, b.units,
               rs(b.energy, e, sizeof e), rs(b.fixed, f, sizeof f), rs(b.total, t, sizeof t));
        grand_total += b.total;
        sum_parts   += b.energy + b.fixed;
        ok++;
    }
    char g[32], c[32];
    printf("\nBills issued : %d\nRejected     : %d\n", ok, bad);
    printf("Total billed : Rs %s\n", rs((long)grand_total, g, sizeof g));
    printf("Check (sum of energy + fixed parts) : Rs %s   %s\n", rs((long)sum_parts, c, sizeof c),
           grand_total == sum_parts ? "MATCH" : "MISMATCH");
    return bad ? 2 : 0;
}
