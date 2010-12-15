---
layout: doc_es
title: Ruby Parser
previous: Bytecode Compiler
previous_url: bytecode-compiler
next: AST
next_url: bytecode-compiler/ast
review: true
translated: true
---

El analizador es una extensión de C llamada Melbourne. Es básicamente el
programa de análisis de RM envuelto en una forma que puede ser utilizado en
Rubinius. El analizador convierte Ruby código fuente a un árbol de análisis
interno de los nodos. El árbol de análisis es procesado por llamar a un método
de Ruby para cada nodo. El procesador está en lib/melbourne/processor.rb. Los
métodos en el procesador de crear objetos de Ruby que son nodos de un árbol de
sintaxis abstracta (AST). La AST se procesa para generar bytecode.
