/***************************************************************************
 *  Description:
 *      Extract a sequence from a reference genome using info from a GFF.
 *
 *  History: 
 *  Date        Name        Modification
 *  2021-10-28  Jason Bacon Begin
 ***************************************************************************/

#include <stdio.h>
#include <sysexits.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <biolibc/gff.h>
#include <biolibc/fasta.h>

#define KEY_MAX     1024

void    usage(char *argv[]);

int     main(int argc,char *argv[])

{
    FILE        *fasta_stream,
		*gff_stream;
    char        *gff_file,
		*fasta_file,
		*feature_type,
		*feature_name,
		*feature_seq,
		search_key[KEY_MAX],
		*gff_chrom,
		*fasta_chrom;
    int         status;
    size_t      chrom_len,
		feature_len;
    unsigned long   start, end;
    bl_gff_t    gff_feature = BL_GFF_INIT;
    bl_fasta_t  fasta_seq = BL_FASTA_INIT;
    
    switch(argc)
    {
	case 5:
	    gff_file = argv[1];
	    fasta_file = argv[2];
	    feature_type = argv[3];
	    feature_name = argv[4];
	    break;
	
	default:
	    usage(argv);
	    return EX_USAGE;    // Useless but to silence compiler warning
    }
    
    
    // FIXME: Limit field input
    if ( (gff_stream = fopen(gff_file, "r")) == NULL )
    {
	fprintf(stderr, "%s: Cannot open %s: %s\n", argv[0], gff_file,
		strerror(errno));
	return EX_NOINPUT;
    }
    
    // FIXME: Factor this out to a bl_gff_search() function in biolibc
    bl_gff_skip_header(gff_stream);
    // printf("Searching %s for %s %s\n", argv[1], feature_type, feature_name);
    snprintf(search_key, KEY_MAX, "Name=%s;", feature_name);
    while ( (status = bl_gff_read(&gff_feature, BL_GFF_FIELD_ALL, gff_stream))
		== BL_READ_OK )
    {
	if ( (strcmp(BL_GFF_FEATURE(&gff_feature), feature_type) == 0) &&
	     (strstr(BL_GFF_ATTRIBUTES(&gff_feature), search_key) != NULL) )
	{
	    // bl_gff_write(&gff_feature, BL_GFF_FIELD_ALL, stdout);
	    break;
	}
    }
    fclose(gff_stream);
    
    // Extract sequence from FASTA
    if ( status == BL_READ_OK )
    {
	if ( (fasta_stream = fopen(fasta_file, "r")) == NULL )
	{
	    fprintf(stderr, "%s: Cannot open %s: %s\n", argv[0], fasta_file,
		    strerror(errno));
	    return EX_NOINPUT;
	}
	gff_chrom = BL_GFF_SEQUENCE(&gff_feature);
	chrom_len = strlen(gff_chrom);
	start = BL_GFF_START(&gff_feature);
	end = BL_GFF_END(&gff_feature);
	// printf("%s %lu %lu\n", gff_chrom, start, end);
	
	// FIXME: Factor this out to bl_fasta_search() in biolibc
	while ( bl_fasta_read(fasta_stream, &fasta_seq) == BL_READ_OK )
	{
	    fasta_chrom = BL_FASTA_DESC(&fasta_seq) + 1;    // Skip '>'
	    if ( memcmp(fasta_chrom, gff_chrom, chrom_len) == 0 )
	    {
		// puts(BL_FASTA_DESC(&fasta_seq));
		printf(">%s %s:%zu:%zu:%zu %s %s %s\n", gff_chrom, feature_type,
			start, end, feature_len, feature_name,
			gff_file, fasta_file);
		feature_len = end - start + 1;
		feature_seq = BL_FASTA_SEQ(&fasta_seq) + start - 1;
		feature_seq[feature_len] = '\0';
		puts(feature_seq);
	    }
	}
	fclose(fasta_stream);
	return EX_OK;
    }
    else
    {
	fprintf(stderr, "%s: Search key \"%s\" not found in %s.\n",
		argv[0], search_key, gff_file);
	return EX_UNAVAILABLE;
    }
}


void    usage(char *argv[])

{
    fprintf(stderr, "Usage: %s file.gff3 file.fasta feature-type feature-name\n", argv[0]);
    fprintf(stderr, "Example: %s Danio_rerio.GRCz11.104.gff3 Danio_rerio.GRCz11.dna.primary_assembly.fa gene Jun\n", argv[0]);
    exit(EX_USAGE);
}
