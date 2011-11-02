---
layout: doc_es
title: Compilador a bytecode
previous: Lógica de búsqueda y ejecución de métodos
previous_url: virtual-machine/custom-dispatch-logic
next: Parser
next_url: bytecode-compiler/parser
review: true
---

El compilador a bytecode de Rubinius convierte el código fuente a bytecode que
la máquina virtual pueda ejecutar. Su uso es trata de una serie de etapas
secuenciales de transformación y que la máquina virtual la entienda.

Cada una de estas etapas desacoplan el resto del proceso, sólo se espera una
forma de entrada particular y se envía su salida a la siguiente. Como
resultado, el proceso de compilación es muy configurable, y puede instrumentar
simplemente cualquiera de las etapas.

Cada etapa del proceso recibe una entrada, se ejecuta el escenario, y pasa a
la salida de la siguiente etapa del proceso. Las etapas por defecto, así
como sus entradas y salidas, se ilustra a continuación.

<div style="text-align: center; width: 100%">
  <img src="/images/compilation_process.png" alt="Compilation process" />
</div>

1. [Parser](/doc/es/bytecode-compiler/parser/)
1. [AST](/doc/es/bytecode-compiler/ast/)
1. [Generador](/doc/es/bytecode-compiler/generator/)
1. [Encoder Stage](/doc/es/bytecode-compiler/encoder/)
1. [Packager Stage](/doc/es/bytecode-compiler/packager/)
1. [Writer Stage](/doc/es/bytecode-compiler/writer/)
1. Printers
1. [Transformaciones](/doc/es/bytecode-compiler/transformations/)
1. [Customizing the Pipeline](/doc/es/bytecode-compiler/customization/)
