---
layout: doc_pt_br
title: Bytecode Compiler
previous: Virtual Machine - Custom Dispatch Logic
previous_url: virtual-machine/custom-dispatch-logic
next: Parser
next_url: bytecode-compiler/parser
review: true
---

O compilador do Rubinius converte código fonte Ruby em bytecodes que podem
ser interpretados pela máquina virtual. A máquina virtual usa uma série de
estágios sequenciais para transformar a entrada algo que ela possa entender.

Cada um dos estágios é desacoplado do resto do processo, esperando apenas
uma forma particular de entrada e enviando sua saída para o próximo estágio.
Como resultado, o processo de compilação tem bons níveis de configuração e
os estágios pode ser instrumentados de maneira simples.

Cada estágio do processo recebe uma entrada, executa, e passa a saída para
o estágio sequinte. Os estágios padrão, assim como suas entradas e saídas
são ilustrados abaixo.

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
