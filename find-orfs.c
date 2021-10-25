#include <stdio.h>
#include <sysexits.h>
#include <ctype.h>
#include <biolibc/translate.h>

int     main(int argc,char *argv[])

{
    long    pos = 1,
	    offset;
    char    codon[4];

    if ( (offset = next_start_codon(stdin)) != EOF )
    {
	printf("Start codon AUG at %ld (+%ld)\n", pos + offset, offset);
	pos += offset + 3;
	if ( (offset = next_stop_codon(stdin, codon)) != EOF )
	    printf("Stop codon %s at %ld (+%ld)\n", codon, pos + offset, offset);
	else
	    puts("No stop codon found.");
    }
    else
	puts("No start codon found.");
    return EX_OK;
}
