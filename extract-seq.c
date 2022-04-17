/***************************************************************************
 *  Description:
 *      Extract the sequences of a GFF feature and all of its subfeatures
 *      from a reference genome in FASTA format.
 *
 *      If the feature type is "gene" or "mRNA", a recursive search of the
 *      GFF structure for subfeatures is automatically performed.
 *
 *  History: 
 *  Date        Name        Modification
 *  2021-10-28  Jason Bacon Begin
 ***************************************************************************/

#ifdef __linux__
#define _GNU_SOURCE // strcasestr()
#endif

#include <stdio.h>
#include <sysexits.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <biolibc/gff.h>
#include <biolibc/fasta.h>
#include <xtend/file.h>

#define KEY_MAX     1024

int     print_subfeatures(bl_fasta_t *fasta_rec, bl_gff_t *feature,
			  FILE *gff_stream, const char *description);
void    print_seq(char *feature_seq, int64_t start, int64_t end);
void    usage(char *argv[]);

int     main(int argc,char *argv[])

{
    FILE        *fasta_stream,
		*gff_stream;
    char        *gff_file,
		*fasta_file,
		*primary_feature_type,
		*search_key,
		*description,
		*gff_chrom,
		*fasta_chrom;
    int         status;
    size_t      chrom_len;
    unsigned long   start, end;
    bl_gff_t    feature;
    bl_fasta_t  fasta_rec = BL_FASTA_INIT;
    bool        found_chrom;

    switch(argc)
    {
	case 5:
	    gff_file = argv[1];
	    fasta_file = argv[2];
	    primary_feature_type = argv[3];
	    search_key = argv[4];
	    break;
	
	default:
	    usage(argv);
	    return EX_USAGE;    // Useless but to silence compiler warning
    }
    
    // FIXME: Limit field input
    if ( (gff_stream = xt_fopen(gff_file, "r")) == NULL )
    {
	fprintf(stderr, "%s: Cannot open %s: %s\n", argv[0], gff_file,
		strerror(errno));
	return EX_NOINPUT;
    }

    // FIXME: Call this automatically from bl_gff_read() if needed?
    bl_gff_skip_header(gff_stream);

    // FIXME: Factor this out to a bl_gff_search() function in biolibc
    // FIXME: Collect a list of GFF features, not just one, and check them
    // all against each FASTA record
    // printf("Searching %s for %s %s\n", argv[1], primary_feature_type, feature_name);
    bl_gff_init(&feature);
    description = search_key;
    while ( (status = bl_gff_read(&feature, gff_stream, BL_GFF_FIELD_ALL))
		== BL_READ_OK )
    {
	if ( (strcasecmp(BL_GFF_TYPE(&feature), primary_feature_type) == 0) &&
	     (strcasestr(BL_GFF_ATTRIBUTES(&feature), search_key) != NULL) )
	{
	    gff_chrom = BL_GFF_SEQID(&feature);
	    chrom_len = strlen(gff_chrom);
	    start = BL_GFF_START(&feature);
	    end = BL_GFF_END(&feature);
	    // printf("%s %lu %lu\n", gff_chrom, start, end);
	    
	    // Search key may be "Name=part-of-gene".  Put more
	    // specific info in description if possible.
	    if ( (memcmp(search_key, "Name=", 5) == 0) &&
		 (strcmp(BL_GFF_TYPE(&feature), "gene") == 0) )
	    {
		description = strdup(BL_GFF_FEATURE_NAME(&feature));
		if ( description == NULL )
		{
		    fprintf(stderr, "extract-seq: Could not strdup() description.\n");
		    return EX_UNAVAILABLE;
		}
	    }
	    
	    printf(">%s:%s %s %" PRId64 " %" PRId64 " %s %s %s %s %s\n",
		    description, BL_GFF_TYPE(&feature),
		    gff_chrom, start, end, primary_feature_type, search_key, 
		    BL_GFF_ATTRIBUTES(&feature), gff_file, fasta_file);

	    // GFFs are sorted lexically and FASTAs numerically
	    // so for now we re-read the FASTA from the beginning for
	    // each hit.
	    if ( (fasta_stream = xt_fopen(fasta_file, "r")) == NULL )
	    {
		fprintf(stderr, "%s: Cannot open %s: %s\n", argv[0], fasta_file,
			strerror(errno));
		return EX_NOINPUT;
	    }
    
	    // FIXME: Factor this out to bl_fasta_search() in biolibc?
	    found_chrom = false;
	    while ( !found_chrom &&
		    (bl_fasta_read(&fasta_rec, fasta_stream) == BL_READ_OK) )
	    {
		fasta_chrom = BL_FASTA_DESC(&fasta_rec) + 1;    // Skip '>'
		found_chrom = memcmp(fasta_chrom, gff_chrom, chrom_len) == 0;
		if ( found_chrom )
		{
		    // puts(BL_FASTA_DESC(&fasta_rec));
		    print_seq(BL_FASTA_SEQ(&fasta_rec), start, end);
		    
		    // Process subfeatures
		    // FIXME: Other feature types with subfeatures?
		    if ( (strcmp(primary_feature_type, "gene") == 0) ||
			 (strcmp(primary_feature_type, "transcript") == 0) )
			print_subfeatures(&fasta_rec, &feature, gff_stream,
					  description);
		}
	    }
	    fclose(fasta_stream);
	}
    }
    fclose(gff_stream);
    
    return EX_OK;
}


