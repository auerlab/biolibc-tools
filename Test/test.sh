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

printf "\n===\nTesting fastq2tsv and fastq-dedup...\n"
../Scripts/fastq-dedup test.fastq > temp.fastq
if diff correct-uniq.fastq temp.fastq; then
    printf "No differences found, test passed.\n"
    rm -f temp.fastq
else
    printf "Differences found, test failed.\n"
    printf "Check temp.fastq.\n"
fi
