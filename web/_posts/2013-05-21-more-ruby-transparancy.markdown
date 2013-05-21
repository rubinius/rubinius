---
layout: post
title: Better access to runtime information from Ruby
author: Dirkjan Bussink
---

Rubinius has always tried to provide a lot of information to you as a
developer about runtime information. Things like the variable scope and
constant scope are already available. In this post I'd like to describe
a small new API that is available for even more information.

### Inline caching

One of the important mechanisms in making your Ruby code fast is doing
inline caching. Inline caching means that we store the result of a method
lookup a specific call site.

So what is a call site you might ask? Well, that's every place in your
code where a method is called. So take for example the following
snippet.

{% highlight ruby linenos %}

class Foo
  def bar
    @foo.quz
  end
end

{% endhighlight %}

Here `@foo.quz` is a call site, so the place where quz is called. What we
do here is cache the type of `@foo` and store which method to execute for
this type.

The fact that inline caching works well is based on the idea that at
runtime for most call sites the receiver type never changes. So in this
case it means that `@foo` is always of the same type. This is
called a monomorphic call site. 

Of course there are cases where this reasoning doesn't apply. If we see
multiple types at a certain call site, it's called a polymorphic call site.
For this we store things in a different object and Rubinius currently stores
up to 3 different types in that case.

It might be surprising to some, but these cache objects are also just
Ruby objects. This means that they can be used just like any other Ruby
object and be inspected for example. So there are objects for an empty
call site, for monomorphic and also polymorphic sites.

There is also a specialized site for places where we call `respond_to?` since that
happens often enough to warrant a special case. In the future there
might be new types introduced such as a specific type for `send` calls.

### Accessing inline caches stored at call sites

So now that we have described all this, how do we get access to this
information? It is actually pretty straightforward and builds on other
things already available.

Under the hood, all Ruby code in Rubinius gets translated into a
`CompiledCode` object, which also provides the API for getting access to
inline caches. One way to get access to the `CompiledCode` object is to
grab it through Ruby's Method API.

{% highlight ruby linenos %}

class Foo
  def bar
    @foo.quz
  end
end

f = Foo.new
compiled_code = f.method(:bar).executable
compiled_code.call_sites.each do |cs|
  p cs # #<Rubinius::CallSite:0xXX script.rb:3#quz(0)>
end

{% endhighlight %}

What you can see here is that we have one call site located at `script.rb`
on line 3 (which is the class definition). Currently it's of the type
`CallSite`, which means that it has a basic structure in place with no
additional information because this method was never called. This can
also be inferred from the (0) at the end which indicates the number of
calls through this object.

So if we make sure we call this at least once, we should be able to see
this information.

{% highlight ruby linenos %}

class Foo
  def bar
    @foo.quz
  end
end

f = Foo.new
begin
  f.bar
rescue
end

compiled_code = f.method(:bar).executable
compiled_code.call_sites.each do |cs|
  p cs # #<Rubinius::MonoInlineCache:0xXX script.rb:3#quz(1)
       #   receiver_class=NilClass stored_module=Kernel
       #   method=#<Rubinius::CompiledCode method_missing file=kernel/delta/kernel.rb>>
end

{% endhighlight %}

What we do here is actually run this inside a begin / rescue clause
because the call will fail because @foo being nil. But this will also
show that we can actually cache that this will run the `method_missing`
method and that we cache that.

What you can see here is that we have a `MonoInlineCache`, because we've
only seen one type for `@foo`, which is `nil` so the receiver class is
`NilClass`. The `stored_module` is the module the saved method is in, which
in this case is `Kernel`. The actual method stored is the `method_missing`
method here. We can also see it has been called 1 time so far.

### Caveats

Currently you get these objects back and you can actually modify them.
Modifying them at the moment would actually result in undefined
behavior. You can't do this easily though since they only provide
accessor readers, but since it's Ruby you can of course hack around with
`instance_variable_set` for example.

You can probably do really crazy stuff with this, if it crashes the vm
it's probably a bug, otherwise you're pretty much on your own when
hacking this.

Another thing is that the counters are not guaranteed to be accurate.
This can mainly happen when the JIT kicks in. The JIT doens't update
counters if it inlines methods so that way the method could be called
more often than you'd think based on the counter. If it's zero though,
you can be sure it has been never called since it will always go through
the interpreter path first.

### Future

So besides for fun and crazy, what is this actually useful for? Well,
first of all we already use it to actually test in Ruby whether our
inline caches work properly. This made testing this a lot easier and we
can easily verify that an empty call sites turns into a monomorphic
cache and subsequently in a polymorphic one if needed.

Another reason this can be very useful is for building better tools.
With this information, it would be possible to create better
refactoring tools for example. The information provided here gives you
insight into which methods are bound where and could help perhaps in
renaming things.

It would also be interesting to gather this information from your
runtime application, so you could see which code does and doesn't get
executed in your production environment. All these things are just
ideas, but API's like the one discussed here make these future
developments possible and allow for more insight in your Ruby code.

So play with and let us know what you've done with it!
