# About RubySpec

RubySpec is a project to write a complete, executable specification for the Ruby programming language. The goal is official recognition and support from all the organizations currently involved in Ruby implementations, such as MatzRuby (including YARV), JRuby, IronRuby, MacRuby, Ruby.NET and MagLev. The specs describe Ruby language syntax, core library classes, and standard library classes.

This project has a free-flowing commit bit policy. If you have had a patch accepted by any current Ruby implementation project, you can get a commit bit by referencing your patch. Anyone who submits an accepted patch to this project will also receive a commit bit. The only requirement is that you have a Github account.

  - [RubySpec website](http://rubyspec.org/)
  - [RubySpec Wiki with documentation](http://rubyspec.org/wiki/rubyspec)
  - [Github repository](http://github.com/rubyspec/rubyspec)

## Running RubySpec

The RubySpec specification files are written in Ruby, utilizing the [MSpec](http://github.com/rubyspec/mspec/tree/master) testing framework, so you need to download MSpec in order to run the RubySpec suite. If you don't have MSpec yet, then go to a directory outside the RubySpec directory and check out its source code from Github:

    $ cd /somewhere
    $ git clone git://github.com/rubyspec/mspec.git
    # MSpec is now available in /somewhere/mspec.

You should add the MSpec `bin` directory to your PATH for easy access. In this README we'll assume that the `mspec` command is in PATH.

    $ export PATH=/somewhere/mspec/bin:$PATH

If you already have MSpec, then you should run `git pull` from the MSpec source directory from time to time, because RubySpec often utilizes the latest MSpec features.

By invoking the `mspec` command inside the RubySpec directory, it will run the entire RubySpec suite against the Ruby interpreter (named `ruby`) in your PATH:

    $ cd /path-to-rubyspec
    $ mspec

### Running RubySpec against a specific Ruby interpreter

Use the `-t` option to specify the Ruby interpreter that you want to run RubySpec against. The argument may be a full path to the Ruby binary:

    # Runs RubySpec against /opt/ruby-enterprise/bin/ruby
    $ mspec -t /opt/ruby-enterprise/bin/ruby

The argument may also be a shortcut. For example, if you specify `j`, then MSpec will look for `jruby` in PATH and run RubySpec against that:

    # Runs RubySpec against JRuby in PATH.
    $ mspec -t j

See `mspec --help` for a list of possible `-t` shortcut values.

### Running parts of the spec

To run a single spec file, pass the filename to MSpec, for example:

    $ mspec core/kernel/kind_of_spec.rb

You can also pass a directory, in which case all specs in that directories will be run:

    $ mspec core/kernel

Note however that passing a directory to MSpec may not always be a good idea, because some specs aren't supposed to be run against the active Ruby interpreter. For example, if you run `mspec -t /usr/bin/ruby1.8 library`, then MSpec will run the 1.9-specific library specs as well, even though you specified Ruby 1.8 as the Ruby interpreter. Instead, you can specify pseudo-directories, which are defined in ruby.1.8.mspec and ruby.1.9.mspec. These pseudo-directories only include files appropriate for the active Ruby interpreter.

    $ mspec :core
    $ mspec :library
    $ mspec :language

## More

For more documentation, e.g. with regard to writing specs, please visit the [Wiki](http://rubyspec.org/wiki/rubyspec).