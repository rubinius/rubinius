---
layout: doc_pt_br
title: Bytecode Compiler
previous: Virtual Machine - Custom Dispatch Logic
previous_url: virtual-machine/custom-dispatch-logic
next: Parser
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

1. [Parser Stage](/doc/pt-br/bytecode-compiler/parser/)
1. [AST](/doc/pt-br/bytecode-compiler/ast/)
1. [Generator Stage](/doc/pt-br/bytecode-compiler/generator/)
1. [Encoder Stage](/doc/pt-br/bytecode-compiler/encoder/)
1. [Packager Stage](/doc/pt-br/bytecode-compiler/packager/)
1. [Writer Stage](/doc/pt-br/bytecode-compiler/writer/)
1. Printers
1. [Transformations](/doc/pt-br/bytecode-compiler/transformations/)
1. [Customizing the Pipeline](/doc/pt-br/bytecode-compiler/customization/)
