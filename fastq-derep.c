/***************************************************************************
 *  Description:
 *      Remove replicates from a fastq stream.  This implementation is
 *      similar to seqkit rmdup, using xxhash to generate hash keys and
 *      uthash to build a hash table and check for replicates.
 *
 *      Currently this implementation has the same runtime as seqkit rmdup,
 *      uses significantly less memory, but does not filter all the same
 *      records.  seqkit appears to filter the correct number of records
 *      based on agreement with the sort - uniq method.
 *
 *      This tool was created primarily to demonstrate how easy it can be
 *      to write bioinformatics apps in C, given the right libraries.
 *      In this case, biolibc handles all the FASTQ I/O, xxhash the sequence
 *      hasing, and uthash the hash table management.
 *  
 *  History: 
 *  Date        Name        Modification
 *  2021-07-27  Jason Bacon Begin
 ***************************************************************************/

#include <stdio.h>
#include <sysexits.h>
#include <uthash.h>
#include <xxhash.h>
#include <biolibc/fastq.h>
#include <xtend/mem.h>
#include <xtend/mem.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>

typedef struct
{
    XXH64_hash_t    hash;
    UT_hash_handle  hh;
}   entry_t;

int     main(int argc, char *argv[])

{
    bl_fastq_t      rec = BL_FASTQ_INIT;
    size_t          records_read,
		    records_written;
    int             seed = 0;
    entry_t         *table = NULL,
		    *entry,
		    *found = NULL;
    XXH64_hash_t    hash;
    struct rusage   usage;
    
    fputs("\nRemoving replicate sequences from a FASTQ file may not be a\n"
	"good idea for the following reasons:\n\n"
	"1) It preferentially removes data with no read errors, since\n"
	"   a read error in one read or the other will cause a mismatch.\n"
	"   This can be mitigated by removing replicates after alignment\n"
	"   using samtools, which identifies replicates based on how they\n"
	"   align rather than by 100% identify.\n\n"
	"2) There is no way to distinguish between natural and artificial\n"
	"   replicates at this stage.  Make sure that you either want to\n"
	"   remove both, or that the benefit of removing artificial\n"
	"   replicates outweighs the cost of losing natural ones.  This will\n"
	"   depend on the type of downstream analysis to be done and the\n"
	"   behavior of the sequencer used.\n\n", stderr);
    
    records_read = records_written = 0;
    while ( bl_fastq_read(stdin, &rec) == BL_READ_OK )
    {
	++records_read;
	hash = XXH64(BL_FASTQ_SEQ(&rec), BL_FASTQ_SEQ_LEN(&rec), seed);
	
	/*
	 *  Note: We assume XXH64 produces no collisions, i.e. only
	 *  identical sequences will produce the same hash.  If it does,
	 *  we may end up removing some uniq sequences.  This should be
	 *  extremely rare, though.  We could use the sequence itself
	 *  in the hash table, but this would significantly increase
	 *  memory use (e.g. 100-byte sequences vs 8-byte hashes).
	 */
	HASH_FIND(hh, table, &hash, sizeof(hash), found);
	if ( found == NULL )
	{
	    // Record key for comparison to future records
	    entry = xt_malloc(1, sizeof(entry_t));
	    entry->hash = hash;
	    HASH_ADD(hh, table, hash, sizeof(entry->hash), entry);
	    
	    // Output record
	    bl_fastq_write(stdout, &rec, BL_FASTQ_LINE_UNLIMITED);
	    ++records_written;
	}
    }
    
    fprintf(stderr, "%zu records read, %zu written, %zu removed\n",
	   records_read, records_written, records_read - records_written);
    getrusage(RUSAGE_SELF, &usage);
    fprintf(stderr, "Maximum RAM used = %lu MiB\n", usage.ru_maxrss / 1024);
    return EX_OK;
}
