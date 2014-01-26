# Rubinius

Rubinius is an implementation of the Ruby programming language. Rubinius aims
to be compatible with Ruby version 2.1.

Rubinius includes a Ruby parser, bytecode virtual machine, bytecode compiler,
generational garbage collector, and just-in-time (JIT) native machine code
compiler. Rubinius uses native OS threads with no global interpreter lock.
Rubinius also provides C-API compatibility for native C extensions.

The Ruby core library is written almost entirely in Ruby. Rubinius tools, such
as the bytecode compiler and debugger, are also written in Ruby.  Rubinius
provides the same standard libraries as Matz's Ruby implementation (MRI) with
the following exceptions:

* Continuation
* Ripper
* TracePoint
* Tracer

Rubinius runs on Mac OS X and many Unix/Linux operating systems.  Microsoft
Windows is not yet supported.

Most popular Ruby applications, like Rails, run on Rubinius.

## Installing Rubinius from Source

To install Rubinius, use the following steps:

1. Ensure that MRI 2.0+, rubygems, rake, and git are installed
2. ``git clone git://github.com/rubinius/rubinius.git``
3. ``cd rubinius``
4. ``bundle``
5. ``./configure --prefix=/path/to/install/dir``
6. ``rake``

When the install process finishes, follow the directions printed to the
terminal to add the Rubinius executable (bin) directory to your PATH.

## Using RubyGems

Rubinius comes with RubyGems built-in. To install a gem, run the following:

``rbx -S gem install <gem_name>``

## Features Currently Not Supported

The following features of Ruby/MRI are currently not (yet) supported by
Rubinius:

* Keyword arguments (https://github.com/rubinius/rubinius/issues/2669)
* Refinements
* $SAFE levels

## Documentation

After installing Rubinius, run 'rbx docs' to access the built-in documentation
at any time.

## Tickets

Please file tickets for bugs or problems. The issue tracker is:

http://github.com/rubinius/rubinius/issues

## Contributing

The Rubinius team welcomes contributions. For more information read the
CONTRIBUTING file in the root directory of Rubinius.

## License

Rubinius uses the BSD license. See LICENSE for details.