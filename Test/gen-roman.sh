#!/bin/sh -e

for file in ~/Prog/Src/fasda/Test/Data/04-reference/Saccharomyces_cerevisiae.R64-1-1.106.chromosome.*.gff3.gz; do
    zgrep -v '^#' $file | head -1
done


