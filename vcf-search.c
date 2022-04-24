/***************************************************************************
 *  Description:
 *      Scan a VCF file for matching chr and position.
 *
 *      This is simply a faster way to search VCFs than awk, perl,
 *      python, etc. scripts, for cases where the added performance
 *      matters.
 *
 *  History: 
 *  Date        Name        Modification
 *  2021-07-18  Jason Bacon Begin
 ***************************************************************************/

#include <stdio.h>
#include <sysexits.h>
#include <string.h>
#include <stdlib.h>
#include <xtend/dsv.h>
#include <biolibc/vcf.h>

void    usage(char *argv[]);

int     main(int argc,char *argv[])

{
    char            *chr, *end;
    unsigned long   pos;
    int             field_mask = BL_VCF_FIELD_ALL; // BL_VCF_FIELD_CHROM|BL_VCF_FIELD_POS;
    bl_vcf_t        vcf_call;

    switch(argc)
    {
	case    3:
	    chr = argv[1];
	    pos = strtoul(argv[2],&end,10);
	    if ( *end != '\0' )
	    {
		fprintf(stderr, "Invalid position: %s\n", argv[2]);
		usage(argv);
	    }
	    break;
	default:
	    usage(argv);
	    return EX_USAGE;
    }
    
    if ( bl_vcf_skip_header(stdin) == NULL )
	return EX_DATAERR;

    bl_vcf_init(&vcf_call);
    
    // FIXME: Switch to multisample calls when implemented
    while ( bl_vcf_read_ss_call(&vcf_call, stdin, field_mask) != BL_READ_EOF )
    {
	if ( (pos == BL_VCF_POS(&vcf_call)) &&
	     (strcmp(chr, BL_VCF_CHROM(&vcf_call)) == 0) )
	    bl_vcf_write_ss_call(&vcf_call, stdout, BL_VCF_FIELD_ALL);
    }
    return EX_OK;
}


void    usage(char *argv[])

{
    fprintf(stderr,"Usage: %s chromosome position < VCF-file\n",argv[0]);
    exit(EX_USAGE);
}
