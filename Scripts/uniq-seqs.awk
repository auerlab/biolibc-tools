#############################################################################
#   Description:
#       Filter out consecutive lines with the same sequence
#
#   History: 
#   Date        Name        Modification
#   2021-08-10  Jason Bacon Begin
#############################################################################

BEGIN {
    FS="\t";
    last_seq="";
    removed=0;
}
{
    if ( $2 != last_seq ) {
	# Put back in proper FASTQ format
	printf("%s\n%s\n%s\n%s\n", $1, $2, $3, $4);
	last_seq=$2
    } else {
	++removed;
    }
}
END {
    printf("Removed %u sequences.\n", removed) > /dev/stderr;
}
