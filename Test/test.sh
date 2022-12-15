#!/bin/sh -e

pause()
{
    local junk
    
    printf "Press return to continue..."
    read junk
}

cd ..
./cave-man-install.sh
cd Test
export PATH=..:$PATH

../fastx-stats test.fastq
../fastx-stats test.fasta
pause

printf "\n===\nTesting fastx2tsv and fastq-derep.sh...\n"
../Scripts/fastq-derep.sh test.fastq > temp.fastq
if diff correct-uniq.fastq temp.fastq; then
    printf "No differences found, test passed.\n"
    rm -f temp.fastq
else
    printf "Differences found, test failed.\n"
    printf "Check temp.fastq.\n"
    pause
    more temp.fastq
fi
pause

printf "\n===\nTesting fastx-derep...\n"
../fastx-derep < test.fastq | ../fastx2tsv | sort -k 2 \
    | awk -F '\t' '{ printf("%s\n%s\n%s\n%s\n", $1, $2, $3, $4) }' > temp.fastq
if diff correct-uniq.fastq temp.fastq; then
    printf "No differences found, test passed.\n"
    rm -f temp.fastq
else
    printf "Differences found, test failed.\n"
    printf "Check temp.fastq.\n"
    pause
    more temp.fastq
fi
pause

printf "\n===\nTesting fasta2seq...\n"
../fasta2seq < test.fasta > temp.seq
if diff correct.seq temp.seq; then
    printf "No differences found, test passed.\n"
else
    printf "Differences found, test failed.\n"
    printf "Check temp.seq.\n"
    pause
    more temp.seq
fi
pause

printf "\n===\nTesting find-orfs...\n"
../find-orfs 0 < temp.seq > temp.orfs
if diff correct.orfs temp.orfs; then
    printf "No differences found, test passed.\n"
    rm -f temp.seq temp.orfs
else
    printf "Differences found, test failed.\n"
    printf "Check temp.orfs.\n"
    pause
    more temp.orfs
fi
pause

printf "\n===\nTesting vcf-search...\n"
../vcf-search chr1 4580 < test.vcf > temp.vcf
if diff correct-search.vcf temp.vcf; then
    printf "No differences found, test passed.\n"
    rm -f temp.vcf
else
    printf "Differences found, test failed.\n"
    printf "Check temp.vcf.\n"
    pause
    more temp.vcf
fi

printf "\n===\nTesting vcf-downsample...\n"
../vcf-downsample 50 < test.vcf > temp.vcf
wc -l test.vcf temp.vcf
more temp.vcf
rm -i temp.vcf

printf "\n===\nTesting deromanize...\n"
../deromanize 1 roman.gff3
