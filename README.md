# biolibc-tools

## Description

Biolibc-tools is a collection of fast, memory-efficient, simple programs
based on biolibc for processing biological data.

These are programs too simple to warrant a separate project, because most
of the functionality is provided by the biolibc library.

They provide permanent, portable solutions for basic data processing tasks
in a form that's easier to install and use, and often more CPU and memory
efficient than analogous tools.

The needs met by biolibc-tools will in turn drive improvements to biolibc.
Most of the new code will go into the library, making it easier over time
to develop more simple C tools for bioinformatics.

## Building and installing

biolibc-tools is intended to build cleanly in any POSIX environment on
any CPU architecture.  Please
don't hesitate to open an issue if you encounter problems on any
Unix-like system.

Primary development is done on FreeBSD with clang, but the code is frequently
tested on Linux, MacOS, NetBSD, and OpenIndiana as well.  MS Windows is not supported,
unless using a POSIX environment such as Cygwin or Windows Subsystem for Linux.

The Makefile is designed to be friendly to package managers, such as
[Debian packages](https://www.debian.org/distrib/packages),
[FreeBSD ports](https://www.freebsd.org/ports/),
[MacPorts](https://www.macports.org/), [dreckly](https://github.com/drecklypkg/dreckly), etc.

End users should install using a package manager, to ensure that
dependencies are properly managed.

I maintain a FreeBSD port and a dreckly package, which is sufficient to install
cleanly on virtually any POSIX platform.  If you would like to see a
biolibc-tools package in another package manager, please consider creating a
package yourself.  This will be one of the easiest packages in the collection
and hence a good vehicle to learn how to create packages.

Note that dreckly can be used by anyone, on virtually any POSIX operating
system, with or without administrator privileges.

For an overview of available package managers, see the
[Repology website](https://repology.org/).

### Installing biolibc-tools on FreeBSD:

FreeBSD is a highly underrated platform for scientific computing, with over
2,000 scientific libraries and applications in the FreeBSD ports collection
(of more than 30,000 total), modern clang compiler, fully-integrated ZFS
filesystem, and renowned security, performance, and reliability.
FreeBSD has a somewhat well-earned reputation for being difficult to set up
and manage compared to user-friendly systems like [Ubuntu](https://ubuntu.com/).
However, if you're a little bit Unix-savvy, you can very quickly set up a
workstation, laptop, or VM using
[desktop-installer](http://www.acadix.biz/desktop-installer.php).
[GhostBSD](https://ghostbsd.org/) offers an experience very similar
to Ubuntu, but is built on FreeBSD rather than Debian Linux.  GhostBSD
packages lag behind FreeBSD ports slightly, but this is not generally
an issue and there are workarounds.

To install the binary package on FreeBSD:

```
pkg install biolibc-tools
```
You can just as easily build and install from source.  This is useful for
FreeBSD ports with special build options, for building with non-portable
optimizations such as -march=native, and for 
[work-in-progress ports](https://github.com/outpaddling/freebsd-ports-wip),
for which binary packages are not yet maintained.

```
cd /usr/ports/biology/biolibc-tools && env CFLAGS='-march=native -O2' make install
cd /usr/ports/wip/biolibc-tools && make install
```

### Installing via dreckly

[Dreckly](https://github.com/drecklypkg/dreckly) is a cross-platform package manager that works on any Unix-like
platform. It is derived from pkgsrc, which is part of [NetBSD](https://www.netbsd.org/), and well-supported
on [Illumos](https://illumos.org/), [MacOS](https://www.apple.com/macos/),
[RHEL](https://www.redhat.com)/[CentOS](https://www.centos.org/), and
many other Linux distributions.
Unlike most package managers, using dreckly does not require admin privileges.  You can install a dreckly
tree in any directory to which you have write access and easily install any
of the nearly 20,000 packages in the collection.

The
[auto-dreckly-setup](https://github.com/outpaddling/auto-admin/blob/master/User-scripts/auto-dreckly-setup)
script will help you install dreckly in about 10 minutes.  Just download it
and run

```
sh auto-dreckly-setup
```

Then, assuming you selected current packages and the default prefix

```
source ~/Dreckly/pkg/etc/dreckly.sh   # Or dreckly.csh for csh or tcsh
cd ~/Dreckly/dreckly/biology/biolibc-tools
sbmake install clean clean-depends
```

## Instructions for packagers

If you would like to add this project to another package manager
rather than use FreeBSD ports or dreckly, basic manual build instructions
for package can be found
[here](https://github.com/outpaddling/Coding-Standards/blob/main/package.md).
Your contribution is greatly appreciated!
