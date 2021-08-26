/***************************************************************************
 *  Description:
 *      Remove replicates from a fastq stream.  This implementation is
 *      similar to seqkit rmdup, using xxhash to generate hash keys and
 *      uthash to build a hash table and check for replicates.
 *
 *      Currently this implementation has the same runtime as seqkit rmdup,
 *      and uses about 40% less memory.  It may be possible to improve
 *      performance further with faster hash table library than uthash
 *      and faster I/O operations.
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
#include <biolibc/fastx.h>
#include <xtend/mem.h>      // xt_malloc
#include <xtend/time.h>     // xt_tic, xt_toc
#include <sys/types.h>
#include <sys/time.h>

typedef struct
{
    XXH64_hash_t    hash;
    UT_hash_handle  hh;
}   entry_t;


int     main(int argc, char *argv[])

{
    bl_fastx_t      rec = BL_FASTX_INIT;
    size_t          records_read,
		    records_written;
    int             seed = 0;
    entry_t         *table = NULL,
		    *entry,
		    *found = NULL;
    XXH64_hash_t    hash;
    struct rusage   start_usage,
		    end_usage;
    struct timeval  start_prog,
		    start_hash,
		    end_hash,
		    start_table_find,
		    end_table_find,
		    start_table_add,
		    end_table_add;
    unsigned long   hash_time,
		    table_find_time,
		    table_add_time;
    int             status;
    
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
    
    bl_fastx_init(stdin, &rec);
    xt_tic(&start_prog, &start_usage);
    records_read = records_written = hash_time = 
	table_find_time = table_add_time = 0;
    while ( (status = bl_fastx_read(stdin, &rec)) == BL_READ_OK )
    {
	//fputs(bl_fastx_desc(&rec), stderr);
	++records_read;
	// Profiling with gettimeofday() adds about 1% to run time
	gettimeofday(&start_hash, NULL);
	hash = XXH64(bl_fastx_seq(&rec), bl_fastx_seq_len(&rec), seed);
	gettimeofday(&end_hash, NULL);
	hash_time += difftimeofday(&end_hash, &start_hash);
	
	/*
	 *  Note: We assume XXH64 produces no collisions, i.e. only
	 *  identical sequences will produce the same hash.  If it does,
	 *  we may end up removing some uniq sequences.  This should be
	 *  extremely rare, though.  We could use the sequence itself
	 *  in the hash table, but this would significantly increase
	 *  memory use (e.g. 100-byte sequences vs 8-byte hashes).
	 */
	
	gettimeofday(&start_table_find, NULL);
	HASH_FIND(hh, table, &hash, sizeof(hash), found);
	gettimeofday(&end_table_find, NULL);
	table_find_time += difftimeofday(&end_table_find, &start_table_find);
	if ( found == NULL )
	{
	    // Record key for comparison to future records
	    gettimeofday(&start_table_add, NULL);
	    entry = xt_malloc(1, sizeof(entry_t));
	    entry->hash = hash;
	    HASH_ADD(hh, table, hash, sizeof(entry->hash), entry);
	    gettimeofday(&end_table_add, NULL);
	    table_add_time += difftimeofday(&end_table_add, &start_table_add);
	    
	    // Output record
	    bl_fastx_write(stdout, &rec, BL_FASTX_LINE_UNLIMITED);
	    ++records_written;
	}
    }
    
    fprintf(stderr, "bl_fastx_read() status = %d\n", status);
    fprintf(stderr, "%zu records read, %zu written, %zu removed\n",
	   records_read, records_written, records_read - records_written);
    xt_toc(stderr, NULL, &start_prog, &start_usage);
    fprintf(stderr, "xxHash encode    = %10lu microseconds\n", hash_time);
    fprintf(stderr, "uthash find      = %10lu microseconds\n", table_find_time);
    fprintf(stderr, "uthash add       = %10lu microseconds\n", table_add_time);
    getrusage(RUSAGE_SELF, &end_usage);
    fprintf(stderr, "Maximum RAM used = %10lu MiB\n",
	    end_usage.ru_maxrss / 1024);
    return EX_OK;
}
