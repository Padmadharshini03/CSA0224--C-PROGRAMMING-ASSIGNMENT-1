#include <stdio.h>
#include <ctype.h>
#include "billing.h"

static const char *texts[] = {
    "ok", "invalid input (not numbers / wrong number of fields)", "reading out of range 0..65535",
    "unknown category (only D is defined)", "rollover flag must be Y or N",
    "current reading is less than previous and no roll-over declared",
    "roll-over declared but current reading is not below previous", "household number must be positive"
};
const char *status_text(enum parse_status st) { return texts[st]; }

enum parse_status parse_line(const char *line, Record *rec)
{
    long house, prev, curr;
    char cat, flag, extra;
    int n = sscanf(line, "%ld %ld %ld %c %c %c", &house, &prev, &curr, &cat, &flag, &extra);
    if (n != 5) return ST_SYNTAX;
    if (house <= 0) return ST_HOUSE;
    if (prev < 0 || prev > METER_MAX_READING || curr < 0 || curr > METER_MAX_READING) return ST_RANGE;
    cat  = (char)toupper((unsigned char)cat);
    flag = (char)toupper((unsigned char)flag);
    if (cat != 'D') return ST_CATEGORY;
    if (flag != 'Y' && flag != 'N') return ST_FLAG;
    if (flag == 'N' && curr < prev) return ST_LESS_NO_ROLL;
    if (flag == 'Y' && curr >= prev) return ST_ROLL_BUT_HIGHER;
    rec->household = house; rec->previous = (meter_t)prev; rec->current = (meter_t)curr;
    rec->category = cat; rec->rollover = flag;
    return ST_OK;
}
