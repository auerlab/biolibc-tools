#!/bin/sh -e

##########################################################################
#   Synopsis:
#       fastq-derep file.fastq|fq[.xz|.bz2|.gz]
#       
#   Description:
#       Remove replicates from a fastq file using fastq2tsv to reformat
#       to tab-separated data for easier sorting, then using Unix sort
#       and an awk script to remove adjacent entries with the same
#       sequence (column 2 of the TSV).  Per latest benchmarks,
#       seqkit rmdup --by-sequence is about 3x as fast.  However,
#       fastq-derep can process any file with minimal memory as it
#       uses the Unix sort command, which automatically breaks large
#       files into chunks for later merging.  This may make it possible
#       to dereplicate large files that require too much memory for
#       seqkit rmdup.
#       
#   Arguments:
#       filename    FASTQ file optionally compressed with xz, bzip2, or gzip
#       
#   Returns:
#       Exit status of pipe consisting of optionally a decompressor,
#       fasta2tsv, sort, and awk.
#
#   Examples:
#       fastq-derep file.fastq.xz
#
#   See also:
#       fastq2tsv
#       
#   History:
#   Date        Name        Modification
#   2021-08-16  Jason Bacon Begin
##########################################################################

usage()
{
    printf "Usage: $0 [file.[fastq|fq][.xz|.bz2|.gz]]\n"
    exit 1
}


##########################################################################
#   Main
##########################################################################

# GNU sort is the fastest,
if which gsort > /dev/null; then
    sort=gsort
else
    sort=sort
fi
sort_flags='--buffer-size=25%'

# mawk is the fastest implementation (short of awka) per my benchmarks
if which mawk > /dev/null; then
    awk=mawk
else
    awk=awk
fi

# Default to ../Scripts for Test/test.sh.  Patch on install.
LIBEXEC=../Scripts

# Sort and remove lines with the same SEQ.  gsort -u flag does not limit
# itself to the key field unless -n is also used (unlike BSD sort).
# So implement the uniq function with an awk filter.
case $# in
1)
    fq=$1
    uncompressed=${fq%.*z}
    stem=${uncompressed%.f*q}
    ext=${fq##*.}
    case $ext in
    xz)
	xzcat $fq | fastq2tsv | $sort $sort_flags -k 2 -t "$(printf '\t')" \
	    | $awk -f $LIBEXEC/uniq-seqs.awk
	;;
    
    bz2)
	bzcat $fq | fastq2tsv | $sort $sort_flags -k 2 -t "$(printf '\t')" \
	    | $awk -f $LIBEXEC/uniq-seqs.awk
	;;
    
    gz)
	zcat $fq | fastq2tsv | $sort $sort_flags -k 2 -t "$(printf '\t')" \
	    | $awk -f $LIBEXEC/uniq-seqs.awk
	;;
    
    fastq|fq)
	fastq2tsv < $fq | $sort $sort_flags -k 2 -t "$(printf '\t')" \
	    | $awk -f $LIBEXEC/uniq-seqs.awk
	;;
    
    *)
	usage
	;;
    
    esac
    ;;

0)
    fastq2tsv | $sort $sort_flags -k 2 -t "$(printf '\t')" \
	| $awk -f $LIBEXEC/uniq-seqs.awk
    ;;

*)
    usage
    ;;

esac
