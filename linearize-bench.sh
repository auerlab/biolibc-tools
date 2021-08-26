#!/bin/sh -e

fasta=Homo_sapiens.GRCh38.dna.primary_assembly.fa
fastq=realbig.fastq
make clean all
printf "\n===\n"

printf "\ntime wc -l $fasta\n"
time wc -l $fasta

# /dev/null

printf "\ntime cat $fasta > /dev/null\n"
time cat $fasta > /dev/null

printf "\ntime fastx2tsv < $fasta > /dev/null\n"
time ./fastx2tsv < $fasta > /dev/null

printf "\ntime seqkit seq -w 0 $fasta > /dev/null\n"
time seqkit seq -w 0 $fasta > /dev/null

printf "\ntime seqtk seq $fasta > /dev/null\n"
time seqtk seq $fasta > /dev/null

# SSD

printf "\ntime cat $fasta > cat.fa\n"
time cat $fasta > cat.fa

printf "\ntime fastx2tsv < $fasta > fastx2tsv.tsv\n"
time ./fastx2tsv < $fasta > fastx2tsv.tsv

printf "\ntime seqkit seq -w 0 $fasta > seqkit.fa\n"
time seqkit seq -w 0 $fasta > seqkit.fa

printf "\ntime seqtk seq $fasta > seqtk.fa\n"
time seqtk seq $fasta > seqtk.fa
