---
layout: doc_es
title: Guía de estilo
previous: Comunicación
previous_url: contributing/communication
next: Ruby
next_url: ruby
---

Las siguientes directrices tienen por objeto mantener el código de Rubinius
entendible. Cuando tenga alguna duda acerca de una guía presentada aquí,
pregunte en el canal de IRC #rubinius en irc.freenode.net.

## Para todo el código

  * Configure su editor para utilizar tabuladores suaves, no
    tabuladores duros.
  * Utilice indentación a dos espacios
  * Deje una línea nueva al final cada archivo
  * Utilice nombres de variables, clases, comentarios en inglés.

## Código C++

  * No ponga espacio entre la condición y paréntesis.
      Utilice `if(1)`, NO `if (1)`

  * Ponga la llave de apertura en la misma línea que la declaración de la
    función o condicional

  * Utilice siempre llaves, aunque se puedan omitir.

  * Prefiera paréntesis a las reglas de prioridad implícita (dentro de lo
    razonable)

  * Versiones alternativas de las funciones deben ser nombradas en función de
    por qué son diferentes de la primera. Si hay una función 'person()' y
    quiere una versión que tome el nombre de la persona, esta debería ser
    'person_with_name(char \*name)' o 'person_with_details(char \*name, ...)',
    NO 'person1(char \*name)'.

## Código Ruby

  * Métodos: Trate de mantener sus métodos cortos (que se puedan visualizar en
    una sola pantalla) y trate de cumplir el principio DRY dentro de lo
    razonable. En general, la funcionalidad común debe ser extraída en métodos
    auxiliares (que usted puede hacer "privados"), pero en algunos casos, en
    particular cuando se trabaja con código del núcleo, a veces tratar de 
    hacer las cosas en un estilo DRY puede ser sólo un obstáculo, por ejemplo
    si se tiene que maniobrar entre de diferentes condiciones de error.

  * Los nombres de método: deben ser claros, expresivos y significativos.
    Evite el uso de guines bajos para "proteger" el método ('\_\_send\_\_')
    con algunas excepciones.

  * Los nombres de método estilo Smalltalk están bien, lo que significa
    que podría haber un método SomeClass.make_from cuando está destinado a ser
    invocado como `SomeClass.make_from file` o
    `SomeClass.make_from :file => name`.  En estos casos el nombre del
    parámetro _completa_ el nombre del método y contribuye a una lectura más
    natural.

  * Los nombres de variables: que sean claros y significativos (con algunas
    conocidas excepciones como la utilización de 'i' como un contador.) Trate
    de evitar enmascarar los nombres de métodos, por ejemplo dentro de Array
    utilice 'idx' en vez de 'index' porque el último es también un nombre de
    método.

  * Post-condiciones: use post-condiciones sólo *si* su expresión es de una
    sola línea *y* no tiene muchas condiciones.

  * Bloques: Utilice `do ... end` o `{...}`, con espacios entre los
    delimitadores y el código (`foo { |arg| código }`). Separe expresiones
    largas o complejas en varias líneas, de esta forma:

        mapped = foo.map do |elem|
          do_something_with elem
        end

  * Definiciones de Módulos/Clases con scope:

        module Rubinius::Profiler
          class Sampler
          end
        end

## Código del Kernel (núcleo)

La directriz principal para todo el código del núcleo es simple y eficiente.
Un código simple es a menudo más eficaz y generalmente más facil de
comprender. No debe haber metaprogramación en el código de inicio. Utilice los
métodos #attr_xxx en todo el código fuente del núcleo. Además, haga alias de
métodos utilizando #alias_method inmediatamente despues de la definición del
método. Especifique los métodos privados con `private :sym` justo después de
la definición del método. Recuerde que las versiones de los métodos enumerados
aquí durante la etapa alfa toman un argumento único: un símbolo.

## Documentación

  * Utilice RDoc para la documentación del código Ruby.

  * Use Doxygen para la documentación del código C++.

  * Utilice Markdown para la documentación dentro del directorio
    /doc. Vea [Sintaxis de
    Markdown](http://daringfireball.net/projects/markdown/syntax). Establezca
    el ancho de texto a 78 caracteres y el uso de fin de lineas duras.
