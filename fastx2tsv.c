/***************************************************************************
 *  Description:
 *      Convert a FASTA or FASTQ stream to TSV (tab separated values),
 *      combining split sequences into a single line.
 *
 *      This makes it easy to search FASTA files using simple awk commands,
 *      and similar.
 *  
 *  History: 
 *  Date        Name        Modification
 *  2021-07-27  Jason Bacon Begin
 ***************************************************************************/

#include <stdio.h>
#include <sysexits.h>
#include <string.h>
#include <xtend/string.h>
#include <biolibc/fastx.h>
#include <biolibc/biolibc.h>

int     main(int argc, char *argv[])

{
    bl_fastx_t      rec = BL_FASTX_INIT;
    unsigned long   records = 0;

    /*
     *  FIXME: This runs about as fast as
     *  while ( (ch = getc(stdin)) != EOF )
     *      putc(ch, stdout);
     *  which is pretty fast, but much slower than seqkit and seqtk.
     *  Explore other options using faster I/O
     */

    fputs("\nBe aware than fastx2tsv replaces TABs with spaces in the description\n"
	  "so that they won't be interpreted as separators.\n\n", stderr);

    bl_fastx_init(&rec, stdin);
    while ( bl_fastx_read(&rec, stdin) != BL_READ_EOF )
    {
	// Replace TABs in description to avoid interpretation as separators
	xt_strtr(bl_fastx_desc(&rec), "\t", " ", 0);
	switch(BL_FASTX_FORMAT(&rec))
	{
	    case    BL_FASTX_FORMAT_FASTA:
		printf("%s\t%s\n",
			bl_fastx_desc(&rec), bl_fastx_seq(&rec));
			break;
	    case    BL_FASTX_FORMAT_FASTQ:
		printf("%s\t%s\t%s\t%s\n",
			bl_fastx_desc(&rec), bl_fastx_seq(&rec),
			bl_fastx_plus(&rec), bl_fastx_qual(&rec));
			break;
	}
	++records;
    }
    bl_fastx_free(&rec);
    fprintf(stderr, "%lu records processed.\n", records);
    return EX_OK;
}
