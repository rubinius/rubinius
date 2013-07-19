---
layout: doc_en
title: Ruby Parser
previous: Bytecode Compiler
previous_url: bytecode-compiler
next: AST
next_url: bytecode-compiler/ast
review: true
---

The first stage in the compilation pipeline is the Ruby Parser. The Ruby
parser receives either a String of code or a file and passes an AST to
the next stage of the process, the generator.

The parser itself (called Melbourne) has a C part, which is essentially
MRI's parser, and a Ruby part, which is responsible for creating the Ruby
AST. The C parser communicates with Ruby by calling a method for each
node in the parse tree.

Each of these methods has a signature containing all of the information
about the part of the parse tree it is processing. For instance, if the
underlying Ruby code has an `if` statement, the C parser will call
`process_if` with the line number, a parameter representing the
condition, and parameters representing the body of the if statement and
the else section, if any.

    def process_if(line, cond, body, else_body)
      AST::If.new line, cond, body, else_body
    end

You can see all of the possible `process_` calls by taking a look at
`lib/melbourne/processor.rb` in the Rubinius source code.

Note that in many cases, the parser passes the result of calling a
previous `process_` method as the arguments to a `process_` method. In
the case of `true if 1`, the parser first calls `process_lit(line, 1)` and
`process_true(line)`. It also calls `process_nil(line)`, because the
original parse tree contains a `nil` for the `else` body. It then calls
`process_if` with the line number, the result of `process_lit`, the
result of `process_true`, and the result of `process_nil`.

This process recursively builds up a tree structure, which Rubinius passes
on to the next stage, the Generator stage.

## Files Referenced

* *lib/melbourne/processor.rb*: the Ruby interface to the C parser. This
  file contains methods beginning with `process_`, which the C parser
  calls for each node in the raw parse tree.
* *lib/compiler/ast/\**: the definitions for each of the AST nodes used
  by the melbourne processor.

## Customization

There are two ways to customize this stage of the compilation process.
The easiest way to customize the creation of the AST is through
[AST Transforms](/doc/en/bytecode-compiler/transformations/).

You can also subclass the Melbourne processor and define your own
handlers for the `process_` methods. This is an advanced topic that is
not yet documented.

