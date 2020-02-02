[![Gitter](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/rubinius/rubinius?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge) [![Build Status](https://travis-ci.org/rubinius/rubinius.svg?branch=master)](https://travis-ci.org/rubinius/rubinius)

# The Rubinius Language Platform

Rubinius is a modern language platform that supports a number of programming languages.

The philosophy and architecture of Rubinius are described below.

Rubinius runs on macOS and many Unix/Linux operating systems. Microsoft Windows is not supported.

## Code of Conduct

Participation in the Rubinius project is governed by the Rubinius [Code of Conduct](https://rubinius.com/code-of-conduct/).

## Issues & Support

Please [file tickets](http://github.com/rubinius/rubinius/issues) for bugs or problems.

For additional help, visit the [Rubinius Gitter chat room](https://gitter.im/rubinius/rubinius).

## Contributing

We welcome contributions. For more information, see the FAQ below and read the [contributing](https://github.com/rubinius/rubinius/blob/master/.github/contributing.md) file.

## License

All source code in this repository is subject to the terms of the Mozilla Public License, version 2.0 unless stated otherwise. A copy of this license can be found the file "LICENSE" or at <https://www.mozilla.org/MPL/2.0/>.

Contributions made prior to January 3rd, 2016 are licensed under the old BSD 3-clause license. A copy of this license can be found in the file "BSD_LICENSE".

In the event a directory contains a "LICENSE", "LICENSE.txt" or "COPYING" file the license specified in said file applies to the contents of said directory and all sub directories, overwriting the licenses specified above.

## Installing Rubinius

To install Rubinius from source, use the following steps:

1. Ensure that MRI 2.0+, rubygems, rake, and git are installed
2. git clone git://github.com/rubinius/rubinius.git
3. cd rubinius
4. ./configure --prefix=/path/to/install/dir && make install

When the install process finishes, follow the directions printed to the terminal to add the Rubinius executable (bin) directory to your PATH.

Certain setups of openssl require you to pass along additional options, `--with-lib-dir` and `--with-include-dir`
pointing to include the relevant openssl directories. 

E.g If you are installing via RVM / homebrew on OS X.
```
rvm install rbx-3 -C \
  --with-lib-dir=$(brew --prefix openssl)/lib \
  --with-include-dir=$(brew --prefix openssl)/include
```

*See https://book.rubinius.com/manuscript/getting_rubinius.html
for a more complete guide to installing Rubinius from source.*

## Philosophy & Architecture

Rubinius is a virtual computing machine. As a computing machine, it possesses several mechanism that physical computers possess to support computation.

It runs in an environment that provides various capabilities. These are presented to the machine through the Environment object.

It has certain configuration values that influence how the machine operates. This is managed by the Configuration object.

The machine has a boot process that carefully builds up the components in a precise order until the machine is ready to run user code. When the user code completes executing, the machine halts, again following a precise sequence of deconstructing the various components.

The rest of the components of the machine are described below.

### Instructions

\<todo>

### Heaps & Garbage Collector

\<todo>

### Console

\<todo>

### Debugger

\<todo>

### Profiler

\<todo>

### Diagnostics

\<todo>

### Machine-code Compiler

\<todo>

### Data Types & Functions

\<todo>

### C-API

\<todo>

## FAQ

**Q. There's this other <programming language, project, concept, application> that seems <better, faster, cheaper>, shouldn't I use that instead?**

A. Yes, absolutely. The sooner the better, really.

**Q. Why are there no tickets tagged for beginners?**

A. We have a lot of respect for your abilities, whether you've ever written a line of code before or not, whether you've ever worked on a compiler, virtual machine, garbage collector, or debugger before.

  Find something that interests you and dive in. If you get stuck, ask a question.

**Q. Is there more documentation?**

A. Yes, there is a [book](https://rubinius.com/book/) that needs a lot of love and attention.

**Q. What about the Ruby Programming language?**

A. Many popular Ruby applications, like Rails, may run on Rubinius, which aims to be compatible with the most recent stable Ruby version.

   Rubinius provides the standard Ruby libraries, with the following exceptions:

  * Continuation
  * Ripper
  * TracePoint
  * Tracer

  The following Ruby features are not supported on Rubinius:

  * Refinements
  * $SAFE levels

**Q. How do I use RubyGems?**

A. Rubinius comes with RubyGems built-in. To install a gem, run the following:

    $ rbx -S gem install <gem_name>

**Q. Why doesn't Rubinius install gem binaries in the same place as Ruby?**

A. Rubinius is intended to be installed alongside Ruby without causing conflicts. Only the main executable, `rbx`, should be installed into a system directory. Edit your shell PATH to include the directories listed when Rubinius is installed to access other executables like gem binaries.
