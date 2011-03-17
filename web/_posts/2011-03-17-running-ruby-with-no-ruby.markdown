---
layout: post
title: Running Ruby With No Ruby
author: Brian Ford
---

Humans have come a long way since our cave-dwelling days. No, that's not a
metaphor for primitive software. I mean literally since we lived in caves. One
of the big inventions is the lock. There are birds that bury food and will
move it later if they notice they were watched burying it. But they have no
access control. Any bird may come along and dig up the food.

Humans, though, are smarter than the average bird.

We have numerous systems that implement the analog of a lock, namely, some
sort of access control. For every one of these systems, we have other systems
that attempt to circumvent or defeat the access control. Two sides of the
ubiquitous coin of life.

In software, attempts to implement access control typically involve
distinguishing between source code and some form of executable code. Direct
access to the source code is not permitted. Further, the format of the
executable code resists attempts to derive the source code. There are several
reasons for this:

1. **Licenses:** If a vendor licenses individual copies of the software, they
   want to prevent unrestricted copying of the software without paying for a
   license.
1. **Intellectual Property:** A vendor may have trade secrets or other
   proprietary information that is disclosed by the source code.
1. **Security:** Full access to the source code may reveal vulnerabilities in
   the software or related systems. This is sometimes called _Security Through
   Obscurity_ and is generally disparaged, but things are not so black and
   white and there may be valid security concerns in the source code.

The process that separates the source code from the executable program is
typically a compilation step. However, Ruby code is not typically associated
with any sort of compilation. That's one of the great things about Ruby,
right? There is no _edit-compile-link-load_ cycle to wait on. Just edit and run. But
if there is no compilation step, how do we separate the source code from the
executable code?

