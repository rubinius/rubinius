---
layout: doc_pt_br
title: Coding Style Guide
previous: Communication
previous_url: contributing/communication
next: Ruby
next_url: ruby
---

The following guidelines aim to keep the Rubinius codebase maintainable. When
in doubt about a guideline here, ask us in the #rubinius IRC channel on
irc.freenode.net.


## All Code

  * Configure your editor for soft tabs, not hard tabs
  * Expand tabs to two spaces
  * Leave a trailing newline at the end of each file


## C++ Code

  * No space between condition and paren.
      Use `if(1)` NOT `if (1)`

  * Put the opening brace on the same line as the function declaration or
    conditional

  * Always use curly braces, even if they can be left out.

  * Prefer parentheses to implicit precedence rules (within reason)

  * Alternate versions of functions should be named why they are different
    from the primary.  If there is a function 'person()' and you want a
    version that takes the name of the person, it should be
    'person_with_name(char \*name)' or 'person_with_details(char \*name, ...)'.
    NOT 'person1(char \*name)'.


## Ruby Code

  * Methods: Try to keep your methods short--one screenful and try to adhere
    to DRY within reason. Generally common functionality should be abstracted
    to helper methods (which you can make 'private') but in some cases,
    particularly working with Core, sometimes trying to DRY things up is just
    an obstacle if you have to maneuver around several different error
    conditions, for example.

  * Method names: should be clear, expressive and meaningful. Avoid using
    underscores to 'protect' the method ('\_\_send\_\_') with some exceptions.

  * Smalltalk-style method names are OK, meaning that you could have a method
    `SomeClass.make_from` when it is intended to be invoked as
    `SomeClass.make_from file` or `SomeClass.make_from :file => name`. There
    the parameter name _completes_ the method name and makes for more natural
    reading.

  * Variable names: make them clear and meaningful (with some well-known
    exceptions like using 'i' for a counter.) Try to avoid shadowing method
    names, for example within Array use 'idx' in favour of 'index' because the
    latter is also a method name.

  * Postconditions: use postconditions only *if* your expression is a
    one-liner *and* you do not have many conditions.

  * Blocks: Use either `do ... end` or `{...}`, spaces between the delimiters
    and code (`foo { |arg| code }`). Split long or complex expressions over
    multiple lines like this:

        mapped = foo.map do |elem|
          do_something_with elem
        end

  * Module/Class definitions with scope qualifiers:

        module Rubinius::Profiler
          class Sampler
          end
        end

## Kernel Code

The primary guideline for all kernel code is simple and efficient. Simple code
is often more efficient and generally more comprehensible. There should be no
metaprogramming code in bootstrap. Use the #attr_xxx methods throughout the
kernel source. Also, alias methods using the #alias_method call next to the
method definition. Specify private methods with the `private :sym` method next
to the method definition.  Remember that the versions of the methods listed
above in the alpha stage take a single, symbol argument.

## Documentation

  * Use RDoc for documentation in Ruby code.

  * Use Doxygen for documentation in C++ code.

  * Use Markdown for documentation in the /doc directory. See [Markdown
    syntax](http://daringfireball.net/projects/markdown/syntax) Set the text
    width to 78 characters and use hard breaks.
