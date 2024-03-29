############################################################################
#
#              Another Programmer's Editor Makefile Template
#
# This is a template Makefile for a simple program.
# It is meant to serve as a starting point for creating a portable
# Makefile, suitable for use under ports systems like *BSD ports,
# MacPorts, Gentoo Portage, etc.
#
# The goal is a Makefile that can be used without modifications
# on any Unix-compatible system.
#
# Variables that are conditionally assigned (with ?=) can be overridden
# by the environment or via the command line as follows:
#
#       make VAR=value
#
# For example, MacPorts installs to /opt/local instead of the default
# ../local, and hence might use the following:
# 
#       make PREFIX=/opt/local
#
# Different systems may also use different compilers and keep libraries in
# different locations:
#
#       make CC=gcc CFLAGS=-O2 LDFLAGS="-L/usr/X11R6 -lX11"
#
# Variables can also inheret values from parent Makefiles (as in *BSD ports).
#
# Lastly, they can be overridden by the environment, e.g.
# 
#       setenv CFLAGS "-O -Wall -pipe"  # C-shell and derivatives
#       export CFLAGS="-O -Wall -pipe"  # Bourne-shell and derivatives
#       make
#
# All these override methods allow the Makefile to respect the environment
# in which it is used.
#
# You can append values to variables within this Makefile (with +=).
# However, this should not be used to add compiler-specific flags like
# -Wall, as this would disrespect the environment.
#
#   History: 
#   Date        Name        Modification
#   2021-08-15  Jason Bacon Begin
############################################################################

############################################################################
# Installed targets

# Arbitrary main binary for APE
BIN     = blt

# Add fastx-diff when finished
BINS    = fastx2tsv fastx-derep vcf-search fasta2seq find-orfs gff3-to-bed \
	  extract-seq chrom-lens fastx-stats ensemblid2gene vcf-downsample \
	  deromanize

############################################################################
# Compile, link, and install options

# Install in ../local, unless defined by the parent Makefile, the
# environment, or a command line option such as PREFIX=/opt/local.
# FreeBSD ports sets this to /usr/local, MacPorts to /opt/local, etc.
PREFIX      ?= ../local

# Where to find local libraries and headers.  If you want to use libraries
# from outside ${PREFIX} (not usually recommended), you can set this
# independently.
LOCALBASE   ?= ${PREFIX}

# Allow caller to override either MANPREFIX or MANDIR
MANPREFIX   ?= ${PREFIX}
MANDIR      ?= ${MANPREFIX}/man
# FIXME: Need to realpath this if relative (e.g. ../local) or blt won't
# find subcommands from arbitrary CWD
# Currently must use cave-man-install.sh for this until a bmake/gmake
# portable method is found
LIBEXECDIR  ?= ${PREFIX}/libexec/biolibc-tools

############################################################################
# Build flags
# Override with "make CC=gcc", "make CC=icc", etc.
# Do not add non-portable options (such as -Wall) using +=
# Make sure all compilers are part of the same toolchain.  Do not mix
# compilers from different vendors or different compiler versions unless
# you know what you're doing.

# Defaults that should work with GCC and Clang.
CC          ?= cc
CFLAGS      ?= -Wall -g -O
CFLAGS      += -DLIBEXECDIR=\"${LIBEXECDIR}\" -DXXH_INLINE_ALL
CFLAGS      += -DVERSION=\"`./version.sh`\"
CFLAGS      += -Wno-char-subscripts

# Link command:
# Use ${FC} to link when mixing C and Fortran
# Use ${CXX} to link when mixing C and C++
# When mixing C++ and Fortran, use ${FC} and -lstdc++ or ${CXX} and -lgfortran
LD          = ${CC}

CPP         ?= cpp

AR          ?= ar
RANLIB      ?= ranlib

INCLUDES    += -isystem ${PREFIX}/include -isystem ${LOCALBASE}/include
CFLAGS      += ${INCLUDES}
RPATH       ?= -Wl,-rpath
LDFLAGS     += -L${PREFIX}/lib ${RPATH},${PREFIX}/lib \
	       -L${LOCALBASE}/lib ${RPATH},${LOCALBASE}/lib \
	       -lbiolibc -lxtend -lm

############################################################################
# Assume first command in PATH.  Override with full pathnames if necessary.
# E.g. "make INSTALL=/usr/local/bin/ginstall"
# Do not place flags here (e.g. RM = rm -f).  Just provide the command
# and let flags be specified separately.

CP      ?= cp
MV      ?= mv
MKDIR   ?= mkdir
LN      ?= ln
RM      ?= rm
SED     ?= sed

# No full pathnames for these.  Allow PATH to dtermine which one is used
# in case a locally installed version is preferred.
PRINTF  ?= printf
INSTALL ?= install
STRIP   ?= strip

############################################################################
# Standard targets required by package managers

.PHONY: all depend clean realclean install install-strip help

