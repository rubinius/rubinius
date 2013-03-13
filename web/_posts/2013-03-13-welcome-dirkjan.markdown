---
layout: post
title: Welcome Dirkjan!
author: Brian Shirai
---

I am excited to announce that [Engine Yard](https://engineyard.com) is
sponsoring [Dirkjan Bussink](https://github.com/dbussink) to work on Rubinius!

A long time ago (around 2008) when Dirkjan first showed up in the Rubinius
project he was still a university student finishing his Masters degree. He let
me read a draft of his thesis on brain-computer interfaces. Dirkjan has been
helping out on Rubinius since then. Recently, he's been fixing concurrency,
garbage collection, and JIT issues.

This week, Dirkjan is working from the Engine Yard office in Portland, OR.
We've got a list of topics that we're delving into to get ready for several
technology improvements in Rubinius. We started with a list of five things,
one for each day. Unfortunately, time always goes faster than we expect so
we've dropped one topic. I'll blog a summary of our discussions so you get an
idea of some of the big stuff coming in Rubinius.

Our first discussion was about our inline cache mechanisms. An inline cache is
used at a method call location to speed up calling a method. The first time it
is called, the method is looked up. Subsequently, as long as the receiver is
the correct type, the same method can safely be used. This is a typical
execution-time versus memory size trade-off. We cache the method we found,
which saves time looking it up again at the expense of some additional memory
to store the method. As with all caches, we have to carefully consider cache
invalidation.  Let's use a quick code sample to illustrate the inline cache.

{% highlight ruby linenos %}

class A
  def a
  end
end

class B < A
end
{% endhighlight %}

In the example above, the class `B` inherits from `A`, which provides the method
`a`. If we call `a` on an instance of `B`, we look up the method and find it
in `A`.

{% highlight ruby linenos %}
b = B.new
b.a
{% endhighlight %}

Here, in the code representation for the call `b.a`, we store an inline cache
object. As long as the type of object we see in `b` is the same, we can safely
call the cached method `a`. However, if the type of `b` changes, for example,
because we define a method `a` on class `B` itself, we must invalidate the
cache and look up the method again.

The inline cache can be very important for the speed of the virtual machine
executing Ruby code. However, the inline cache is also very important for our
just-in-time (JIT) compiler that generates machine code, completely avoiding
the VM interpreter. Since the inline cache records the types of the Ruby
objects seen at the call site, we can use that information in the JIT to
generate fast code as long as our assumptions about the object type are
correct.

Since inline caches are so important, Dirkjan and I did a complete review of
our existing inline caching and analyzed how to improve it. The most important
thing for improving execution speed is to do less. And the best way to do less
is to reduce redundancy.

We started our analysis with an enumeration of all the kinds of calls that we
see in Ruby. This is the list that we made:

* fixed-arity method send
* variadic method send
* `respond_to?`
* `kind_of?`
* fixed-arity `a.send`
* variadic `a.send`
* `super` with explicit arguments
* `super` with implicit arguments
* `method_missing`

This list is all kinds of method sends that you can see directly in Ruby code.
We added to this list a few other things that are not in Ruby code:

* extension API calls
* method combinations like `A.new` which reduces to `A.allocate.initialize`
* call_custom (a special Rubinius instruction)
* primitives (parts of Ruby functionality that must be implemented in the VM)

The next thing we did was look at the method call sites from the perspective
of the virtual machine. Certain method calls, like `respond_to?` imply certain
VM operations. This perspective is very important. Sometimes people complain
that Ruby is too dynamic because you could, for example, alias `respond_to?`
or `eval` to some other name. Now any attempt to statically analyze Ruby fails
because you can't know ahead of time what the alias may be. This is actually
not an issue if you consider the perspective from the VM. No matter what the
method name is, the ultimate VM operation for looking up a method will
execute. By associating method call sites with a VM operation, we get more
insight into how best to structure the inline cache.

The next part of our analysis involved looking at what values are stored in
the inline cache for the various types we enumerated, and what values are used
for cache invalidation. The purpose of this analysis is two-fold: correctness
and performance. We want to ensure that our cache invalidation is correct
but also not generating false positives (i.e. invalidating cases that are not
actually invalid). We also want to ensure that storing and updating the data
in the cache can be done efficiently.

The final part of our analysis concerned the cardinality of the cache. In
object-oriented code, a method call site may be _monomorphic_ (only one type
is seen), _polymorphic_ (a small number of different types are seen), or
_megamorphic_ (a huge number of different types are seen). This perspective
can be very important for performance and for memory overhead. We don't want
to waste memory for sites that are monomorphic. Nor do we want the cache to be
thrashing when many different types are seen.

In summary, we looked at these aspects of inline caches: method call site
types, VM operations that result from the method call, the kind of data cached
and cache invalidation, and the cardinality of the method call site. We'll be
taking the results of our discussion and implementing changes to our inline
caching over the next few weeks.

If you get a chance, pop into the #rubinius IRC channel on freenode.net and
say hello to Dirkjan!
