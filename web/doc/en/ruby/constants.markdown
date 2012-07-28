---
layout: doc_en
title: Constants
previous: Methods
previous_url: ruby/methods
next: Classes & Modules
next_url: ruby/classes-and-modules
review: true
---

A constant is just an object wherein we can store strings or whatever object
we want. For example, we can store a simple string (your name for instance):

    
    Name = "John"

A constant can be differencied by a variable by the fact by its name starts
with a capital letter and because it's normally unredefinable. To define a
constant, normally, you just have to capitalize the first letter but, by 
convention, we capitalize all of the letters such as:

    
    NAME = "John"

If you want, you can call methods on the constant or, if you need it, pass
it as an argument of a method. For example, with the `hello` method of the
previous chapter, we can pass our constant as a parameter:

    
    hello(NAME) # => "Hello John"

A constant is just an object so you can call method on it! Such as:

    
    NAME.upcase # => "JOHN"

    NAME.reverse # => "nhoJ"

    NAME.prepend("Doe ") #=> "Doe John"

Bang methods alter the constant itself. You can use the bang equivalent in the
example above placing an exclamation mark just after `upcase` or `reverse` to
change the value of the constant. 

In Ruby, constants are also used to define modules and classes names. You can
for instance take the example of the native classes (`String`, `Fixnum`, `Array`,
or `Hash` for instance). They all start with a capital letter and are considered
as constants.