/***************************************************************************
 *  Description:
 *      Recursively output subfeatures of the given feature
 *
 *  History: 
 *  Date        Name        Modification
 *  2022-04-12  Jason Bacon Begin
 ***************************************************************************/

int     print_subfeatures(bl_fasta_t *fasta_rec, bl_gff_t *feature,
			  FILE *gff_stream, const char *description)

{
    int     status;
    char    *parent_feature_type,
	    *parent_feature_id;

    // Recursive calls will overwrite feature, so save information needed
    // by this level
    parent_feature_type = strdup(BL_GFF_TYPE(feature));
    parent_feature_id = strdup(BL_GFF_FEATURE_ID(feature));
    
    // Read everything to next feature of the same type, i.e. all subfeatures
    // of a gene or transcript
    status = bl_gff_read(feature, gff_stream, BL_GFF_FIELD_ALL);
    while ( (status == BL_READ_OK) &&
	    (strcmp(BL_GFF_FEATURE_PARENT(feature), parent_feature_id) == 0) )
    {
	printf(">%s:%s %s %" PRId64 " %" PRId64 " %s %s\n",
		description, BL_GFF_TYPE(feature),
		BL_GFF_SEQID(feature),
		BL_GFF_START(feature),
		BL_GFF_END(feature),
		BL_GFF_TYPE(feature),
		BL_GFF_ATTRIBUTES(feature));
	
	print_seq(BL_FASTA_SEQ(fasta_rec),
		  BL_GFF_START(feature), BL_GFF_END(feature));
	
	// Recurse from gene -> transcript -> exon, etc.
	if ( (strcmp(BL_GFF_TYPE(feature), "gene") == 0) ||
	     (strcmp(BL_GFF_TYPE(feature), "mRNA") == 0) )
	    status = print_subfeatures(fasta_rec, feature, gff_stream, description);
	else
	    status = bl_gff_read(feature, gff_stream, BL_GFF_FIELD_ALL);
    }
    
    free(parent_feature_type);
    free(parent_feature_id);
    return status;   // FIXME: Return a status value
}


/***************************************************************************
 *  Description:
 *      Print a substring of a sequence
 *
 *  History: 
 *  Date        Name        Modification
 *  2022-04-12  Jason Bacon Begin
 ***************************************************************************/

void    print_seq(char *feature_seq, int64_t start, int64_t end)

{
    int64_t feature_len = end - start + 1;
    int     save_base;
    
    feature_seq += start - 1;
    save_base = feature_seq[feature_len];
    feature_seq[feature_len] = '\0';    // Temporary null-terminator
    puts(feature_seq);
    feature_seq[feature_len] = save_base;
}


void    usage(char *argv[])

{
    fprintf(stderr, "\nUsage: %s file.gff3 file.fasta[.gz|.bz2|.xz] feature-type 'search-key'\n\n", argv[0]);
    fprintf(stderr, "Search-key is any exact substring of the attributes column in the GFF.\n");
    fprintf(stderr, "To match a gene name exactly, include ';' in the search key. e.g. 'Name=jun;'\n");
    fprintf(stderr, "View your GFF file with \"more file.gff3\" to get ideas for search-key.\n");
    fprintf(stderr, "\nExample:\n\nblt extract-seq Danio_rerio.GRCz11.104.gff3 Danio_rerio.GRCz11.dna.primary_assembly.fa gene 'Name=jun;'\n\n");
    fprintf(stderr, "Be sure to enclose search-key in hard quotes ('') if it contains any special\n");
    fprintf(stderr, "characters such as *, ?, ;, etc.\n\n");
    exit(EX_USAGE);
}
