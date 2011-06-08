---
layout: post
title: Why Use Rubinius
author: Brian Ford
---

_Why should I use Rubinius?_ We have been asked that question many, many times
over the past four years. It is a great question. It is an important question.
It's a _hard_ question. I'm not holding out on you. I want to give you an
answer that sates your curiosity, helps you make informed decisions, and
empowers you to speak eloquently when _you_ are inevitably asked, "Why do you
use Rubinius?"

The trouble is, there are many different situations in which people use Ruby
and there is simply no answer, however comprehensive, that really speaks to
everyone's concerns. So rather that boring you at length, I thought a _Choose
your own adventure_ style would be a better approach.

From the list below, select the persona that best describes you. Don't worry,
if the one you select doesn't sound right, you can easily backtrack here. Read
as many as interest you. After all, none of us fit easily into any one box.
When you are done exploring all the fascinating reasons to use Rubinius, let's
meet up at the <a href="#wur-conclusion">Conclusion</a> for some parting
words.

_Enjoy!_

<h3><a class="anchor_title" name="wur-personas">Choose Your Persona</a></h3>

* <a href="#wur-newby">Rails or Ruby Newby</a>
* <a href="#wur-creative">The Creative</a>
* <a href="#wur-experienced">Experienced Programmer</a>
* <a href="#wur-seasoned">Seasoned Programmer</a>
* <a href="#wur-academic">Academic Researcher</a>
* <a href="#wur-uber">Über Programmer</a>
* <a href="#wur-philosophical">Philosophy Student Seeking the Meaning of Ruby</a>
* <a href="#wur-manager">Manager</a>
* <a href="#wur-knowledge">Knowledge Seeker</a>
* <a href="#wur-enthusiast">Language Enthusiast</a>


<h4><a class="anchor_title" name="wur-newby">Rails or Ruby Newby</a></h4>

You are pretty new to programming and after hearing about Ruby on Rails you
watched a screencast and made a website. You are curious and enthusiastic.

You are the empty teacup of the Zen proverb. You are a fresh-faced flower
glistening with the morning dew. The sun smiles on you and you smile back.
You seem to like this Ruby language that makes programmers happy and you've
come to lend your cheery spirit...

Welcome!

So, you have heard of this thing called Rubinius or rbx or whatever and some
folks you respect or admire seem to like it and naturally you want to know
what the big deal is and you're like, "Yo, why would I use Rubinius?".

Cool.

