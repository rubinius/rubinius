---
layout: post
title: Making Rubinius .rbc Files Disappear
author: Brian Ford
---

Rubinius is rather unusual as a Ruby implementation. It both compiles Ruby
source code to bytecode _and_ saves the compiled code to a cache so it does
not need to recompile unless the source code changes. This can be great for
utilities that are run often from the command line (including IRB). Rubinius
merely reloads the cached file and runs the bytecode directly rather than
needing to parse and compile the file. Sounds like a real win!

Unfortunately, it is not that simple. We need some place to store that cache
and this is where the thorns on that pretty rose start poking us in the
thumbs. The solution we have been using since forever is to store the cached
file alongside the source file in the same directory, like so:

    $ echo 'puts "hello!"' > hello.rb
    $ ls hello.*
    hello.rb
    $ rbx hello.rb
    hello!
    $ ls hello.*
    hello.rb	hello.rbc

That doesn't look too crazy, but it can get more complicated:

    $ mv hello.rb hello
    $ rbx hello
    $ ls hello.*
    hello.compiled.rbc	hello.rbc

Whoa, what is `hello.compiled.rbc`? Since `hello` did not have an extension,
we add that longer `compiled.rbc` to make it clear which file the cache is
for. Also, note that we have that `hello.rbc` hanging about even though the
original `hello.rb` is gone.

To summarize the issues with our caching scheme:

1. It requires an additional file for every Ruby source file.
1. It requires some potentially complicated naming scheme to associate the
   cache file with the source and not clash with other names.
1. Removing or renaming the Ruby source file leaves the cache file behind.

Again, the advantage of the cache file is that you do not have to wait for
Rubinius to recompile the file if you have not changed the source. Let's see
if we can get all the advantages with none of the disadvantages. That old
saying comes to mind, _Having your cake and eating it, too_, so we may not
be successful, but it is worth a shot.

First, let's take a step back. This issue is not unique to Rubinius. Python
has `.pyc` and `.pyo` files. Java has `.class` files. C/C++ has `.o` files.
Lots of things need a place to store a compiled or cached representation of
some data. Every SCM worth mention has some mechanism to ignore the files you
don't want to track. The same is generally true of editors. So in some sense,
this is a solved problem. However, we have always received complaints about
the `.rbc` files, so we thought we would try to make other, hopefully better,
solutions available.

### Solution 1: No Cache

One simple solution is just to never ever ever create the compiled cache files
in any form anywhere. We have an option for that:

    $ ls hello.*
    hello.rb
    $ rbx -Xcompiler.no_rbc hello.rb
    hello!
    $ ls hello.*
    hello.rb

Win! Not one lousy `.rbc` file in sight. Although, that's quite the option to
type. Never fear, we have a solution to that below.

Here is our scorecard for solution 1:

**Use Case:** Use when you never want any compiler cache files created. For
example, on a server where startup time is not really a concern.

**Pros:** No `.rbc` files at all.

**Cons:** Startup will be slightly slower depending on what Ruby code you are
running. It will be more noticeable in a Rails application, for example.
However, the Rubinius bytecode compiler is several times faster than it was a
couple years ago so it may not be an issue for you.

### Solution 2: Cache Database

What if we could put all the compilation data in a single cache location,
something like a database? We have an option for that.

This option is a little more complex, so let's take it in two steps.

    $ ls hello.*
    hello.rb
    $ rbx -Xrbc.db hello.rb
    hello!
    $ ls hello.*
    hello.rb
    $ ls -R .rbx
    60

    .rbx/60:
    60c091c3ed34c1b93ffbb33d82d810772902d3f9

Success! No `.rbc` files here. But what's with all the numbers in the `.rbx`
directory and how did that directory get there?

The `-Xrbc.db` option without any argument will store the compilation cache in
the `.rbx` directory in the current working directory. The cache files
themselves are split into subdirectories to avoid creating too many entries
for the file system to handle in one directory.

What if you have a special location where you would prefer all compilation
cache files be saved? No problem, just give `-Xrbc.db` a path as follows:

    $ ls hello.*
    hello.rb
    $ rbx -Xrbc.db=$HOME/.my_special_place hello.rb
    hello!
    $ ls hello.*
    hello.rb
    $ ls -R $HOME/.my_special_place
    60

    /Users/brian/.my_special_place/60:
    60c091c3ed34c1b93ffbb33d82d810772902d3f9

If you primarily work with projects, putting the `.rbx` directory in the
current working directory may be the best solution because it keeps the
compilation cache with the project. It is easy to add an SCM ignore for the
directory and easy to remove the directory to clear the cache (e.g. in a clean
task).

However, if you are frequently running scripts in many directories, you may
not want to litter `.rbx` directories everywhere. In this case, putting the
directory in your `$HOME` dir or `/tmp` may be preferable. Additionally,
`/tmp` may be cleared on every reboot so you will not accumulate many stale
cache files.

Note that, right now, Rubinius does not clear the cache directory. It will
happily continue adding to it indefinitely. However, this may not be an issue
unless you are cycling through a bunch of Ruby files, for example, working on
a number of Ruby projects in series. In that case, using a per-project (per
current working directory) cache is probably the best option.

Here is how solution 2 shakes out:

**Use Case:** You want to combine all compilation cache files in one location.

**Pros:** No `.rbc` files mixed in with the rest of your files.

**Cons:** You may still need a per-project or per-working-directory cache
directory. However, you can easily specify where to put that directory.

### Using RBXOPT for Options

As mentioned above, the `-X` options can get a little long and you certainly
don't want to retype them constantly. We have added support for the `RBXOPT`
environment variable, which is an analog of the `RUBYOPT` environment variable
that we already support.

Use `RBXOPT` to specify `-X` options that Rubinius should use. For example:

    export RBXOPT=-Xrbc.db=/path/to/dir

You can check out all the `-X` options with `rbx -Xconfig.print` or `rbx
-Xconfig.print=2` for more verbose output. If you want to use multiple `-X`
options in `RBXOPT`, use quotes and separate the options with a space:

    export RBXOPT='-Xrbc.db -Xagent.start'

### Conclusion

Rubinius saves a compilation cache for compiled Ruby code to avoid wasting
time and resources recompiling source that has not changed. However, we need
some place to store the cache. Rubinius provides options for omitting the
cache altogether or for storing it in a directory of your choosing. Note that
the format of the compilation cache is an implementation detail and we reserve
the right to change it at any time, so please don't rely on it being in any
particular format.

We have not turned on `-Xrbc.db` by default yet because we don't know what a
good default is. So give us feedback on your use cases and what you would find
most useful.

Finally, whenever we discuss the compilation cache we are inevitably asked if
you can run directly from the cache and not use the Ruby source at all after
it has been compiled. The short answer is "Yes", the long answer is "It
depends". I will be writing a post exploring this question in detail shortly.
For now, get out there and write more Ruby code!
