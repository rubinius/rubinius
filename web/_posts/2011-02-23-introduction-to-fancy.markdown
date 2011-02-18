---
layout: post
title: Introduction to Fancy
author: Christopher Bertels
---

Fancy is a new general-purpose programming language targetting the
Rubinius VM.

This blog post will give a short introduction to the language, what
kind of problems it's trying to solve and why I chose Rubinius as the
VM to run Fancy on.


### What is Fancy? ###

Fancy is a new general-purpose, dynamic, pure object-oriented
programming language heavily inspired by Ruby, Smalltalk and Erlang
that runs on the Rubinius VM. It's the first fully bootstrapped
language, aside from Ruby, running on Rubinius. This means that the
compiler that generates bytecode for Rubinius is written in Fancy
itself.

You can think of Fancy as a mix of features from the mentioned
languages above, taking each of their strengths and improving upon
their weaknesses.
Fancy has a very small core and is largely based on the concept of
message passing, just like Smalltalk. It tries to have as many
language concepts being first-class values in the language.

Just like Ruby, Fancy is a dynamic object-oriented language that
allows changing code at runtime, everything being an expression and
generally embracing more then one way to do things. Fancy also has all
the literal support that Ruby has, plus literal syntax for Tuples and
Patterns (more on that below).

In contrast to Ruby and just like Smalltalk, Fancy has a very small
amount of built-in keywords and all of the control structures are
implemented in terms of message sends to objects using closures.

The third language that served as an inspiration is Erlang, from which
Fancy takes the idea that concurrent programming should be easy by
having the Actor Model built into the language. This part is still a
work in progress, but should come together soon. The fact that
Rubinius has a built-in Channel type, inter-VM communication
capabilities and even an actor library makes implementing this easier
than in traditional systems.


### Why Fancy? ###

I believe there is real value in having a language that supports
certain things out of the box. Especially when it comes to things like
asynchronous and concurrent programming, having proper semantics built
into the language can often help developers more than a library can.
Very often it's not just about the functionality itself but also about
the semantics you want that functionality to have. This can cause
problems particularly if the language's semantics differ from what
your library is trying to solve. A good example is the callback-based
approach to asynchronous progamming which leads to code that differs
both in semantics as well as how code is structured, compared to
synchronous code. Ideally you'd still want to write code in a
synchronous fashion, where exceptions pop up naturally while still
being highly asynchronous.

In that sense Fancy is more flexible than Ruby as there's not many
special case semantics built in to the core language. Everything's
done via message passing, which fits nicely the actor model approach
to concurrency. Fancy's syntax is a lot simpler, too.

Since all the core control structures are just implemented in Fancy
itself and adhere to the message passing protocol, you can easily
override them for your personal needs. This is especially interesting
when implementing domain specific languages.
Say, you'd want to add some logging to conditional or looping
constructs - it's as easy as overriding a method in your DSL's
classes. Fancy also has class-based mixins, so it makes it easy to
share functionality across class hierarchy boundaries.

Finally, I created Fancy because I wanted a language implementation
that was well documented, easy to understand and very flexible to
extend. Ruby is a nice language, but it has some inconsistencies and
there's only so much you can do when you're bound by backwards
compatibility. By starting fresh, Fancy has a clean, simple and easy
to extend core which allows further exploration of features and
abstractions.

### Why target Rubinius? ###

The initial implementation of Fancy was a simple interpreter written
in C++, similar to how Ruby 1.8 (MRI) works. It was a simple AST
walker. After moving to Rubinius and writing an initial bootstrap
compiler in Ruby, the codebase shrank to about 20% of the original
implementation while actually being more performant. This of course is
mostly due to Rubinius' architecture and JIT compiler but it was a
great experience nontheless.

The nice part about having a common virtual machine and runtime is
that you're not forced to a completely different platform to get the
job done. Fancy and Ruby can coexist in the same application nicely
and calling code from one another is dead simple. In fact, as of now,
Rubinius doesn't know anything about Fancy. And it shouldn't. As long
as all languages running on top of it adhere to the same interface
(in this case the bytecode), it should just work fine.

