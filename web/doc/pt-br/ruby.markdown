---
layout: doc_pt_br
title: Ruby
previous: Style Guide
previous_url: contributing/style-guide
next: Scripts
next_url: ruby/scripts
review: true
---

One perspective to approach understanding how Rubinius implements Ruby begins
with understanding how various concepts in Ruby work. The purpose of this
topic is to introduce Rubinius features by way of Ruby concepts that should
already be familiar. The Rubinius documentation generally assumes that you are
either familiar with Ruby or familiar with virtual machine and compiler
concepts.

Central to each of the topics below is the concept of _scope_. In Ruby syntax,
scope is generally a derived concept. In other words, there are not syntax
elements whose primary responsibility it is to delimit or reference scope. For
this reason, it can be confusing to talk about scope. An example of this is a
normal method definition:

    a = 5

    def diligent(a)
      puts a * 2
    end

Here, the `#diligent` method gives us a name with which to refer to the
computation `puts a * 2`. But the method also defines a closed lexical scope
for the variable `a`. The scope is closed because the `a = 5` above the method
definition has no relation to the variable `a` in `#diligent`.

Often it is asserted that everything in Ruby is an object. This is not quite
true. _Most_ things in Ruby are objects, but some things that are absolutely
essential for running Ruby code are not necessarily objects that you can put
your hands on. Instead, which of these "execution environment" things are
objects in Ruby depends heavily on the implementation. Scope is one of these
things.

Essentially, _scope_ as a concept is a context for answering questions such
as: What is the value of `self` here? What local variables exist here? What
value will the constant `APPLE` have at this point in the code?

Each of the following elements of Ruby are discussed from the perspective of
understanding how Rubinius implements them and how the concept of _scope_ is
involved in each one.

1. [Scripts](/doc/pt-br/ruby/scripts/)
1. [Methods](/doc/pt-br/ruby/methods/)
1. [Constants](/doc/pt-br/ruby/constants/)
1. [Classes & Modules](/doc/pt-br/ruby/classes-and-modules/)
1. [Blocks & Procs](/doc/pt-br/ruby/blocks-and-procs/)
1. [Local Variables](/doc/pt-br/ruby/local-variables/)
1. [Instance Variables](/doc/pt-br/ruby/instance-variables/)
1. [Class Variables](/doc/pt-br/ruby/class-variables/)
1. [Global Variables](/doc/pt-br/ruby/global-variables/)
