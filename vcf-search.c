/***************************************************************************
 *  Description:
 *      Scan a VCF file for matching chr and position
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
    FILE            *vcf_meta_stream;

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
    
    if ( bl_vcf_skip_meta_data(stdin, &vcf_meta_stream) != BL_READ_OK )
	return EX_DATAERR;
    // Discard header line (#CHROM ...)
    dsv_skip_rest_of_line(stdin);
    bl_vcf_init(&vcf_call, 1024, 1024, 1024);
    
    // FIXME: Switch to multisample calls when implemented
    while ( bl_vcf_read_ss_call(stdin, &vcf_call, field_mask) != BL_READ_EOF )
    {
	if ( (pos == BL_VCF_POS(&vcf_call)) &&
	     (strcmp(chr, BL_VCF_CHROM(&vcf_call)) == 0) )
	{
	    bl_vcf_write_ss_call(stdout, &vcf_call, BL_VCF_FIELD_ALL);
	}
    }
    return EX_OK;
}


void    usage(char *argv[])

{
    fprintf(stderr,"Usage: %s chromosome position < VCF-file\n",argv[0]);
    exit(EX_USAGE);
}