Choosing Rubinius as a successor platform for Fancy was easy. It's
built for Ruby, a language that's closely related to Fancy. Rubinius,
while having been developed as a VM for running Ruby code, is very
flexible and there are many features that abstract over Ruby's
external semantics. It was just a natural choice given the fact that
Rubinius' architecture and design was heavily influenced by Smalltalk
VMs. Also, it's a very nice dynamic bytecode virtual machine. The
community is very responsive and helpful. Bugs get fixed instantly,
there's always someone to help out and overall it's been a great
experience.


### Let's look at some code! ###

OK, enough talking. Let's have a look on how to get some Fancy code up
and running. Our little sample application will be a simple IRC bot
that connects to Fancy's irc channel on Freenode and says hello to
everyone that greets it. To make life easier, there's already a Fancy
package out there that helps with exactly this task:
[FancyIRC](https://github.com/bakkdoor/fancy_irc).

FancyIRC is a simple IRC client library inspired by Ruby's IRC bot
framework [Cinch](https://github.com/cinchrb/cinch). It's much simpler
and the code is fairly easy to read, but it gives you a similar
interface for writing IRC clients or bots.

So let's get going by installing Fancy. You can either use the Fancy
Rubygem and install it with Rubinius or get the code from GitHub and
run `rake` in the directory. You'll also then have to add the `bin`
directory to your `$PATH`. If you want the latest and greatest version
of Fancy I recommend building directly from source, as the Gem might
not be up to date all the time. For demonstration purposes, let's
install the Rubygem.

    $ rbx -S gem install fancy

To get the FancyIRC package we use Fancy's built-in package manager,
which knows how to find the code on GitHub and install it locally:

    $ fancy install bakkdoor/fancy_irc

#### Writing the code ####
{% highlight fancy linenos %}
require: "fancy_irc"

greeter_bot = FancyIRC Client new: {
  configuration: {
    nickname: "greeter_bot"
    server: "irc.freenode.net"
    port: 6667
    channels: ["#fancy"]
  }

  # greet person back
  on: 'channel pattern: /^[hH]ello greeter_bot/ do: |msg| {
    msg reply: "Hello to you too, #{msg author}!"
  }

  # "echo" command
  # invoke with: !echo <text>
  on: 'channel pattern: /^!echo (.*)$/ do: |msg, text| {
    msg reply: "#{msg author} said: #{text}"
  }

  # tell bot to shutdown via !shutdown command
  on: 'channel pattern: /^!shutdown/ do: |msg| {
    msg reply: "OK, shutting down"
    System exit
  }
}

greeter_bot connect
greeter_bot run
{% endhighlight %}

I think the code is pretty straight forward. This should give you a
feeling for what Fancy looks and feels like. There is of course lots
more to Fancy than what was shown here. It would not fit into a single
blog post.

A quick list of what's currently being worked on:

* New pattern matching system: Message passing based pattern matching
  that preserves encapsulation and is very extensible including
  pattern literals that allow custom pattern types to be defined by
  anyone. There's an experimental branch for that. I'm happy
  to answer questions.
* Async support using coroutines (Fibers) - Write async code in a more
  natural way where exceptions propagate naturally and you don't have
  to think about callbacks all the time.
* First-class support for actors - Asynchronous message sends, Futures
  and multi-vm messaging built-in.
* And much more...


### Interested? ###

If you got interested in Fancy and want to know where to go next,
here's a short list of things to check out:

* [Fancy's GitHub repository](https://github.com/bakkdoor/fancy).
  The standard library is completely written in Fancy and most classes
  and methods have docstrings. It should be fairly easy to understand.
* [Programming InFancy](https://github.com/fancy-lang/infancy)
  An open source tutorial on learning Fancy. Work in progress.
* [http://api.fancy-lang.org](http://api.fancy-lang.org)
  A work in progress page containing Fancy's standard library class
  and method documentation.
* [Fancy's Google Group mailinglist](https://groups.google.com/forum/#!forum/fancy-lang)
* IRC Channel: #fancy @ irc.freenode.net
* Website: [http://www.fancy-lang.org](http://www.fancy-lang.org)
