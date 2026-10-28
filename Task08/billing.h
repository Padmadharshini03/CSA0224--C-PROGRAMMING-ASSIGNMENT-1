/* billing.h - shared declarations for the multi-file build (input.c, tariff.c, report.c, main.c) */
#ifndef BILLING_H
#define BILLING_H
#include <stddef.h>
#include "tariff.h"

enum parse_status { ST_OK = 0, ST_SYNTAX, ST_RANGE, ST_CATEGORY, ST_FLAG,
                    ST_LESS_NO_ROLL, ST_ROLL_BUT_HIGHER, ST_HOUSE };

typedef struct { long household; meter_t previous, current; char category, rollover; } Record;
typedef struct { unsigned int units; long energy, fixed, total; } Bill;

/* input.c */
enum parse_status parse_line(const char *line, Record *rec);
const char *status_text(enum parse_status st);
/* tariff.c */
Bill make_bill(const Record *r);
/* report.c */
const char *rs(long paise, char *buf, size_t n);
void print_header(void);
void print_bill(int lineno, const Record *r, const Bill *b);
void print_rejected(int lineno, const char *line, enum parse_status st);
void print_summary(int ok, int bad, long long grand_total, long long sum_parts);
#endif
