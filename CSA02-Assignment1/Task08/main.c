#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "billing.h"

int main(void)
{
    char line[256];
    int lineno = 0, ok = 0, bad = 0;
    long long grand_total = 0, sum_parts = 0;
    print_header();
    while (fgets(line, sizeof line, stdin)) {
        lineno++;
        const char *p = line;
        while (isspace((unsigned char)*p)) p++;
        if (*p == '\0' || *p == '#') continue;
        Record rec;
        enum parse_status st = parse_line(line, &rec);
        if (st != ST_OK) {
            line[strcspn(line, "\r\n")] = '\0';
            print_rejected(lineno, line, st);
            bad++;
            continue;
        }
        Bill b = make_bill(&rec);
        print_bill(lineno, &rec, &b);
        grand_total += b.total;
        sum_parts   += b.energy + b.fixed;
        ok++;
    }
    print_summary(ok, bad, grand_total, sum_parts);
    return bad ? 2 : 0;
}
