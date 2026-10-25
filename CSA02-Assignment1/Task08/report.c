#include <stdio.h>
#include "billing.h"

const char *rs(long paise, char *buf, size_t n)
{
    snprintf(buf, n, "%ld.%02ld", paise / 100, paise % 100);
    return buf;
}
void print_header(void)
{
    printf("%-6s %-9s %-6s %-12s %-10s %-12s\n", "Line", "House", "Units", "Energy(Rs)", "Fixed(Rs)", "Total(Rs)");
}
void print_bill(int lineno, const Record *r, const Bill *b)
{
    char e[24], f[24], t[24];
    printf("%-6d %-9ld %-6u %-12s %-10s %-12s\n", lineno, r->household, b->units,
           rs(b->energy, e, sizeof e), rs(b->fixed, f, sizeof f), rs(b->total, t, sizeof t));
}
void print_rejected(int lineno, const char *line, enum parse_status st)
{
    printf("%-6d REJECTED: %s  [\"%s\"]\n", lineno, status_text(st), line);
}
void print_summary(int ok, int bad, long long grand_total, long long sum_parts)
{
    char g[32], c[32];
    printf("\nBills issued : %d\nRejected     : %d\n", ok, bad);
    printf("Total billed : Rs %s\n", rs((long)grand_total, g, sizeof g));
    printf("Check (sum of energy + fixed parts) : Rs %s   %s\n", rs((long)sum_parts, c, sizeof c),
           grand_total == sum_parts ? "MATCH" : "MISMATCH");
}
