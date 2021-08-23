/***************************************************************************
 *  Description:
 *      Remove replicates from a fastq stream
 *  
 *  History: 
 *  Date        Name        Modification
 *  2021-07-27  Jason Bacon Begin
 ***************************************************************************/

#include <stdio.h>
#include <sysexits.h>
#include <string.h>
#include <xtend/string.h>
#include <biolibc/fasta.h>
#include <biolibc/fastq.h>
#include <biolibc/biolibc.h>

int     main(int argc, char *argv[])

{
    bl_fasta_t      fasta_rec = BL_FASTA_INIT;
    bl_fastq_t      fastq_rec = BL_FASTQ_INIT;
    unsigned long   records = 0;
    int             ch;

    ch = getc(stdin);
    ungetc(ch, stdin);
    if ( ch == '>' )
    {
	while ( bl_fasta_read(stdin, &fasta_rec) != BL_READ_EOF )
	{
	    // Replace TABs in description to avoid interpretation as separators
	    strtr(BL_FASTQ_DESC(&fasta_rec), "\t", " ", 0);
	    printf("%s\t%s\n", BL_FASTQ_DESC(&fasta_rec), BL_FASTQ_SEQ(&fasta_rec));
	    ++records;
	}
	bl_fasta_free(&fasta_rec);
    }
    else if ( ch == '@' )
    {
	while ( bl_fastq_read(stdin, &fastq_rec) != BL_READ_EOF )
	{
	    // Replace TABs in description to avoid interpretation as separators
	    strtr(BL_FASTQ_DESC(&fastq_rec), "\t", " ", 0);
	    printf("%s\t%s\t%s\t%s\n", BL_FASTQ_DESC(&fastq_rec), BL_FASTQ_SEQ(&fastq_rec),
		    BL_FASTQ_PLUS(&fastq_rec), BL_FASTQ_QUAL(&fastq_rec));
	    ++records;
	}
	bl_fastq_free(&fastq_rec);
    }
    else
    {
	fputs("stdin is neither a FASTA nor a FASTQ file.\n", stderr);
	return EX_DATAERR;
    }
    fprintf(stderr, "%lu records processed.\n", records);
    return EX_OK;
}