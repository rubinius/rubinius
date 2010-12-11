---
layout: doc_es
title: Coding Style Guide
previous: Communication
previous_url: contributing/communication
next: Ruby
next_url: ruby
translated: true
---

Las siguientes directrices tienen por objeto mantener el código base Rubinius
de mantener. Cuando En caso de duda acerca de una guía aquí, nos preguntan en
el canal de IRC en # Rubinius irc.freenode.net.


## Todas Código

  * Configurar el editor de etiquetas suaves, no etiquetas duras
  * Ampliar etiquetas a dos espacios
  * Deja una nueva línea al final al final de cada archivo

## C++ Código

  * No hay espacio entre la condición y paréntesis.
      Utilice `if(1)` NO `if (1)`

  * Ponga la llave de apertura en la misma línea que la declaración de la
    función o condicional

  * Utilice siempre las llaves, aunque se puede dejar fuera.

  * Prefiere paréntesis a las reglas de prioridad implícita (dentro de lo
    razonable)

  * Versiones alternativas de las funciones debe ser nombrado por qué son
    diferentes de la primaria. Si hay una función de 'person()' y quiere una
    de la versión que lleva el nombre de la persona, debe ser
    'person_with_name(char \*name)' o 'person_with_details(char \*nome, ...)'.
    NO 'person1(char \*name)'.


## Código de Ruby

  * Métodos: Trate de mantener sus métodos a corto - una pantalla y tratar de
    cumplir a DRY dentro de lo razonable. En general, la funcionalidad común
    debe ser extraída a métodos auxiliares (que usted puede hacer "privado"),
    pero, en algunos casos en particular de trabajo con el sistema central, a
    veces tratando de cosas SECO hasta es sólo un obstáculo si tienes que
    maniobrar alrededor de error diferente varias condiciones, por ejemplo.

  * Los nombres de método: debe ser clara, expresiva y significativa. Evite el
    uso de subraya que "proteger" el método ('\ _\_send\_\_') con algunas
    excepciones.

  * Los nombres de método de estilo de Smalltalk-está bien, lo que significa
    que podría haber un método SomeClass.make_from cuando está destinado a ser
    invocada como `SomeClass.make_from file` o `SomeClass.make_from :file =>
    name. Allí, el nombre de parámetro _completa_ el nombre del método y lo
    convierte en más natural lectura.

  * Los nombres de variables: que sean claros y significativos (con algunas
    conocidas excepciones como la utilización de 'i' de un contador.) Trate de
    evitar las sombras método nombres, por ejemplo el 'idx' se usa en vez de
    'index' porque el último es también un nombre de método.

  * Postconditions: uso postcondiciones sólo *si* su expresión es un de una
    sola línea *y* no tiene muchas condiciones.

  * Bloques: Utilice `do ... end` o `{...}`, espacio entre los delimitadores
    y código (`foo { |arg| código }`) . Split largo o expresiones más complejas
    varias líneas como esta:

        mapped = foo.map do |elem|
          do_something_with elem
        end

  * Módulo/Definiciones de clase con los calificadores ámbito de aplicación:

        module Rubinius::Profiler
          class Sampler
          end
        end

## Código del Kernel

La directriz principal para todo el código del núcleo es simple y eficiente.
Simple código es a menudo más eficaz y comprensible por lo general mucho más.
No debe haber metaprogramación código de arranque. Utilice los métodos
attr_xxx # en todo el las fuentes del núcleo. Además, los métodos de alias
utilizando el # alias_method siguiente llamada a la método de definición.
Especificar los métodos privados con: método de `private :sym` junto a la
definición del método. Recuerde que las versiones de los métodos enumerados
más arriba en la etapa alfa tener un único argumento, el símbolo.

## Documentación

  * Utilice RDoc para la documentación en el código Ruby.

  * Uso de Doxygen para la documentación en código C++.

  * Utilice Markdown para la documentación en el directorio / doc. Ver
    [Sintaxis de Markdown](http://daringfireball.net/projects/markdown/syntax).
    Establecer el texto ancho de 78 caracteres y el uso de las pausas duro.
