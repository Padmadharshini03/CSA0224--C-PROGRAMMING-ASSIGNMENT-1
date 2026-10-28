#!/bin/sh
# Task 8 alternative: compile time after a change to ONE file - single file vs separate compilation.
# usage: sh measure_build.sh F        (F = functions per module; each .c has F lines)
F=${1:-4000}; D=/tmp/claude-0/-home-claude/362b5eb2-929a-5f86-a7b5-77e760b8a54b/scratchpad/big$F
rm -rf $D; python3 gen_modules.py $D $F; cd $D
ms() { echo $(( ($(date +%s%N) - $1) / 1000000 )); }
# ---- (A) one translation unit: concatenate everything
cat input.c tariff.c report.c main.c | grep -v '#include "big.h"' > all_body.c
{ echo '#include <stdio.h>'; cat big.h; cat all_body.c; } > single.c
t=$(date +%s%N); gcc -O1 single.c -o single_exe; A_full=$(ms $t)
echo "// edit" >> single.c
t=$(date +%s%N); gcc -O1 single.c -o single_exe; A_edit=$(ms $t)
# ---- (B) separate compilation
t=$(date +%s%N); for f in input tariff report main; do gcc -O1 -c $f.c -o $f.o; done; gcc *.o -o split_exe; B_full=$(ms $t)
echo "// edit" >> tariff.c
t=$(date +%s%N); gcc -O1 -c tariff.c -o tariff.o; gcc input.o tariff.o report.o main.o -o split_exe; B_edit=$(ms $t)
# ---- (C) header change: everything depends on it
touch big.h
t=$(date +%s%N); for f in input tariff report main; do gcc -O1 -c $f.c -o $f.o; done; gcc *.o -o split_exe; C_hdr=$(ms $t)
S_single=$(size single_exe | awk 'NR==2{print $1+$2}'); S_obj=$(size input.o tariff.o report.o main.o | awk 'NR>1{t+=$1+$2} END{print t}')
O_bytes=$(cat input.o tariff.o report.o main.o | wc -c); E_bytes=$(stat -c %s single_exe)
echo "functions per module F = $F   (source lines per module = $F)"
echo "                                          time (ms)"
echo "single file  : full build                 $A_full"
echo "single file  : after editing 1 function   $A_edit"
echo "separate .c  : full build                 $B_full"
echo "separate .c  : after editing tariff.c     $B_edit   (only tariff.c recompiled + link)"
echo "separate .c  : after editing big.h        $C_hdr   (every file includes it -> all recompiled)"
echo "object files total bytes (4 files)        $O_bytes     single executable bytes $E_bytes"
