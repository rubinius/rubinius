---
layout: doc_pt_br
title: Compiler Transforms
previous: Compiler
previous_url: bytecode-compiler/compiler
next: Generator
next_url: bytecode-compiler/generator
---

The bytecode compiler has a simple AST transform mechanism that recognizes
certain AST forms and replaces them. The replaced forms emit different
bytecode than the original form would have emitted. The source for the
transforms is in lib/compiler/ast/transforms.rb

TODO: document the compiler plugin architecture.


### Safe Math Compiler Transform

Since the core libraries are built of the same blocks as any other Ruby code
and since Ruby is a dynamic language with open classes and late binding, it is
possible to change fundamental classes like Fixnum in ways that violate the
semantics that other classes depend on. For example, imagine we did the
following:

  class Fixnum
    def +(other)
      (self + other) % 5
    end
  end

While it is certainly possible to redefine fixed point arithmetic plus to be
modulo 5, doing so will certainly cause some class like Array to be unable to
calculate the correct length when it needs to. The dynamic nature of Ruby is
one of its cherished features but it is also truly a double-edged sword in
some respects.

In the standard library the 'mathn' library redefines Fixnum#/ in an unsafe
and incompatible manner. The library aliases Fixnum#/ to Fixnum#quo, which
returns a Float by default.

Because of this there is a special compiler plugin that emits a different
method name when it encounters the #/ method. The compiler emits #divide
instead of #/. The numeric classes Fixnum, Bignum, Float, and Numeric all
define this method.

The safe math transform is enabled during the compilation of the Core
libraries to enable the plugin. During regular 'user code' compilation, the
plugin is not enabled. This enables us to support mathn without breaking the
core libraries or forcing inconvenient practices.
