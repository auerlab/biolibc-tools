/***************************************************************************
 *  Description:
 *      Downsample a VCF file by selecting N evenly distributed sites
 *      at random.
 *
 *  As it stands, it would suffice to use fgets() in place of
 *  bl_vcf_read_ss_call(), but we may want to add filtering options in
 *  the future that require knowledge of the calls.
 *
 *  History: 
 *  Date        Name        Modification
 *  2022-04-22  Jason Bacon Begin
 ***************************************************************************/

#include <stdio.h>
#include <sysexits.h>
#include <stdlib.h>
#include <time.h>
#include <biolibc/vcf.h>

void    usage(char *argv[]);

int     main(int argc,char *argv[])

{
    bl_vcf_t    site;
    int64_t     desired_sites, original_sites;
    char        *end;
    FILE        *tmp_stream;
    int         ch;
    off_t       start_pos;
    long        random_cutoff;
    
    switch(argc)
    {
	case 2:
	    desired_sites = strtoul(argv[1], &end, 10);
	    if ( *end != '\0' )
	    {
		fprintf(stderr, "Invalid site count: %s\n", argv[1]);
		usage(argv);
	    }
	    break;
	
	default:
	    usage(argv);
    }
    
    bl_vcf_init(&site);
    
    if ( (tmp_stream = bl_vcf_skip_header(stdin)) == NULL )
    {
	fprintf(stderr, "vcf-downsample: Error reading VCF header.\n");
	return EX_NOINPUT;
    }
    
    // FIXME: Factor this out to a bl_vcf_t function.
    while ( (ch = getc(tmp_stream)) != EOF )
	putc(ch, stdout);
    
    // Copy header line
    while ( ((ch = getc(stdin)) != '\n') && (ch != EOF) )
	putc(ch, stdout);
    putc('\n', stdout);
    
    // Count sites in VCF and copy input to a seekable file
    start_pos = ftello(tmp_stream);
    original_sites = 0;
    while ( (ch = getc(stdin)) != EOF )
    {
	putc(ch, tmp_stream);
	if ( ch == '\n' )
	    ++original_sites;
    }
    
    random_cutoff = RAND_MAX * desired_sites / original_sites;
    srandom(time(NULL));
    
    fseeko(tmp_stream, start_pos, SEEK_SET);
    while ( bl_vcf_read_ss_call(&site, tmp_stream, BL_VCF_FIELD_ALL) == BL_READ_OK )
    {
	if ( random() < random_cutoff )
	    bl_vcf_write_ss_call(&site, stdout, BL_VCF_FIELD_ALL);
    }
    fclose(tmp_stream);
    
    return EX_OK;
}


void    usage(char *argv[])

{
    fprintf(stderr, "Usage: %s desired-count [< in.vcf] [> out.vcf]\n", argv[0]);
    exit(EX_USAGE);
}
