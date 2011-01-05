# About RubySpec

RubySpec is a project to write a complete, executable specification for the
Ruby programming language. The specs describe Ruby language syntax as well as
the core and standard library classes.

* [RubySpec website](http://rubyspec.org/)
* [Github repository](http://github.com/rubyspec/rubyspec)


## Running RubySpec

The RubySpec specification files are written using RSpec-compatible syntax.
RubySpec provides a purpose-built framework, named
[MSpec](http://github.com/rubyspec/mspec/tree/master), for running the specs.

The easiest way to run the RubySpecs suite is to install the MSpec gem.

    $ [sudo] gem install mspec

However, RubySpec often utilizes the latest MSpec features, so you may want to
use MSpec directly from the Git repository.

    $ cd /somewhere
    $ git clone git://github.com/rubyspec/mspec.git

MSpec is now available in `/somewhere/mspec`.

To make the MSpec scripts available, add the MSpec `bin` directory to you
PATH:

    $ export PATH=/somewhere/mspec/bin:$PATH

Once you have MSpec installed, clone the RubySpec Git repository to run the
specs.

    $ cd /somewhere
    $ git clone git://github.com/rubyspec/rubyspec.git

To run the RubySpec suite:

    $ cd /somewhere/rubyspec
    $ mspec

This will execute all the RubySpec specs using the executable named `ruby` on
your current PATH.


### Running RubySpec against a specific Ruby interpreter

Use the `-t` option to specify the Ruby implementation with which to run the
specs. The argument may be a full path to the Ruby binary. For example, to run
RubySpec against `/opt/ruby-enterprise/bin/ruby`:

    $ mspec -t /opt/ruby-enterprise/bin/ruby

There are some arguments that are abbreviations for known Ruby implementations.
For example, if you specify `j`, then MSpec will look for `jruby` in PATH and
run RubySpec against that:

    $ mspec -t j

See `mspec --help` for a list of `-t` abbreviations.


### Running parts of the spec

To run a single spec file, pass the filename to `mspec`:

    $ mspec core/kernel/kind_of_spec.rb

You can also pass a directory, in which case all specs in that directories
will be run:

    $ mspec core/kernel

Note however that passing a directory to MSpec may not always be a good idea,
because some specs aren't supposed to be run against the active Ruby
interpreter. For example, if you run `mspec -t /usr/bin/ruby1.8 library`, then
MSpec will run the 1.9-specific library specs as well, even though you
specified Ruby 1.8 as the Ruby interpreter. Instead, you can specify
pseudo-directories, which are defined in ruby.1.8.mspec and ruby.1.9.mspec.
These pseudo-directories only include files appropriate for the active Ruby
interpreter.

    $ mspec :core
    $ mspec :library
    $ mspec :language
