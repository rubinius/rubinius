---
layout: doc_en
title: Methods
previous: Scripts
previous_url: ruby/scripts
next: Constants
next_url: ruby/constants
review: true
---

# Basics

If you have used other languages, you certainly know what is a function.
A function is a peace of code which can take several arguments or parameters
(they designate the same term) and which will return or print out something
when we call it. A function, in most case, return a result.
A method works like a function but is associated to an object. Take a
simple example: a method which say "hello" to a given name:

    
    def hello(name)
      puts "Hello #{name}!"
    end

    hello("John")

Now, if you execute your script, it will output "Hello John". We have said that
a method is associated to an object and even if we don't create a single one
here, our method is associated to an object. This object is the `main` object
which inherit from the `Object` object.

Here our method print out a sentence. In most case, we return something then
use a method to print the return (like `puts` in most case). Here, we can
edit our method like this:

    
    def hello name
      return "Hello #{name}!"  	  
    end

    puts hello "John"

In the code above, you can see that there is no more braces both in the method
declaration and when we call it. Rubies lets you write beautiful code with
no extra braces, semi-colon or whatever which can seems to be useless. In 
programming it's what we call *syntaxic sugar*. You'll see that there is a lot
of syntaxic sugar in Ruby, and, sometimes, you'll see that syntaxic sugar is
essential.

There is also a rule in Ruby you can consider as a kind of syntaxic sugar which
says that the last line of a method is the returned statement. Sometimes, we
still use `return` for more readability. So, we can edit our method like this:


    def hello(name)
      "Hello #{name}"
    end

And it will work! 

# Arguments

As we have see in the previous example, a method can take arguments (also called
parameters). In the previous example, our uniq parameter was the name of the 
person who we want to say hello. If we want, we can pass several arguments if 
we named them. We will also see that we can also pass specific argument such
as blocks. For example, we can define a `multiply` method which will multiply
two given numbers. For example:

    
    def multiply(a, b)
      a * b
    end

    multiply 2, 3 # => 6

The problem whith this kind of approach is that you should define x variables
as parameters if you want to multiply x numbers together. It's not really
flexible. The solution is to put an asterisk (*) just before a single parameter
like this for example:

    
    def multiply(*args)
      b = 1
      args.map {|n| b *= n}
      return b
    end

Here, we define a single argument with an asterisk just before it ; it tells
Rubinius to allow the user to pass several arguments to the method. You'll
often see `args` just after the asterisk. It's just a convention but you can
name it as you want. So now, we can call our method multiplying as many numbers
as we want:

    
    multiply(1, 2, 3, 4) # => 24

It works fine. Cool ! One thing we have not covered here is the ability to
define a default value for an argument. For example, take our first example
with the `hello` method. We can for example set a default name for the `name` 
argument like this:

    
    def hello(name = "John")
      "Hello #{name}!"
    end

    puts hello # => Hello John! 

# Bang and predicate methods

There are special methods which are specific to Ruby and often covered together. 
These methods end with a question mark (?) or a bang (!) and are respectively
called predicate and bang methods.

## Predicate methods

The predicate methods (which end with a question mark) return boolean values 
(`true` or `false`). There are not *magical*, you should make code which return 
the correct boolean ; it's just a kind of convention when you named your methods.
For example, take a method, called `palindrome?` which return `true` if the 
given word is a palindrome and `false` if it isn't:

    
    def palindrome?(word)
      if word.reverse == word
        return true
      else
        return false
      end
    end

    palindrome?("kayak") # => true
    palindrome?("volley") # => false

Quite simple, no ? Here, you can see that the reverse method is call on an object
(here the `word` object which is a string). In this case, we often say that the
method is the sender (it sends a message to the object) and the object, the receiver. 

There are native predicate methods such as `Fixnum#even?`, `Array#include?` or
`Kernel#block_given?`.

## Bang methods

The bang methods are often introduced with the predicate methods but their
aim isn't the same. The bang methods (which end with an exclamation mark) are
methods which alter the object itself when they are called on it. For example,
the `reverse` method has a bang equivalent (which is `reverse!`). See how it
works in `irb` for example:

    
    >> name = "John"
    => "John"
    >> name.reverse
    => "nhoJ"
    >> name
    => "John"
    >> name.reverse!
    => "nhoJ"
    >> name
    => "nhoJ"

When we call the `reverse!` method, the name variable is altered and its value
changes accordingly. It's quite hard to try to create a bang method for the
moment because we haven't yet covered some concepts such as `self` or classes. 