Well, you should use Rubinius because I said so. Try your code on it. Tell us
what worked for you. Tell us if something didn't work by opening an
[issue](https://github.com/rubinius/rubinius/issues/).
Set your imagination loose and tell us what tool you would use if you could.

Spend some time reading the Rubinius source code. Start at the `kernel/`
directory. It's full of Ruby code! As you read through how Ruby is
implemented, how it actually works, it will give you a level of understanding
of your code that many programmers don't have in _any_ language.

Most of all, hang on to your curiosity and enthusiasm. Those were vital to the
creation of the Rubinius project in the beginning and have sustained us
through many challenges. We _can_ make our Ruby experience better, freeing us
from the shackles of other languages and foreign libraries. We _can_ have fast
and reliable web servers, games, editors, websites and applications written in
Ruby. We _can_ have first class tools written for and with Ruby. The world
_can_ be rosy red without our glasses.

<a href="#wur-personas"><strong>Back to personas</strong></a>

<h4><a class="anchor_title" name="wur-creative">The Creative</a></h4>

Ruby is groovy. No, not that Groovy, eww, no. I mean:

<blockquote>
<p>groovy |ˈgroōvē| adj.</p>

<ul>
  <li>fashionable and exciting : <em>sporting a groovy new haircut</em></li>
  <li>enjoyable and excellent : <em>he played all the remarkably groovy guitar parts
himself</em></li>
</ul>

<p>(Apple's dashboard dictionary widget.)</p>
</blockquote>

Ruby respects creativity. It has an _aesthetic_. You don't just write Ruby
code, you write _beautiful_ Ruby code.  It would be unthinkable to do
otherwise. Sure, there is more than one way to do many things. This is not
some sterile laboratory. We are not automatons; we are people. Of course,
being utilitarian is not bad. But other languages have that angle pretty
well covered. There is probably only one right way to implement Python.

Rubinius has an aesthetic, too: excellence, utility, simplicity, beauty, joy.
Mostly in that order. Useful code that isn't of very good quality is a drag.
It slows you down. It gives you a headache. It drives you away. We strive to
keep it out of Rubinius. On the other hand, we are not just writing sonnets
here. This is Serious Business™. We have some hard-core problems to solve. So
we strive for excellent, useful, beautiful code that is a joy to work with.

Of course, this is an ongoing process. It is a journey, not a destination.
There are areas of Rubinius that could use a thorough cleaning or a new
perspective on making the implementation of this beautiful object-oriented
language more beautiful and object-oriented.

We welcome your artistic perspective. Help us improve the dialog between
Rubinius and the person using it. The command line doesn't have to be a
desolate place of obscure, condescending error messages. Web interfaces to the
diagnostic tools deserve a good dose of user-experience and interaction
design. You know that feeling you get when looking at an Enterprise web
application? That weird plastic\-masquerading\-as\-quality\-material feeling?
The too much 1996\-Enterprise\-faux\-rounded\-corner\-wanabe\-2006\-hip gloss?
Gives me the willies whenever I have to use an app like that. Yeah, we don't
want that.

We want to create tools that are powerful, graceful, easy to use, and
beautiful to look at. Beautiful tools are easier to use. (Yehuda Katz provided
a couple links related to this: [The Impact of Design and Aesthetics on
Usability, Credibility, and Learning in an Online Environment
](https://www.westga.edu/~distance/ojdla/winter134/david_glore134.html) and
[In Defense of Eye
Candy](http://www.alistapart.com/articles/indefenseofeyecandy). If you know of
other research, leave us a comment.) So if you have a creative bent but enjoy
writing code also, try out Rubinius and let us know where it could use some
polish.

<a href="#wur-personas"><strong>Back to personas</strong></a>

<h4><a class="anchor_title" name="wur-experienced">Experienced programmer</a></h4>

That saying, _Time is Money_, you live by it. You have applications to deliver
and you choose the best tool for the job. You are professional, conscientious,
duly cautious, and not inclined to episodes of emotional exuberance about the
latest fad. You accept compromises. There are always trade-offs. The correct
approach is cost-benefit analysis. The numbers tell the story and level-headed
decision making follows the numbers.

You have heard about Rubinius and you are curious whether it may be
appropriate for your current project. As usual, rather than speculating or
paying too much heed to the buzz, you look into it yourself. After some
investigation, you discover that:

1. Much of Rubinius is implemented in Ruby itself. This may be a big help when
   tracking down troublesome bugs.
1. Rubinius has a very fast bytecode virtual machine, as well as a modern
   generational garbage collector so memory profiles should be more
   predictable and consistent in deployed applications.
1. It has a profile-driven JIT compiler that uses type-feedback to
   aggressively inline methods resulting in significant performance
   improvements.
1. It has a built-in debugger and precise method profiler, both of which are
   fast due to being well integrated.
1. It has a built-in API for monitoring a VM out-of-process, even on a remote
   machine. We are building a variety of diagnostic tools atop this API.

Of course, even if the technology in Rubinius sounds terrific in theory, how
suitable is Rubinius for your application? How does it perform under your
specific constraints? Again, you do some investigating. You have a solid test
suite for your application, so you start by running that. If you hit any
problems, please open an [issue](https://github.com/rubinius/rubinius/issues/)
to let us know.

If everything goes well with the tests, you start running some of the
benchmarks that you have accumulated while doing performance tuning. Of
course, no sensible person asks for benchmark results from _other_ people's
code. That defies logic. It's like asking if your program will run because
your Aunt Mabeline likes decaf coffee. It's contrary to the very point of
benchmarking, where you are trying to correlate two values that are connected.

Again, if you note an significant issues, please let us know. Sometimes
Rubinius exposes issues in existing code. Performance characteristics of real
applications are vital to making Rubinius faster. Also, if you have
suggestions for tools you would like to use, tell us. If you just want to
chat about the technology, that's fine, too. We're hanging out in the
#rubinius channel on freenode.net.

<a href="#wur-personas"><strong>Back to personas</strong></a>

<h4><a class="anchor_title" name="wur-seasoned">Seasoned programmer</a></h4>

Well, I am being kind by saying _seasoned_. You know when you look in the
mirror that _jaded_ and _cynical_ are much more apt. You've seen it all and it
has worn you down. You've been fighting the good fight, carefully guarding
that last flicker of optimism that burns in the secret place deep in your
heart. You've programmed Java/.NET/C++ professionally. You've even sucked it
up and written some PHP and Python when asked; you are a professional, they
ask and you deliver. You've seen attacked servers on fire off the shoulder of
Rackspace...

Rubinius has a lot to offer you. Remember that little flicker of optimism?  It
is only the idealists that get ground down by the complete indifference to
pursuit of an ideal in so much of the world. Deep down, you are an idealist
and you will find plenty to refresh you here.

Rubinius aims to be the best possible implementation of Ruby by putting Ruby
itself front and center. We are using modern technology and always improving.
We change when there is a better way to do things. We judiciously rewrite and
are not too attached to any code or algorithm. The legacy Enterprise isn't on
the steering committee. Our work will be done when you can use Ruby, just
Ruby, to solve your thorny problems.

Sure, that sounds idealistic. But never mind the pessimists that tell you that
you have to compromise. If you are not idealistic, you will not be unsatisfied
with things that are not as good as they could be; you will not try to change
the world. So give Rubinius a try, you may be surprised. And if you are, put
all that hard-earned wisdom you have gained to use for the betterment of Ruby.

<a href="#wur-personas"><strong>Back to personas</strong></a>

<h4><a class="anchor_title" name="wur-academic">Academic Researcher</a></h4>

Forgive me for staring, I know it is impolite. I'm just... intrigued. Of
course, you know Ruby is a late bound language, every message sent could
conceivably fail to find a target, potentially resulting in an uncaught
exception and program termination. There's shared state, wild orgies of
mutation that disallow any reasonable attempt at automated parallelization.
Program proof is as oxymoronic a concept as military intelligence. It's a very
messy affair of programming and meta-programming and meta-meta-programming,
which, for the love of Lisp, could be done so simply with macros. There's all
this eager evaluation and complete disregard for purity. Despite vast odds,
somehow programs are written that actually run. You have noted all this with
great objectivity but you are nonetheless interested.

Excellent, we are pleased. We have much to learn and welcome the opportunity
for lively discussions about bringing formal methods to bear on the problems
of making Ruby as fast as possible.

Java benefited tremendously from the amount of attention it received by
academic researchers. Ruby can benefit from some of this research as well, not
to mention the research into Smalltalk and Self that preceded it. But Ruby has
its own set of problems to solve and deserves specific attention. The problems
are hard but not insurmountable. Rubinius is already demonstrating that.  The
suggestion that we need to add more keywords, restrict Ruby dynamism, or write
`public static final int` all over are simply nonsense.

Rubinius already leverages research for fast virtual machines, garbage
collection (e.g. the generational approach and the Immix _mark\-region_
algorithm), and JIT compilers (based on pioneering research done in Self and
used in the JVM Hotspot VM). Rubinius uses the exceptional
[LLVM](http://llvm.org) project for optimization and code generation in the
JIT compiler. We are also working on better infrastructure for the JIT to
address Ruby complexities head-on.

Rubinius would be excellent to use in teaching. A compiler construction class
could study the architecture of the bytecode compiler written in Ruby and
experiment with exploratory changes to the compiler using IRB without having
to recompile anything! A 30-minute introduction to Rubinius could proceed
immediately to simple AST generation and have students experimenting with
their own syntax immediately. While it is easy to get started, there is plenty
of depth for exploring complex topics in virtual-machine construction and
garbage collection.

Whether you are interested in language research or language pedagogy, Rubinius
is an great project to consider. We look forward to hearing from you.

<a href="#wur-personas"><strong>Back to personas</strong></a>

<h4><a class="anchor_title" name="wur-uber">Über programmer</a></h4>

You learned the untyped lambda calculus sitting on your mother's knee while
she worked on her doctorate in computer science. You were substituting terms
before you even uttered the word, "dada". You wrote three different Lisp
implementations in Commodore Basic before you were seven. You can write
multi-threaded web servers in one pass with no tests and never hit a deadlock
or critical data race. You write parsers and compilers for odd languages on a
Friday night for the heck of it while waiting for the pizza to arrive before a
night out at the karaoke bar where you give an inspiring performance of Laga
Gaga's _Poker Face_.

(_Loooong pause_. You're not reading this. You've already written one or a few
languages on Rubinius and posted them to our
[Projects](http://rubini.us/projects/) page. But anyway, I'll continue...)

You are the Luke Skywalker of Ruby; Yoda has nothing more to teach you. Only
your fate confronts you now. Use the Source Luke and save the Federation of
Ruby loyalists from the Evil Oracle and its Java the Hurt.

There are a number of domains in which Ruby could benefit tremendously from
excellent libraries:

1. **Servers and web servers**: the web is here to stay but the argument that all
   applications are going to be in Javascript on the client is not valid. A variety
   of hybrid client-server architectures will continue to be the norm. We need
   software that enables application authors to build a suitable solution to
   their particular problems rather than trying to stuff their apps into
   someone else's solution with layers of wrapping.
1. **Concurrency**: multi-core is here to stay but it is not only functional
   programming that is suitable for high-concurrency applications.
1. **Graphical user interface**: the web browser is also here to stay but it is
   not the last word in applications. There are many cases where GUI apps are
   the best option and Ruby needs a mature library or set of libraries to
   build these apps on any major platform. I know some of these libraries
   exist, but they seem to be collecting dust lately.
1. **Big data and data analysis libraries**: our industry repeatedly witnesses
   the same pattern: domain X starts with huge applications running on huge
   horsepower servers for huge businesses and then it starts appearing in small
   applications on small computers for small businesses. Accounting and
   geographic information systems (GIS) are two examples. Data analysis is
   coming to a laptop near you.

These are general areas in which Ruby can be an excellent solution. So how
does Rubinius fit in? Rubinius is dedicatedly pushing more and more into Ruby
itself. Each of these domain is typically handled in Ruby right now by going
to find a library in a foreign language to wrap in a fuzzy Ruby embrace.
Rubinius is calling on the über-programmers of the world to implement
solutions in Ruby to help us identify performance challenges and address them.

Rubinius is also being used in some fascinating language experiments. Two of
these are Atomo ([http://atomo-lang.org](http://atomo-lang.org) which is
implemented in Haskell, with a Rubinius implementation code-named
[quanto](https://github.com/vito/quanto/)) and Fancy
([http://fancy-lang.org](http://fancy-lang.org)). So, if language design is
your cup of tea, Rubinius offers an excellent platform for experimentation.

<a href="#wur-personas"><strong>Back to personas</strong></a>

<h4><a class="anchor_title" name="wur-philosophical">Philosophy Student Seeking the Meaning of Ruby</a></h4>

Like your persona description, you tend to be long winded. You find most
descriptions too brief, almost dismissive. There are words and words should be
used to delve into the minutiae of minutiae. You, more than anyone, want to
know "Why?" with every fiber of your being. You will continue asking long
after the supply of hallucinogens has been exhausted and everyone else is
drooling in their sleep.

For you, Rubinius is an existential dilemma crying out for justification. If
we already have MRI, why build Rubinius?

It would be accurate to say that Rubinius has a philosophy. That philosophy is
simply this: _Ruby should be a first class language_. What does that mean?
Simply that it should be possible to solve problems writing Ruby code.

Let's consider libraries: Being first class means not having to wrap a Java
library or build a C extension. If wrapping the library were the end of the
story, it wouldn't be so bad. But that is _never_ the case. Libraries have
bugs, weird APIs, incompatibility with other libraries, threading issues, and
disappearing maintainers. They may even be incompatible with newer versions of
the language in which they are written.

This list goes on. To address any one of these issues requires delving into a
different language with weird and incompatible semantics. If the library is
your core competency, that's not such a big deal. But I will wager that it is
not, which is why you are using the library in the first place. Also, the
language in which you are wrapping the library (Ruby here) is not likely the
core competency of the library author, or you probably wouldn't need to be
wrapping it. So Ruby wrapping one of these libraries will always be a
second-class citizen. Decisions will be made about the library's API that do
not give one thought to the Ruby programs using it. Furthermore, the code
written in that foreign language does nothing to support the ecosystem of
Ruby. The knowledge gained in writing the library and the improved skills of
the library author do not benefit Ruby. Ruby deserves better.

Ruby has gotten a big boost recently with the production release of MRI 1.9.2.
There are significant speed improvements and welcomed additions to the core
libraries, like powerful handling of String encodings. At the same time, the
Complex and Rational libraries were added to the core library and rewritten
from Ruby to C code. This is disappointing. We should be able to solve these
problems more effectively in Ruby itself.

The philosophy of Rubinius is to make Ruby a first-class citizen. Ruby plays
second fiddle to no one. There is no other language whose history, semantics,
or vested interests compete with Ruby's. It is true that there are difficult
problems to solve in making Ruby fast. But much of the technology already
exists and we will build what does not. Evan often quips that if we can get
Rubinius caught up to the dynamic language technology of ten years ago, Ruby
will be light-years ahead.  That may be overstating how far behind Ruby is,
but it illustrates the focus of Rubinius.

There's the saying, _In theory, there is no difference between theory and
practice. In practice, there is_. In Rubinius, theory and practice are
merging. We are motivated by the desire for Ruby to be a first-class language.
But we are also showing real progress in making that a reality. The Rubinius
VM executes Ruby code blazingly fast. The JIT compiler, while still being
quite young, is showing great promise. Compatibility with MRI is quite good
and speed is constantly improving.

Is the Rubinius philosophy valid? We think the proof is in the pudding.

<a href="#wur-personas"><strong>Back to personas</strong></a>

<h4><a class="anchor_title" name="wur-manager">Manager</a></h4>

No, it did not cross my mind to describe this persona as Pointy-haired Boss.
Not only would that be unfair to Dilbert, but that persona would be reading an
article on Web Scale. No, you are someone who has fought hard battles in the
trenches and learned valuable lessons: it's about execution and execution
depends on good technology.

Rubinius is building solid technology. We started the RubySpec project and
have contributed tens of thousands of lines of code to it. With the support of
Rubyspec, in just over four years as a public project, we have basically
caught up with MRI 1.8.7 in compatibility and performance. For some code, our
performance is much better, for other code, it is not as good. However,
Rubinius is built on solid, modern technology and the project's trajectory and
velocity are outstanding.

Rubinius is a completely new implementation of core Ruby. Rubinius did not
start as a port of existing code. Furthermore, Rubinius implements its own
virtual machine and garbage collector in C++. The bytecode compiler that
targets the virtual machine is pure Ruby. The core Ruby library is mostly Ruby
with some primitive operations in C++. The JIT compiler uses the
[LLVM](http://llvm.org) project. Given the amount of work being done in the
project, Rubinius is pacing extremely well relative to other implementations.

Currently, we are working on support for Ruby 1.9 features, Windows support,
and full concurrency with no global interpreter lock (GIL).

If you are looking at Ruby to implement your next project, rest assured that
Ruby will have the support of excellent technology. If you are already using
Ruby, consider investigating how your application runs on Rubinius. We welcome
the feedback and look forward to solving challenging engineering problems.

<a href="#wur-personas"><strong>Back to personas</strong></a>

<h4><a class="anchor_title" name="wur-knowledge">Knowledge Seeker</a></h4>

You thirst for Knowledge. You follow it wherever it leads you. You'll happily
walk Haskell's hallowed halls of pure laziness or sit at the feet of the
meta-program gazing raptly at class transmorgrification. You don't judge. You
have more than enough knowledge to be dangerous, enough to know that the
universe is amoral and knowledge is the only Truth there is. Nor does any mere
mortal language bind you. All languages are finite. You'll be here today and
gone tomorrow; there is no permanence for the knowledge seeker.

Rubinius is merely a step along the path you journey. Take what you want, it
is all free. As a Ruby implementation, it has much to offer your quest for
knowledge. The Ruby code in the core library is accessible and easy to follow.
The interface between Ruby and the C++ primitives is consistent. The C++ code
itself is restrained. You won't need a PhD in Turing-complete template
languages to understand it.

Rubinius offers extensive opportunities to learn about programming languages
in general and Ruby in particular. When I first started working with Rubinius,
I knew a little bit about garbage collection and virtual machines. I would
call what I knew, toy knowledge. As I struggled to learn more, it seemed
helpful to consider layers of understanding:

1. **General programming language semantics**: the procedure abstraction,
   looping and iteration, recursion, references and values, etc.
1. **Ruby semantics**: modules and classes, access restrictions, blocks and
   lambdas, etc. Even with fundamental programming knowledge, a particular
   language can be confusing. When I was learning C, a friend was also
   studying it. One day he walked over and threw _The C Programming Language_
   book down on my desk and said, "This `for` loop makes no sense!" He was
   quite upset. "Look," he said, "in this example `for (i=0; i < n; i++)` how
   can `i < n` get executed _after_ the code in the body?!" It's easy to laugh
   at that confusion, but coming from BASIC, that really threw him. Deepening
   our understanding to this second level requires confronting some
   "counter-intuitive" notions.
1. **Hypothetical implementation**: knowing how Ruby works, how might one
   implement it. I think this is an important layer of understanding and it is
   easy to miss or gloss over it. By pausing at this layer and thinking how
   you might implement something, you test whether or not you are really
   understanding it.
1. **The MRI implementation**: Reading the MRI source code is an excellent way
   to investigate Ruby. For one thing, it will inform you how Ruby _actually_
   works, and you may be surprised.
1. **The Rubinius implementation**: here you are exposed to the philosophy of
   Rubinius and the challenges to implementing Ruby. We are attempting to
   bring the beauty of Ruby as an object-oriented language deep into the core
   of Ruby itself.

While the Rubinius code itself offers many opportunities for learning, don't
hesitate to drop by the #rubinius channel on freenode.net and ask us
questions. Perhaps you already know a lot about another language and are
interested in how Rubinius implements some feature. Or you may be relatively
new to programming languages and have some basic questions. We enjoy talking
about these concepts. If you are quite new to Rubinius, you may find these
posts informative:

* [Making Ruby Fast: The Rubinius JIT](http://www.engineyard.com/blog/2010/making-ruby-fast-the-rubinius-jit/)
* [Improving the Rubinius Bytecode Compiler](http://www.engineyard.com/blog/2009/improving-the-rubinius-bytecode-compiler/)
* [Compiling Ruby: From Text to Bytecode](http://www.engineyard.com/blog/2009/the-anatomy-of-a-ruby-jit-compile/)

Finally, consider helping other knowledge seekers by writing blog posts on
what you learn about Rubinius. Or, help us [write documentation](http://rubini.us/doc/en/how-to/write-documentation/)!

<a href="#wur-personas"><strong>Back to personas</strong></a>

<h4><a class="anchor_title" name="wur-enthusiast">Language Enthusiast</a></h4>

You like languages for their intrinsic value. Of course the world comes in
many shapes and sizes. You wouldn't have it any other way. That's the fun and
spice, joie de vivre, raison d'etre, supermarché... Sometimes you get carried
away writing a program in another language just because you like how the
letters arrange down the screen. Ruby is definitely one of the impressive
languages and sometimes you almost notice a tiny bit of favoritism in your
normally egalitarian attitude.

As with any enthusiast, you like to experiment. Your interest is not mere
curiosity or sterile investigation. You want to get your feet wet and your
hands dirty. Rubinius is an excellent opportunity to delve into a number of
fascinating subjects. We can merely suggest a path; your experiences along the
way will tell you whether or not Rubinius has value to you.

If you are most interested in languages themselves, the syntax and arrangement
of features, Rubinius offers you immediate gratification. Look for Evan's
upcoming post on his Language Toolkit or check out the code to
[prattle](https://github.com/evanphx/prattle), a Smalltalk dialect used to
illustrate the ease of building a language on Rubinius. Also look at some of
the existing languages [projects](http://rubini.us/projects/) targeting
Rubinius.

If it is the machinery under the covers that is more interesting, start
reading some code. The bytecode compiler lives in `lib/compiler/`. The virtual
machine is in `vm/`, and the garbage collector is in `vm/gc`. As you are
reading through, consider helping us write better documentation. There are
already sections for the [virtual
machine](http://rubini.us/doc/en/virtual-machine/),
[garbage-collector](http://rubini.us/doc/en/garbage-collector/), [JIT
compiler](http://rubini.us/doc/en/jit/) and [bytecode
compiler](http://rubini.us/doc/en/bytecode-compiler/) in the documentation, so
adding content is easy.

You may also be interested in these previous posts about Rubinius:

* [Rubinius wants to help YOU make Ruby better](http://www.engineyard.com/blog/2010/rubinius-wants-to-help-you-make-ruby-better/)
* [5 Things You'll Love About Rubinius](http://www.engineyard.com/blog/2009/5-things-youll-love-about-rubinius/)
* [Rubinius: The Book Tour](http://www.engineyard.com/blog/2009/rubinius-the-book-tour/)

Most of all, experiment. Rubinius is easy to hack on. Are you curious about a
particular feature needed in your language? Try adding it to Rubinius. Think
Lua is all the rage because it uses a register VM? You could probably write a
register-based bytecode interpreter for Rubinius in an afternoon. That's just
an example, of course. The point is to play around with your ideas and have
fun doing it. I think you'll find Rubinius to be an adventuresome companion.

Be sure to let us know what you're working on. We like to be inspired, too!
Consider writing a blog post about things that you find interesing, like this
[recent post](http://yehudakatz.com/2011/02/18/getting-comfortable-with-rubinius-pure-ruby-internals/) by Yehuda Katz.

<a href="#wur-personas"><strong>Back to personas</strong></a>

<h3><a class="anchor_title" name="wur-conclusion">Conclusion</a></h3>

So there you have it. Just like there are many different viewpoints, there are
many different reasons to use Rubinius. Not all those reasons make sense to
everyone. We believe, however, that Rubinius has something to offer to just
about everyone interested in Ruby. Most importantly, try it!

If we didn't answer your question here, leave us a comment. If you have a
reason for using Rubinius that we didn't mention, let us know. As always, we
appreciate your feedback. Chat with us in the #rubinius channel on
freenode.net, [watch our Github project](https://github.com/rubinius/rubinius),
and [follow us on Twitter](http://twitter.com/rubinius).

P.S. Thanks to David Waite for suggesting the Academic Researcher and Language
Enthusiast personas, I always forget those!