all:    ${BINS} blt

blt:    blt.o
	${LD} -o blt blt.o ${LDFLAGS}

fastx2tsv: fastx2tsv.o
	${LD} -o fastx2tsv fastx2tsv.o ${LDFLAGS}

fastx-derep: fastx-derep.o
	${LD} -o fastx-derep fastx-derep.o ${LDFLAGS} -lxxhash

vcf-search: vcf-search.o
	${LD} -o vcf-search vcf-search.o ${LDFLAGS}

fasta2seq: fasta2seq.o
	${LD} -o fasta2seq fasta2seq.o ${LDFLAGS}

find-orfs: find-orfs.o
	${LD} -o find-orfs find-orfs.o ${LDFLAGS}

gff3-to-bed: gff3-to-bed.o
	${LD} -o gff3-to-bed gff3-to-bed.o ${LDFLAGS}

extract-seq: extract-seq.o
	${LD} -o extract-seq extract-seq.o ${LDFLAGS}

chrom-lens: chrom-lens.o
	${LD} -o chrom-lens chrom-lens.o ${LDFLAGS}

fastx-stats: fastx-stats.o
	${LD} -o fastx-stats fastx-stats.o ${LDFLAGS}

fastx-diff: fastx-diff.o
	${LD} -o fastx-diff fastx-diff.o ${LDFLAGS}

ensemblid2gene: ensemblid2gene.o
	${LD} -o ensemblid2gene ensemblid2gene.o ${LDFLAGS}

vcf-downsample: vcf-downsample.o
	${LD} -o vcf-downsample vcf-downsample.o ${LDFLAGS}

deromanize: deromanize.o
	${LD} -o deromanize deromanize.o ${LDFLAGS}

############################################################################
# Include dependencies generated by "make depend", if they exist.
# These rules explicitly list dependencies for each object file.
# See "depend" target below.  If Makefile.depend does not exist, use
# generic source compile rules.  These have some limitations, so you
# may prefer to create explicit rules for each target file.  This can
# be done automatically using "cpp -M" or "cpp -MM".  Run "man cpp"
# for more information, or see the "depend" target below.

# Rules generated by "make depend"
# If Makefile.depend does not exist, "touch" it before running "make depend"
include Makefile.depend

############################################################################
# Self-generate dependencies the old-fashioned way
# Edit filespec and compiler command if not using just C source files

depend:
	rm -f Makefile.depend
	for file in *.c; do \
	    ${CC} ${INCLUDES} -MM $${file} >> Makefile.depend; \
	    ${PRINTF} "\t\$${CC} -c \$${CFLAGS} $${file}\n\n" >> Makefile.depend; \
	done

############################################################################
# Remove generated files (objs and nroff output from man pages)

clean:
	rm -f ${BINS} *.nr *.o

# Keep backup files during normal clean, but provide an option to remove them
realclean: clean
	rm -f .*.bak *.bak *.BAK *.gmon core *.core

############################################################################
# Install all target files (binaries, libraries, docs, etc.)

install: all
	${MKDIR} -p \
	    ${DESTDIR}${PREFIX}/bin \
	    ${DESTDIR}${MANDIR}/man1 \
	    ${DESTDIR}${LIBEXECDIR}
	${INSTALL} -m 0755 blt ${DESTDIR}${PREFIX}/bin
	${INSTALL} -m 0755 ${BINS} ${DESTDIR}${LIBEXECDIR}
	${SED} -e "s|../Scripts|`realpath ${LIBEXECDIR}`|g" \
	    Scripts/fastq-derep.sh > fastq-derep.sh
	${INSTALL} -m 0755 fastq-derep.sh ${DESTDIR}${LIBEXECDIR}
	${RM} fastq-derep.sh
	${INSTALL} -m 0755 Scripts/uniq-seqs.awk ${DESTDIR}${LIBEXECDIR}
	${INSTALL} -m 0444 Man/* ${DESTDIR}${MANDIR}/man1

install-strip: install
	${STRIP} ${DESTDIR}${PREFIX}/bin/blt
	for f in ${BINS}; do \
	    ${STRIP} ${DESTDIR}${LIBEXECDIR}/$${f}; \
	done

help:
	@printf "Usage: make [VARIABLE=value ...] all\n\n"
	@printf "Some common tunable variables:\n\n"
	@printf "\tCC        [currently ${CC}]\n"
	@printf "\tCFLAGS    [currently ${CFLAGS}]\n"
	@printf "\tCXX       [currently ${CXX}]\n"
	@printf "\tCXXFLAGS  [currently ${CXXFLAGS}]\n"
	@printf "\tF77       [currently ${F77}]\n"
	@printf "\tFC        [currently ${FC}]\n"
	@printf "\tFFLAGS    [currently ${FFLAGS}]\n\n"
	@printf "View Makefile for more tunable variables.\n\n"

