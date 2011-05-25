---
layout: doc_pt_br
title: Build System
previous: Specs - Compiler
previous_url: specs/compiler
next: Bootstrapping
next_url: bootstrapping
review: true
---

TODO: Many details are missing.

Rubinius uniformly uses Rake as the build system for its own files. However
Rubinius also includes the source for several external libraries and these
typically use makefiles.


## Development versus Install Builds

Rubinius consists of the executable, and various support files like the core
and standard libraries. The executable needs to know where to find these files
even if moved to a different location. To handle this problem, Rubinius
distinguishes two build types: development and install. The executable stores
the full path to its base directory, relative to which it expects to find the
files it needs.

The development executable stores the path to the source directory in which it
was built. You can then move the executable to a different location but as you
make changes to the core library files in the kernel/\*\* directories, the
executable will continue to use those files.

The install executable stores the path to the install directory. Again, even if
the executable is moved to a new location, it will continue to find the
installed files.

This has consequences, of course. If you build a development executable and then
rename your source directory, you will need to rebuild. Likewise, if you build an
install executable and rename the install directory, the executable will *not*
function, *even if the executable is in the directory*. The executable checks a
single, hard-coded path. If this ends up being very painful for some reason,
we'll revise it.


## Installing Rubinius

To install Rubinius, you must first configure it with an install prefix:

    ./configure --prefix=/path/to/install/dir

The configure process creates a 'config.rb' file that specifies the key file
paths that Rubinius uses. Once configured, run 'rake install' to build and
install. The install procedure builds all the files, including compiling the
Ruby standard library files in the lib/ directory, then copies them into the
install location using the 'install' program. The install tasks are located in
rakelib/install.rake.