You may recall from my [last
post](http://rubini.us/2011/03/11/making-rubinius-rbc-files-disappear/) that
Rubinius does compile Ruby code to a bytecode format that the virtual machine
executes. I also promised to explain how you could run the bytecode directly.

But first, let me very clearly state that there are a number of caveats. In
fact, I've included a whole section on them below. Please read them. We will
assume that you have and that you understand them. If you have any questions,
please ask.


### Application Distribution Scenario

Let's review what we would like to accomplish. We'll assume affable Abe is a
developer writing an application for customer Cain.

1. Abe writes some Ruby code.
1. Abe compiles the code to a set of files.
1. Abe packages the files up into an application with no Ruby source.
1. Abe sends the application to Cain.
1. Cain installs the application.
1. Cain runs the application.

In this scenario, I'm assuming a very vague definition of application. In
other words, the process below will fit in with a broad spectrum of bundling
and distribution schemes.


### Application Layout

Let's assume that you have the following application layout. This mirrors what
you would expect to see in a gem. You could also consider this as a subtree in
your larger project.

    widget
    |- lib
    |- widget.rb
    \- widget
       |- red.rb
       |- blue.rb
       \- green.rb

{% highlight ruby linenos %}
# widget.rb
require 'widget/red'
require 'widget/blue'
require 'widget/green'
{% endhighlight %}

{% highlight ruby linenos %}
# widget/red.rb
puts "I am red"
{% endhighlight %}

{% highlight ruby linenos %}
# widget/blue.rb
puts "I am blue"
{% endhighlight %}

{% highlight ruby linenos %}
# widget/green.rb
puts "I am green"
{% endhighlight %}


### Compiling Ruby Files

The Rubinius bytecode compiler is accessible through a command-line script.
See `rbx compile -h` for all options. We will only need one simple option in
our case to easily create a separate tree containing one compiled file for
every Ruby source file in our source tree.

    rbx compile -s '^widget:widget-compiled' widget/

Let's dissect this command. The `-s` option defines a transformation to apply
to every filename. The transformation has the form `<source>:<destination>`
where `<source>` can be a Regexp. In our case, we would like to change any
path starting with `widget` to start with `widget-compiled`. This way, we
create a separate tree of our compiled files. The final option is the
`widget/` directory. The `rbx compile` command will happily compile a single
file or a directory of files. Note that if we did not pass the `-s` option,
`rbx compile` would have created the compiled files alongside the source
files.

If we now look at `widget-compiled`, we should see the following:

    widget-compiled
    |- lib
    |- widget.rbc
    \- widget
       |- red.rbc
       |- blue.rbc
       \- green.rbc


### Loading Pre-compiled Files

Now that we have a separate tree of only compiled files, how do we load them?
Well, first, let's load our source files so we know what to expect. Note that
the technique used in this post should not substitute for a robust test suite.

    $ rbx -Iwidget/lib -e "require 'widget/lib/widget'"
    I am red
    I am blue
    I am green

Ok, that is what I would expect. Now, to load the compiled files:

    $ rbx -Iwidget-compiled/lib -e "Rubinius::CodeLoader.require_compiled 'widget/lib/widget'"
    I am red
    I am blue
    I am green

_The crowed erupts with applause and hooting_.

Golly gee, you guys... _Blush_

Let's review. Our goal is to take a tree of Ruby source files and create a
tree of compiled files that can be sent to a customer and loaded to perform
exactly as the Ruby source would if loaded directly. The most direct and
simple way to accomplish this is to use the Rubinius compiler command-line
script to compile the tree of Ruby source files to a separate tree. Then, load
the root of that tree with `Rubinius::CodeLoader.require_compiled "root"`.


### Caveats

I will admit, I have resisted fiercely against encouraging or even permitting
Rubinius users from using what I showed above in their code. Not because I am
an ogre who is trying to steal your fun, but because there are serious issues
with allowing this. So, please read the following carefully.

1. We, Rubinius, absolutely reserve the right to change any part of the
   underlying compiled file mechanism. Since we are publishing the
   `Rubinius::CodeLoader.require_compiled(name)` method, we will respect that
   contract. What it says is, given a name, we will load a representation of
   that name. _DO NOT assume that `"some_file"` is actually referencing
   `"some_file.rbc"`_. We may change the way compiled files are stored and may
   change the format of the compiled output.
1. We have created this facility to meet a need we had in Rubinius. Since our
   compiler is written in Ruby, we have to run Ruby to run the compiler. But
   since we need to compile Ruby to run it, we need to compile the compiler.
   But since... To handle this, we build the compiler using a bootstrapping
   version of Ruby.  Then we load the pre-compiled compiler files as shown
   above. The approach is quite general, as demonstrated. However, a better
   approach may be appropriate for a particular application. In that case,
   talk to us about how you think it should work and we can point you in a
   direction to try implementing what you need.
1. **We assume no responsibility for any security breaches resulting from your
   incorrect assumption that .rbc files provides any meaningful security
   mechanism.** I cannot stress this enough. The compiled file mechanism is a
   cache. It is a way to avoid recompiling Ruby source code that has not
   changed. The compiled format is simple. We reserve the right to provide
   disassemblers for our compiled code. We are happy to assist you with
   direction for implementing a more secure system for your needs.
1. There is no mechanism that is completely safe from cracking when it comes
   to software access control. Witness how often Microsoft's products have
   their security features defeated. Also witness how often attempts at DRM
   are circumvented. The most secure system I have seen uses a special
   compiler and a hardware dongle. The compiler takes critical parts of the
   application logic and breaks them up so that part of the computation is
   performed on the dongle. This is significantly harder to defeat than binary
   editing an executable to turn a license key check into a no-op. The folks
   you most want to keep from accessing your information are the ones most
   capable of doing so. Security and access control are very hard problems.


### Conclusion

Rubinius compiles Ruby code to bytecode before running it. It is possible to
save the bytecode representation and reload it later. Using this mechanism, it
is possible to avoid providing the Ruby source code and run an application
directly from the compiled bytecode. The mechanism we use to do this was
created to solve our problem of bootstrapping the Rubinius bytecode compiler,
which is written in Ruby. The mechanism is not intended to be used for
security.

It is possible to extend the Rubinius code loading mechanism to support custom
formats for on-disk compiled bytecode and to load those formats. This can be
done entirely in Ruby code. If this interests you, please talk with us about
it.
