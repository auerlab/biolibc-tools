/***************************************************************************
 *  Description:
 *      Generate chromosome length file as required by kallisto, directly
 *      from the FASTA file
 *
 *  History: 
 *  Date        Name        Modification
 *  2021-11-28  Jason Bacon Begin
 ***************************************************************************/

#include <stdio.h>
#include <sysexits.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <biolibc/fasta.h>

void    usage(char *argv[]);

int     main(int argc,char *argv[])

{
    bl_fasta_t  record;
    char        *p;
    int         status;
    
    switch(argc)
    {
	case 1:
	    break;
	
	default:
	    usage(argv);
    }
    
    bl_fasta_init(&record);
    while ( (status = bl_fasta_read(&record, stdin)) == BL_READ_OK )
    {
	p = BL_FASTA_DESC(&record) + 1;
	printf("%s\t%zu\n", strsep(&p, " \t"), strlen(BL_FASTA_SEQ(&record)));
    }
    if ( status != BL_READ_EOF )
    {
	fprintf(stderr, "%s: Error reading FASTA stream: %s\n",
		argv[0], strerror(errno));
	return EX_DATAERR;
    }
    return EX_OK;
}


void    usage(char *argv[])

{
    fprintf(stderr, "Usage: %s < fasta\n", argv[0]);
    exit(EX_USAGE);
}
