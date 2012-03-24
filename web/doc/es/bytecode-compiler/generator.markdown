---
layout: doc_es
title: Etapa de Generación
previous: AST
previous_url: bytecode-compiler/ast
next: Etapa de Codificación
next_url: bytecode-compiler/encoder
review: true
---

Una vez que Melbourne ha creado un AST, invoca la etapa de generación, con el
AST como entrada.

La etapa de generación crea una nueva instancia de `Rubinius::Generator` y le
comunica a la raíz del AST que genere su bytecode en el objeto `Generator`.

Un `Generator` dispone de un DSL escrito en Ruby para generar bytecode de
Rubinius. En su interior, el generador expone métodos que son un reflejo
directo de las [Instrucciones de
Rubinius](/doc/es/virtual-machine/instructions/). Por ejemplo, para crear una
instrucción que añada un nil a la pila, se podría llamar el método `push_nil`
en una instancia de `Generator`.

La clase `Generator` expone también ciertos métodos convenientes que permiten
generar patrones comunes de bytecode sin tener que preocuparse por detalles a
demasiado bajo nivel de algunas áreas del conjunto de instrucciones de
Rubinius.

Por ejemplo, para llamar a un método directamente directamente utilizando
bytecode de Rubinius, se debe crear primero un literal representando el
nombre del método, y entonces llamar `send_stack` para llamar al método.
Además, si se quiere llamar a un método privado, habría primero que crear un
bytecode que permite específicamente la invocación de métodos privados. Si se
quisiera invocar el método `puts` sin argumentos, permitiendo la invocación de
métodos privados, habría que hacer lo siguiente:

    # aquí, g es una instancia de Generator
    g.allow_private
    name = find_literal(:puts)
    g.send_stack name, 0

Utilizar el método auxiliar `send` simplifica bastante el código anterior:

    g.send :puts, 0, true

Cuando se genera bytecode para un AST, Rubinius invoca el método `bytecode`
en cada nodo del AST con un argumento: la instancia actual de `Generator`. He
aquí la implementación del método `bytecode` para el nodo `if`:

    def bytecode(g)
      pos(g)

      done = g.new_label
      else_label = g.new_label

      @condition.bytecode(g)
      g.gif else_label

      @body.bytecode(g)
      g.goto done

      else_label.set!
      @else.bytecode(g)

      done.set!
    end

En primer lugar, el método llama al método `pos`, un método en la superclase
`Node` que llama a su vez `g.set_line @line`. Esto sirve a la máquina virtual
a la hora de proveer información de debugging sobre el código que ejecutamos.

A continuación, el código utiliza los métodos auxiliares para tratar con
etiquetas. Las instrucciones de Rubinius no tienen ningúna estructura de
control de flujo excepto por algunas instrucciones "goto" (`goto`,
`goto_if_true` y `goto_if_false`). Se puede usar la forma corta `git` para
`goto_if_true` y `gif` para `goto_if_false`. En este caso creamos dos
etiquetas nuevas: una para el final de la condición del `if` y otra que marca
el comienzo del bloque `else`.

Después de crear las dos etiquetas, el nodo `if` invoca el método `bytecode`
en su nodo hijo `@condition`, pasándole el actual objeto `Generator`. Esto
emitirá el bytecode para la condición del `if` en el flujo de instrucciones
actual.

Ese proceso debería dejar en la pila el valor que haya resultado de evaluar la
condición, de modo que si el nodo `if` emite una instrucción `goto_if_false`,
saltará inmediatamente a la etiqueta `else_label`. Entonces usa el mismo
patrón que hemos visto anteriormente para pedir al `@body` que emita su
bytecode en el flujo de instrucciones actual, y entonces emite un `goto`
incondicional hasta el final de todo el `if` (su último `end`).

A continuación necesitamos marcar la posición del `else_label`. Desacoplando
la creación de la etiqueta respecto a su uso, podemos pasársela a la
instrucción `goto` antes de marcar dónde está, requerimiento crucial para
ciertas estructuras de control.

Pedimos entonces al nodo `@else` que emita su bytecode y marcamos la
posición de la etiqueta `done`.

Este proceso ocurre recursivamente desde el nodo raíz a través de todo el
AST, lo cual popula el objeto `Generator` con una representación en bytecode
del AST original empezando desde su raíz.

Probablemente es útil echarle un vistazo a las clases dentro del directorio
`lib/compiler/ast`, donde están definidos todos los nodos AST y sus
respectivos métodos `bytecode`. Es una buena forma también de ver ejemplos
prácticos de uso de la API del `Generator`.

Una vez el `Generator` ha conseguido la representación en bytecode del AST,
invoca la siguiente etapa del compilador: la etapa de Codificación.

## Ficheros mencionados

* *lib/compiler/generator_methods.rb*: Fichero generado que contiene
  "wrappers" alrededor de instrucciones de Rubinius. Estos métodos son un
  reflejo directo de [dichas
  instrucciones](/doc/en/virtual-machine/instructions/).
* *lib/compiler/generator.rb*: La definición del objeto `Generator`. Esta
  clase contiene tanto los métodos "crudos" del generador como también algunas
  APIs de alto nivel para generar patrones comunes de bytecode.
* *lib/compiler/ast*: La definición de todos los nodos AST creados por la
  etapa de Parsing del compilador.

## Personalización

La forma más sencilla de personalizar la etapa de Generación del proceso de
compilación es creando métodos de alto nivel complementando los comunes ya
provistos por la implementación por defecto del `Generator`.

También se puede personalizar la clase del Generador que queremos utilizar.
Para aprender más sobre cómo personalizar determinadas etapas del compilador,
léase [Personalizando las etapas](/doc/en/bytecode-compiler/customization/).
