---
layout: doc_de
title: Ruby Parser
previous: Bytecode Compiler
previous_url: bytecode-compiler
next: AST
next_url: bytecode-compiler/ast
review: true
---

The parser is a C extension named melbourne. It is basically the parser from
MRI wrapped up in a way that can be used in Rubinius. The parser converts Ruby
source code to an internal parse tree of nodes. The parse tree is processed by
calling a Ruby method for each node. The processor is in
lib/melbourne/processor.rb. The methods in the processor create Ruby objects
that are nodes in an abstract syntax tree (AST). The AST is processed to
generate bytecode.
