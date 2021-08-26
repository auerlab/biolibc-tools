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

printf "\n===\nTesting fastx2tsv and fastq-derep.sh...\n"
../Scripts/fastq-derep.sh test.fastq > temp.fastq
if diff correct-uniq.fastq temp.fastq; then
    printf "No differences found, test passed.\n"
    rm -f temp.fastq
else
    printf "Differences found, test failed.\n"
    printf "Check temp.fastq.\n"
fi

printf "\n===\nTesting fastx-derep...\n"
../fastx-derep < test.fastq | ../fastx2tsv | sort -k 2 \
    | awk -F '\t' '{ printf("%s\n%s\n%s\n%s\n", $1, $2, $3, $4) }' > temp.fastq
if diff correct-uniq.fastq temp.fastq; then
    printf "No differences found, test passed.\n"
    rm -f temp.fastq
else
    printf "Differences found, test failed.\n"
    printf "Check temp.fastq.\n"
    more temp.fastq
fi
