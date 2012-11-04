# What is Rubinius

Rubinius is an implementation of the Ruby programming language.

Rubinius includes a bytecode virtual machine, parser, bytecode compiler, garbage
collector, and just-in-time (JIT) native machine code compiler.

The Ruby core library is written almost entirely in Ruby.

Rubinius provides the same standard libraries as Matz's Ruby implementation
(MRI).

Rubinius also provides C-API compatibility for native C extensions.

Rubinius currently is compatible with Ruby version **1.8.7**. Full support for
Ruby version 1.9.3 is coming soon.

Rubinius runs on Mac OS X and many Unix/Linux operating systems. Support for
Microsoft Windows is coming soon.

Most popular Ruby applications, like Rails, run on Rubinius.


# License

Rubinius uses the BSD license. See LICENSE for details.


# Installing Rubinius from Source

To install Rubinius, use the following steps:

  1. Ensure that MRI 1.8.7+, rubygems, rake, and git are installed

  2. `git clone git://github.com/rubinius/rubinius.git`

  3. `cd rubinius`

  4. `./configure --prefix=/path/to/install/dir`

  5. `rake install`

When the install process finishes, follow the directions printed to the terminal
to add the Rubinius executable (bin) directory to your PATH.

See [Language Modes](#language-modes) below for more information about
configuring Rubinius for different Ruby language versions.


# Installing Rubinius with RVM

Rubinius can be installed using the Ruby Version Manager (RVM) project.

To install Rubinius with RVM, use the following steps:

  1. Ensure the latest RVM is installed

  2. `rvm install rbx`

To use Rubinius in the current shell session, run:

  `rvm use rbx`

To make Rubinius the default interpreter in new shells, run:

  `rvm use rbx --default`

Go to [rvm page](https://rvm.io) for more information.

See [Language Modes](#language-modes) below for more information about the
supported Ruby language versions.

See the [RVM documentation](https://rvm.io) for how to install Rubinius with
different language modes using RVM.


# Using RubyGems

Rubinius comes with RubyGems built-in. To install a gem, run the following:

  `rbx -S gem install <gem_name>`


# Language Modes

Rubinius implements multiple Ruby language features in one executable.

You can select the language mode at runtime by passing the `-X18`, `-X19`, or
`-X20` command line option or by setting the `RBXOPT` environment variable.

The following two commands are equivalent:

  `RBXOPT=-X19 bin/rbx -v`

  `bin/rbx -X19 -v`

The default language mode is 1.8.

You can change which language modes are enabled and which one is the default
mode with configure options.

For example, to enable both 1.9 and 2.0 modes, with 1.9 the default, use the
follwing configure options:

  `./configure --enable-version=1.9,2.0 --default-version=1.9`

The `--enable-version` option takes a comma-separated list of versions.

The `--default-version` option takes a single version.

If only a single version is enabled, setting the default version is redundant.


# Documentation

After installing Rubinius, run `rbx docs` to access the built-in documentation
at any time.


# Tickets

Please file [tickets](http://github.com/rubinius/rubinius/issues) for bugs or
problems.


# Contributing

The Rubinius team welcomes contributions. For more information read the
[CONTRIBUTING](https://github.com/rubinius/rubinius/blob/master/CONTRIBUTING)
file in the root directory of Rubinius.
