---
layout: doc_es
title: Ruby Parser
previous: Compilador a Bytecode
previous_url: bytecode-compiler
next: AST
next_url: bytecode-compiler/ast
review: true
---

El parser (n. t. analizador léxico) es una extension en C llamada
Melbourne. Básicamente es el mismo analizador que utiliza MRI pero
decorado de una forma que puede ser utilizado en Rubinius. El
analizador convierte código fuente Ruby a un árbol de nodos interno.
El árbol de nodos es procesado invocando un método Ruby por cada nodo.
El procesador está en lib/melbourne/processor.rb. Los
métodos en el procesador crean objetos Ruby que son nodos de un árbol de
sintaxis abstracta (AST). El AST se procesa para generar bytecode.
