---
layout: doc_es
title: Specs
previous: Ruby - Global Variables
previous_url: ruby/global-variables
next: RubySpec
next_url: specs/rubyspec
translated: true
---

El proyecto Rubinius generalmente usa especificaciones ejecutables al estilo
de TDD/BDD para impulsar el desarrollo. El directorio de 'spec' se divide
conceptualmente en dos partes:

   1. Todos los archivos en "./spec/ruby' que describen el comportamiento de
      MatzRuby.
   2. Y todos los otros archivos del './spec' de la guía que describen el
      comportamiento de Rubinius.

Las especificaciones de los casos ./spec/ruby son una copia de RubySpec a una
revisión en concreto.  Estos son regularmente importados del proyecto RubySpec
y especificaciones técnicas no etiquetados para que el proceso de CI siempre
se ejecuta un conjunto conocido bien de especificaciones técnicas. Este
permite confirmar fácilmente que los cambios en el código Rubinius no causan
regresiones.

Documentación sobre la organización de las especificaciones y directrices para
escribir especificaciones técnicas se puede encontrar en el proyecto
[RubySpec](http://rubyspec.org/).

Utilice el siguiente flujo de trabajo al añadir especificaciones técnicas y el
código para Rubinius:

   1. Escribe en su defecto las especificaciones de algún aspecto del
      comportamiento de Ruby. Comprometerse en las especificaciones uno por
      separado se comprometen a los archivos correspondientes en ./spec/ruby.
   2. Agregue código Rubinius para hacer pasar las especificaciones. Una vez
      más, cometen estos cambios en uno para confirmación de los cambios de
      especificaciones.
   3. Ejecutar el comando `rake` para asegurar que todas las especificaciones
      técnicas IC pasar.

Los cambios a los archivos en ./spec/ruby son regularmente empujó a la
RubySpec del proyecto. Además, los cambios actuales RubySpec de committers a
otros Ruby implementaciones se actualizan periódicamente en el archivo
./spec/ruby. Cuando el especificaciones técnicas se actualizan RubySpec, las
etiquetas CI también se actualizan.
