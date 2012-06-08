# Rubinius

## What is Rubinius?

Rubinius is an implementation of the Ruby programming language.

Rubinius includes a bytecode virtual machine, parser, bytecode compiler,
garbage collector, and just-in-time (JIT) native machine code compiler. The
Ruby core library is written almost entirely in Ruby. Rubinius provides the
same standard libraries as Matz's Ruby implementation (MRI). Rubinius also
provides C-API compatibility for native C extensions.

Rubinius currently is compatible with Ruby version 1.8.7. Support for Ruby
version 1.9.2 is coming soon.

Rubinius runs on Mac OS X and many Unix/Linux operating systems. Support for
Microsoft Windows is coming soon.

Most popular Ruby applications, like [Rails](http://rubyonrails.org), run on Rubinius.


## License

Rubinius uses the BSD license. See
[LICENSE](https://github.com/rubinius/rubinius/LICENSE) for details.


## Installing Rubinius from Source

To install Rubinius, use the following steps:

1. Ensure that [MRI 1.8.7+](http://www.ruby-lang.org/en/), [rubygems](https://github.com/rubygems/rubygems), [rake](https://github.com/jimweirich/rake), and [git](http://git-scm.com/) are installed
2. `git clone git://github.com/rubinius/rubinius.git`
3. `cd rubinius`
4. `./configure --prefix=/path/to/install/dir`
5. `rake install`

When the install process finishes, follow the directions printed to the
terminal to add the Rubinius executable (bin) directory to your PATH.

For more information about building and running Rubinius, run `rake docs`.


## Installing Rubinius with RVM

Rubinius can be installed using the [Ruby Version Manager (RVM)](https://rvm.io/) project.

To install Rubinius with RVM, use the following steps:

1. Ensure the latest RVM is installed
2. `rvm install rbx-head`

To use Rubinius in the current shell session, run:

    rvm use rbx

To make Rubinius the default interpreter in new shells, run:

    rvm use rbx --default

The documentation for RVM is available at:

  https://rvm.beginrescueend.com


## Using RubyGems

Rubinius comes with RubyGems built-in. To install a gem, run the following:

    rbx -S gem install <gem_name>


## Documentation

After installing Rubinius, run `rbx docs` to access the built-in documentation
at any time.


## Tickets

Please file tickets for bugs or problems. The issue tracker is:

  http://github.com/rubinius/rubinius/issues


## Contributing

The Rubinius team welcomes contributions. Run `rake docs` and see the
"Contributing" page.
