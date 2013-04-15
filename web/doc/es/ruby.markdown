---
layout: doc_es
title: Ruby
previous: Guía de estilo
previous_url: contributing/style-guide
next: Scripts
next_url: ruby/scripts
review: true
---

Una forma de acercarse a la comprensión de cómo Rubinius implementa Ruby
comienza con la comprensión de cómo funciona los distintos conceptos de Ruby.
El propósito de este tema es introducir las características Rubinius como
conceptos de Ruby que deberían ser familiares. La documentación de Rubinius
generalmente asume que ya tienes experiencia con Ruby o con coneptos de
máquinas virtuales y compiladores.

El concepto de _scope_ es central a los temas siguientes. En la sintaxis
de Ruby, _scope_ es generalmente un concepto derivado. En otras palabras,
no hay elementos sintácticos cuya responsablidad primaria sea delimitar o
referenciar un _scope_. Por este motivo, puede ser confuso hablar de _scope_.
Un ejemplo de esto es una típica definición de método:

    a = 5

    def diligent(a)
      puts a * 2
    end

Aquí, el metodo `#diligent` nos da un nombre para referirnos al cálculo
`puts a * 2`. Pero el método también define un _scope_ léxico cerrado para
la variable `a`. El _scope_ es cerrado porque la sentencia `a = 5` fuera de
la definición del método no tiene relación con la variable `a` en `#diligent`.

Usualmente se dice que todo en Ruby es un objeto. Esto no es del todo cierto.
_La mayoría_ de las cosas en Ruby son objetos, pero algunas cosas que son
absolutamente esenciales para ejecutar código Ruby no son necesariamente
objetos accesibles. Cuales de las cosas del ambiente de ejecución son objetos
en Ruby depende altamente de la implementación. El _scope_ es una de esas
cosas.

Cada uno de los siguientes elementos de Ruby se discuten desde la perspectiva
de entender como las implementa Rubinius y como se involucra el concepto
de _scope_ en cada uno.

1. [Scripts](/doc/es/ruby/scripts/)
1. [Métodos](/doc/es/ruby/methods/)
1. [Constantes](/doc/es/ruby/constants/)
1. [Clases y Módulos](/doc/es/ruby/classes-and-modules/)
1. [Bloques y Procs](/doc/es/ruby/blocks-and-procs/)
1. [Variables locales](/doc/es/ruby/local-variables/)
1. [Variables de instancia](/doc/es/ruby/instance-variables/)
1. [Variables de clase](/doc/es/ruby/class-variables/)
1. [Variables globales](/doc/es/ruby/global-variables/)
