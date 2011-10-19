---
layout: post
title: Contributing to Rubinius
author: Brian Ford
---

Implementing Ruby is a lot of hard work. The Rubinius project has been lucky
to have [more than 240 contributors](http://it.isagit.com/rubinius/rubinius),
many with _hundreds_ of commits, including code, benchmarks, documentation,
translations, and more. Lately, we have seen a surge of new folks working hard
on 1.9 language features. While we concentrate on making Rubinius easy to
contribute to, this post should clarify some things and pave an easier road to
your first Rubinius commit.


### Contributions

Before diving into the Rubinius code, I want to emphasize that there are many
ways that you can contribute to Rubinius. One of the most valuable for us is
trying your own library or application on Rubinius. We've worked super hard to
make using Rubinius in place of MRI as simple as possible. For example, with a
typical Rails application, here is all you should need to do to get set up:

    rvm install rbx
    rvm use rbx
    gem install bundler

Once you've got Rubinius and Bundler installed, running your application
should be this simple:

    cd <my_application>
    rm Gemfile.lock
    bundle install

Note the step of removing the `Gemfile.lock`. This is necessary to force
bundler to resolve the dependencies for Rubinius. This is very important. Not
all gems for MRI will work with Rubinius, and in some cases Rubinius has
built-in gems or stubs that need to be considered when resolving dependencies
(ruby-debug and ffi are two examples).

Once `bundle install` is finished, you should be able to start your
application just like you would under MRI. If you have any trouble, please let
us know. Issues for Rubinius are [tracked on
Github](https://github.com/rubinius/rubinius/issues).

Another way to contribute to Rubinius is talking about the project. If you
tried your application and your 50 gems installed without problems, consider
[tweeting at us](https://twitter.com/rubinius) or writing up a quick blog post
about your experiences. If you've done something
[fancy](http://fancy-lang.org/) that you'd like to share with us, we're always
happy to have [guest blog
posts](http://rubini.us/2011/02/23/introduction-to-fancy/), too. We even have
documentation on [how to write a blog
post](http://rubini.us/doc/en/how-to/write-a-blog-post/).


### Clone & Build

Before we can do anything else, we need to get the Rubinius source code and
build it. Run the commands below to do this:

    git clone git://github.com/rubinius/rubinius.git
    cd rubinius
    ./configure
    rake

You can run Rubinius directly from the build directory, there is no need to
install it. We provide symlinks for common commands like `gem`, `rake`, `irb`,
`ri`, `rdoc`, and even `ruby`. Just add `<rbx_clone>/bin` to your `PATH`.

If you run into any trouble with these steps, see the [Getting
Started](http://rubini.us/doc/en/getting-started/) page for more information.
You may need to install libraries required to build Rubinius. If you don't
find answers there, visit the `#rubinius` channel on freenode.net and we'll
help you out.

While the build is running, let's get a quick overview of how Rubinius is
organized.


### Code Tour

There are two main divisions in the Rubinius source code. The virtual machine,
garbage collector, and just-in-time (JIT) compiler are written in C++. The
Ruby core library, bytecode compiler, and various tools like the profiler and
debugger are written mostly or all in Ruby.


#### Ruby Core Library

The Ruby core library is found in the `kernel/` directory. The kernel is
divided into subdirectories that are loaded in order when Rubinius boots. The
divisions were made to help share the Ruby core library with other
implementations. I'll cover those basic divisions here. For more details about
how the loading process works, see the
[Bootstrapping](http://rubini.us/doc/en/bootstrapping/) documentation.

1. `alpha.rb` - Sets up very basic Ruby features needed to start loading the
   rest of the core library.
1. `bootstrap/`- Contains implementation-specific features needed to load the
   main parts of the core library.
1. `platform/` - Contains platform-specific features like the FFI
   (foreign-function interface) code that is used extensively in Rubinius to
   bind to standard `libc` functions.
1. `common/` - Contains the majority of the Ruby core library and the code
   should be implementation-agnostic.
1. `delta/` - Contains more implementation-specific code that may extend or
   override code from `common/`.

Most of the work on the Ruby core library will be done in `kernel/common/`.


#### Rubinius VM

The concept of a virtual machine is somewhat nebulous. It can be hard to draw
the lines around the different components. In Rubinius, the code for the
bytecode interpreter, garbage collector, and JIT compiler is under the `vm/`
directory. There are subdirectories for the garbage collector (`vm/gc/`) and
the JIT compiler (`vm/llvm/`). The `main()` function is in
`vm/drivers/cli.cpp`.

One of the important parts of Rubinius are the low-level operations that
cannot be defined in Ruby. These are things like adding two Fixnums together.
These operations are called primitives and the code for them is in
`vm/builtin`. Since you will likely encounter these in the core library, we'll
delve into them a bit.


#### Primitives

All methods that can be called in Ruby are exposed as, well, Ruby methods. If
you open `kernel/bootstrap/fixnum.rb`, you should see the following code:

{% highlight ruby linenos %}
def to_f
  Rubinius.primitive :fixnum_to_f
  raise PrimitiveFailure, "Fixnum#to_f primitive failed"
end
{% endhighlight %}

The `Rubinius.primitive :fixnum_to_f` code looks like a normal Ruby method
call but it is not. It's actually a compiler directive to tag this Ruby method
as having an associated primitive operation. The name of the primitive is
`fixnum_to_f`. This naming convention is standard, being composed of the class
name and the method name. Methods in Ruby that are characters, like `+`, are
given word names for the primitives.

When this method is run, the primitive operation is invoked. If the primitive
operation fails, the Ruby code following the `Rubinius.primitive` line is run.
This code can perform any Ruby operation. For example, it may coerce the
arguments to a particular class and re-dispatch to itself. If no other
operation is appropriate, the method should raise an exception.

To see how the Ruby method relates to the primitive code, open
`vm/builtin/fixnum.hpp`:

{% highlight cpp linenos %}
// Rubinius.primitive :fixnum_to_f
Float* to_f(STATE);
{% endhighlight %}

The `vm/builtin/*.hpp` files are processed by the Rubinius build system to
automatically generate C++ code to resolve and bind these primitive
operations. The comment provides the link between the Ruby method and the C++
method.

Finally, the actual implementation of this primitive is found in
`vm/builtin/fixnum.cpp`:

{% highlight cpp linenos %}
Float* Fixnum::to_f(STATE) {
  return Float::create(state, (double)to_native());
}
{% endhighlight %}

Here you can see that a new Float object is being created from the value of
the Fixnum. Rubinius names the C++ classes that implement the Ruby primitive
operations the same as their Ruby counterparts. One of the goals of Rubinius
is to build an elegant, easily comprehensible system, and we feel that this
consistency has been a great benefit toward that goal.

Now that we have a basic idea of the structure of Rubinius, let's look at some
aspects of its runtime behavior, in particular, supporting different Ruby
language modes.


### Language Modes

Rubinius 2.0 (the master branch) implements both 1.8 and 1.9 language features
in one executable. You can select the language mode at runtime by passing the
`-X18` or `-X19` flag, either as a command line option or by setting the
`RBXOPT` environment variable. Both commands below should have the same
effect:

    RBXOPT=-X19 bin/rbx -v
    bin/rbx -X19 -v

If I run that on my system, I will see the following:

    rubinius 2.0.0dev (1.9.2 0f223599 yyyy-mm-dd JI) [x86_64-apple-darwin10.8.0]

The default language mode is 1.8, so if you invoke `rbx` with no other
options, you'll be running in 1.8 mode. You can change the default mode with a
configure time option as follows:

    ./configure --default-version=1.9

If you configure Rubinius to have a default language mode of 1.9, you can
access 1.8 mode with the `-X18` runtime option as discussed above.

Ok, we've got the code, we understand something about how it is organized,
we've got the runtime behavior down, now let's look at actually implementing
Ruby. To do that, we need to know how Ruby behaves, and that is what RubySpec
is all about.


### By the Spec

Rubinius created the RubySpec project to ensure that we would be faithfully
implementing Ruby behavior, and we are constantly contributing more to it.
Basically, Rubinius does it by the spec. So, any commit to the Ruby core
library in Rubinius must either have new specs or make existing specs pass. To
effectively contribute to Rubinius, you'll need to understand some basics
about RubySpec. I recommend that you have a read through the documentation at
[rubyspec.org](http://rubyspec.org/).

RubySpec includes a custom-built framework for the specs called MSpec. The
syntax is intended to be consistent with [RSpec](http://rspec.info), but there
are various facilities that are purpose-built to support multiple
implementations, multiple versions of Ruby, and multiple platforms.


#### Running Specs

MSpec provides several different scripts to run the specs under different
conditions. The default behavior is to simply run all the specs. If you invoke
the following command, it will run all the Ruby Array specs in the default
language mode, which should be 1.8 unless you configured 1.9 to be the
default:

    bin/mspec core/array

To Run the specs in 1.9 mode, add the `-tx19` option:

    bin/mspec -tx19 core/array

The `-t` option specifies which target to run the specs under. The default in
Rubinius is to run them with Rubinius, so `-tx` is implied. You can easily run
with another target by giving the name of an executable on your PATH or the
full path to an executable. Since the specs are intended to show the behavior
of MRI, if you are writing new specs you need to run them under MRI 1.8.7 and
1.9.2. I have those on my PATH, so I can do the following:

    bin/mspec -t ruby1.8.7 core/array
    bin/mspec -t ruby1.9.2 core/array

Finally, if you are running `bin/mspec` in the Rubinius source directory, the
location of the RubySpecs are known (`spec/ruby/`), so you can use the full
path or the shortened version `core/array` above.


#### Continuous Integration

One goal of MSpec is to make it as easy as possible to run the specs for the
parts of Ruby that have been implemented. It takes a long time to implement
all of Ruby correctly, but we want to know that the parts we have implemented
don't get broken while working on other parts. That is the role of continuous
integration. To use CI effectively, we need to partition the specs into those
that we expect to pass and those we know we don't pass yet. MSpec provides a
facility for this, called tagging, that we'll look at shortly. For now, we'll
just look at running the specs in CI mode.

To run all the Rubinius specs in CI mode under the default language version,
use the following command:

    bin/mspec ci

Likewise, to run these specs in the 1.9 language mode, add the `-tx19` option:

    bin/mspec ci -tx19

The `bin/mspec ci` command runs the `mspec-ci` script. You should be familiar
with this mechanism from working with Git. It's the same idea. The `mspec`
script itself is just a utility to invoke the various specific MSpec scripts.
To see the options for `mspec`, run the following command

    bin/mspec -h

There are three basic functions that MSpec performs and these correspond to
`mspec-run`, `mspec-ci`, and `mspec-tag`. When not given an operation, `mspec`
assumes `run`, so the following two commands are equivalent:

    mspec core/array
    mspec run core/array

If the operation is given, it must be the first parameter to `mspec`. In the
case below, the first command runs `mspec-ci` with `core/array` while the
second command runs `mspec-run` with `core/array` and `ci` as files.

    mspec ci core/array
    mspec core/array ci

Now that we've got the basics of MSpec down, let's look at how we find specs
that fail on Rubinius. To do this, we'll use the `mspec tag` command.


#### Tagged Specs

Since Rubinius uses the tagging mechanism to create the set of CI specs to
run, the best way to discover what parts of RubySpec that Rubinius isn't
passing yet is to list the specs that are tagged. There's a command for that:

    bin/mspec tag --list fails -tx19 :ci_files

This command lists all the specs that are tagged as failing. There's some new
syntax here, namely `:ci_files`. MSpec has the concept of pseudo-directories.
Basically, they are lists of files. The reason for this is that running all
the core or standard library specs in RubySpec is not as simple as just
running all the files under `spec/ruby/core` or `spec/ruby/library`. It's more
complicated than that because there are 1.8- and 1.9-specific libraries.
Rather than wrapping everything in `ruby_version_is` guards, MSpec adds
version-specific lists and names them, for example, `:core` and `:library`.

In this case, we're using the list of files specified by `:ci_files`. This
list excludes some files that are known to cause problems if they are run.

The list of specs that are currently marked as failing is pretty long. We can
reduce the number of tags we are looking at by giving a smaller set of specs
to run.  For example, let's just run the File specs:

    bin/mspec tag --list fails -tx19 core/file

Looking at the output from this command, we notice (at least at the time of
writing this post) that there are several failures in the
`File.world_writable?` specs. We can run just these specs:

    bin/mspec tag --list fails -tx19 core/file/world_writable

If we look into the documentation for `File.world_writable?`, we'll find that
it is a new method introduced in 1.9. Excellent, this gives us an opportunity
to talk about language-specific changes in Rubinius.


### Language-specific Changes

When Rubinius boots, it loads different files depending on what language mode
it is running. In the `kernel/**/` directories, there are `load_order18.txt`
and `load_order19.txt` files. These files are used during the build process to
create separate runtime kernels for Rubinius. You can see these in the
`runtime/18` and `runtime/19` directories after building.

Here's how language-specific features are handled in the Rubinius kernel.

1. If there are no language-specific methods, the name of the file in
   `kernel/common` is the name of the class. In the case here, the file is
   `kernel/common/file.rb`. This rule applies regardless of whether the class
   is 1.8- or 1.9-specific. For example, Rational is part of the 1.9 core
   library, but does not exist in the core library in 1.8. The Rational class
   is in `kernel/core/rational.rb`.
1. If there are version-specific methods, they go in `kernel/common/file18.rb`
   and `kernel/common/file19.rb`. The correct file is then added to the
   appropriate `load_orderNN.txt` file.

In the case of `File.world_writable?`, there is no 1.8 version. So, we open
`kernel/common/file19.rb` and add the method definition. After changing any of
the `kernel/**/*.rb` files, we have to build Rubinius. Run the following
command to do that:

    rake build

After making the change, we verify that the specs pass by running the specs
_without_ the CI tags:

    bin/mspec -tx19 core/file/world_writable

If all the specs pass, then you're ready to remove the CI tags. To do so, run
the following command:

    bin/mspec tag --del fails -tx19 core/file/world_writable

After removing the CI tags, the final step is to ensure that all specs still
pass. To run all the CI specs in both 1.8 and 1.9 modes, simply do:

    rake

If everything passes, you're ready to submit a pull request. All in all, that
wasn't too bad, right?

One final note, if you are making changes to RubySpec, make separate commits
in your pull request for changes to `spec/ruby/**/*_specs.rb` and another
commit for any other Rubinius files. It is fine to commit the removed tags
with the other Rubinius changes.


### Wrapping Presents

The information here should give you everything you need to get your feet wet
in Rubinius. By the way, today is Evan's birthday. If you're not taking him to
dinner, why don't you show your appreciation for this fantastic project he
created by grabbing Rubinius and hacking on some Ruby code. Be safe and have
fun! We can't wait to hear from you.
