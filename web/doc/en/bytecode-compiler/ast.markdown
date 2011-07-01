---
layout: doc_en
title: Abstract Syntax Trees
previous: Parser Stage
previous_url: bytecode-compiler/parser
next: Generator Stage
next_url: bytecode-compiler/generator
review: true
---

Once the parser has finished calling `process_*` methods an
abstract syntax tree is created. The syntax tree represents the
abstract syntactic structure of the parsed source code. Each node in
the tree denotes a construct in the source code. This tree is composed of instances
of classes defined in *lib/compiler/ast/* directory.
Each of these classes inherits from `Rubinius::AST::Node` class.
The `Node` class defines a few methods that are utilized
by the subclasses (eg. pos(g) which records the line number for debugging purposes).

The AST classes usually define at least 3 methods:

* initialize(line, args) - called by various `process_*` methods in the previous stage
* bytecode(g) - called by the Generator in the next stage
* to_sexp - used to represent the AST node as a symbolic expression

The easiest way to see the AST that gets created is to call `to_ast`
on a string containing the code, eg:

    irb(main):002:0> "a = 1".to_ast
    => #<Rubinius::AST::LocalVariableAssignment:0xf70
       @value=#<Rubinius::AST::FixnumLiteral:0xf74 @value=1 @line=1>
       @name=:a @variable=nil @line=1>

or compile the code and pass the -A option:

    rbx compile -A -e "def hello;end"
    Script
      @name: __script__
      @file: "(snippet)"
      @body: \
      Define
        @name: hello
        @line: 1
        @arguments: \
        FormalArguments
          @defaults: nil
          @names: \
          @block_arg: nil
          @optional: \
          @splat: nil
          @line: 1
          @required: \
        @body: \
        Block
          @line: 1
          @array: \
            NilLiteral
              @line: 1

Similarily it's possible to get a representation of the syntax tree as
s-expressions:

    irb(main):002:0> "a = 1".to_sexp
    => [:lasgn, :a, [:lit, 1]]

or pass the -S option to compile:

    rbx compile -S -e "def hello;end"
    [:script, [:defn, :hello, [:args], [:scope, [:block, [:nil]]]]]

The AST is a nested structure where nodes contain other nodes. For
example the `hello` method defined above is composed of a `Script` node
containing `Define` in its `@body` which in turns contains
`FormalArguments` in its `@arguments`  and `Block` in its `@body`. The
`Block` node contains only the `NilLiteral` in its `@array`. The
`NilLiteral` node is a leaf node, it doesn't contain other nodes.

Note that the following `if` expression:

    rbx compile -S -e ":foo if :bar"
    [:script, [:if, [:lit, :bar], [:lit, :foo], nil]]

and the same `if` expression but written differently

    rbx compile -S -e "if :bar then :foo; end"
    [:script, [:if, [:lit, :bar], [:lit, :foo], nil]]

produce exactly the same syntax tree. Because the tree doesn't
represent every detail that appears in the real syntax it is called
"abstract".


## Files Referenced

* *lib/compiler/ast/*: directory containing definitions of all AST classes

## Customization

There are two ways to customize this stage of the compilation process.
The easiest way to customize the creation of the AST is through
[AST Transforms](/doc/en/bytecode-compiler/transformations/).

You can also subclass the Melbourne processor and define your own
handlers for the `process_` methods. This is an advanced topic that is
not yet documented.

