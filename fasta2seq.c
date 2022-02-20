/***************************************************************************
 *  Description:
 *      Convert a FASTA file to one continuous sequence
 *
 *  History: 
 *  Date        Name        Modification
 *  2021-10-25  Jason Bacon Begin
 ***************************************************************************/

#include <stdio.h>
#include <sysexits.h>
#include <biolibc/fasta.h>

int     main(int argc,char *argv[])

{
    bl_fasta_t  fasta_rec;
    
    bl_fasta_init(&fasta_rec);
    while ( bl_fasta_read(&fasta_rec, stdin) != EOF )
	printf("%s", BL_FASTA_SEQ(&fasta_rec));
    putchar('\n');
    return EX_OK;
}

