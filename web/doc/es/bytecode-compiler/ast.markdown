---
layout: doc_es
title: AST
previous: Etapa de Parsing
previous_url: bytecode-compiler/parser
next: Etapa de Generación
next_url: bytecode-compiler/generator
review: true
---

Una vez que el parser ha terminado de llamar los métodos `process_*`, se crea
un AST, o árbol sintáctico abstracto. Éste representa la estructura sintáctica
abstracta del código fuente analizado. Cada nodo del árbol denota un
constructo en el código fuente. Este árbol está compuesto por instancias de
clases definidas en el directorio *lib/compiler/ast/*.
Cada una de estas clases hereda de la clase `Rubinius::AST::Node`.
La clase `Node` define unos cuantos métodos utilizados por las subclases (por
ejemplo `pos(g)`, que guarda el número de línea para facilitar el proceso de
debugging).

Las clases del AST definen por lo menos 3 métodos:

* initialize(line, args) - llamado por varios métodos `process_*` en la etapa
  anterior.
* bytecode(g) - llamado por el generador en la etapa siguiente
* to_sexp - se usa para representar el nodo AST como expresión simbólica

La forma más fácil de visualizar el AST a partir de determinado código fuente
es llamar `to_ast` sobre una cadena que contenga dicho código, por ejemplo:

    irb(main):002:0> "a = 1".to_ast
    => #<Rubinius::AST::LocalVariableAssignment:0xf70
       @value=#<Rubinius::AST::FixnumLiteral:0xf74 @value=1 @line=1>
       @name=:a @variable=nil @line=1>

O bien compilar el código con la opción -A:

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

De forma similar, es posible obtener una representación del árbol sintáctico
como una serie de expresiones-S:

    irb(main):002:0> "a = 1".to_sexp
    => [:lasgn, :a, [:lit, 1]]

O bien pasar la opción -S al compilar:

    rbx compile -S -e "def hello;end"
    [:script, [:defn, :hello, [:args], [:scope, [:block, [:nil]]]]]

El AST es una estructura anidada donde nodos contienen otros nodos. Por
ejemplo, el método `hello` definido más arriba está compuesto de un nodo
`Script` que contiene `Define` en su `@body`, el cual a su vez contiene
`FormalArguments` en sus `@arguments` y `Block` en su `@body`. El nodo `Block`
contiene solamente un `NilLiteral` en su `@array`. El nodo `NilLiteral` es una
hoja del árbol: no contiene ningún otro nodo.

Es importante tener en cuenta que la siguiente expresión `if`:

    rbx compile -S -e ":foo if :bar"
    [:script, [:if, [:lit, :bar], [:lit, :foo], nil]]

y la misma expresión `if` escrita de forma distinta:

    rbx compile -S -e "if :bar then :foo; end"
    [:script, [:if, [:lit, :bar], [:lit, :foo], nil]]

producen exactamente el mismo árbol sintáctico. El hecho de que el árbol no
represente cada detalle que aparece en la sintaxis real es el motivo por el
cual se llama "abstracto".

## Ficheros mencionados

* *lib/compiler/ast/*: directorio que contiene las definiciones de todas las
  clases del AST.

## Personalización

Hay dos maneras de personalizar esta etapa del proceso de compilación. La
forma más sencilla de personalizar la creación del AST es a través de las
[Transformaciones del AST](/doc/es/bytecode-compiler/transformations/).

También se puede heredar del procesador Melbourne y definir "handlers" para
los métodos `process_`. Éste es un tema avanzado que no está documentado
todavía.

