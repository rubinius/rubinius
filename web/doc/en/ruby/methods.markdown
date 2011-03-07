---
layout: doc_en
title: Methods
previous: Scripts
previous_url: ruby/scripts
next: Constants
next_url: ruby/constants
review: true
---

## Declaring a Method

To declare a method in Ruby you use the `def` keyword.

{% highlight ruby %}
def rubinius
  puts "Rubinius is fancy!"
end

rubinius # prints: "Rubinius is fancy!"

def String.monkies
  "All objects have monkies"
end

String.monkies # returns: "All objects have monkies"
{% endhighlight %}

## Invoking a Method

To invoke a method you simply use the name of the method.  Paranthesise
are not required when calling a method. You can add them if you want but
it's quite unusual to see `my_lovely_method()` in Ruby.

{% highlight ruby %}
def rubinius
  puts "Rubinius is fancy!"
end

rubinius # prints: Rubinius is fancy!
{% endhighlight %}

## Arguments

Arguments are specified using a comma-separated list of parameters.

{% highlight ruby %}
def lovely_arguments(argument_one, argument_two)
  # Use those lovely arguments
end
{% endhighlight %}

### Required & Optional Arguments

All arguments are required unless a default value is specified. It is
said that the method `lovely_arguments` defined previously has an
[arity](http://www.ruby-doc.org/core/classes/Method.html#M001066) of 2
because it takes two required arguments.

Arguments with default values, otherwise known as optional arguments,
have to go after required arguments.

An example of this, the `say` method below, is valid…

{% highlight ruby %}
def say(something, count = 0)
  count.times do |counter|
    puts "I would like to say #{something}"
  end
end

say("count = 0", 0)
say("with no count we use the default value")

# prints:
# count = 0
# with no count we use the default value
{% endhighlight %}

…but the `fail_whale` method is invalid and can not be parsed. A
`SyntaxError` will be thrown if you try to use code like the example
below.

{% highlight ruby %}
def fail_whale(count = 0, message)
  # You can't declare this method until we have 1.9 support, but why
  # would you want to ;)
end

# Produces something like:
# SyntaxError: filename.rb:1: expecting '='
{% endhighlight %}

### Named Arguments

Sometimes you want to have named arguments passed to a method
invokation. Let's take a look at an example of where this is useful.

{% highlight ruby %}
def without_named_arguments(a, b, c, d)
  p a, b, c, d
end

without_named_arguments('a', 'b', 'c', 'd')
{% endhighlight %}

The `troll` method will work just fine but remembering the order of the
required arguments can be tedious. Ignoring the fact that objects should
probably be used when trolling use of named arguments makes this method
easier to work with.

{% highlight ruby %}
def with_named_arguments(args = {})
  p args[:a], args[:b], args[:c], args[:d]
end

with_named_arguments(:a => 'a', :b => 'b', :c => 'c', :d => 'd')
{% endhighlight %}

## Dynamic Method Declaration

Methods can be declared dynamically with `define_method`. Generally you
don't need to use this and can instead simply use `def`.

{% highlight ruby %}
class Monkey
  [:swing, :eat].each do |method_name|
    define_method(method_name) { puts "I like to #{method_name}!" }
  end
end

Monkey.new.swing # produces: "I like to swing!"
Monkey.new.eat # produces: "I like to eat!"
{% endhighlight %}
