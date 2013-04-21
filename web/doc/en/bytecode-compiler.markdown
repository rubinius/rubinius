---
layout: doc_en
title: Bytecode Compiler
previous: Virtual Machine - Custom Dispatch Logic
previous_url: virtual-machine/custom-dispatch-logic
next: Parser Stage
next_url: bytecode-compiler/parser
review: true
---

The Rubinius bytecode compiler converts Ruby source code to bytecode that the
virtual machine can execute. It uses a series of sequential stages to
transform the input into a form that the virtual machine can understand.

Each of these stages is decoupled from the rest of the process,
expecting only a particular input form and sending its output to the
next stage. As a result, the compilation process is quite configurable,
and you can instrument any of the stages simply.

Each stage of the process receives an input, runs the stage, and passes
the output from the stage to the next stage of the process. The default
stages, as well as their inputs and outputs, are illustrated below.

<div style="text-align: center; width: 100%">
  <img src="/images/compilation_process.png" alt="Compilation process" />
</div>

1. [Parser Stage](bytecode-compiler/parser.markdown)
1. [AST](bytecode-compiler/ast.markdown)
1. [Generator Stage](bytecode-compiler/generator.markdown)
1. [Encoder Stage](bytecode-compiler/encoder.markdown)
1. [Packager Stage](bytecode-compiler/packager.markdown)
1. [Writer Stage](bytecode-compiler/writer.markdown)
1. Printers
1. [Transformations](bytecode-compiler/transformations.markdown)
1. [Customizing the Pipeline](bytecode-compiler/customization.markdown)
