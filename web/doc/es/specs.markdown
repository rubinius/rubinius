---
layout: doc_es
title: Especificaciones
previous: Ruby - Variables Globales
previous_url: ruby/global-variables
next: RubySpec
next_url: specs/rubyspec
---

El proyecto Rubinius utiliza especificaciones ejecutables al estilo
TDD/BDD para impulsar el desarrollo. El directorio 'spec' se divide
conceptualmente en dos partes:

   1. Todos los archivos en "./spec/ruby' que describen el comportamiento de
      MatzRuby.
   2. Y todos los otros archivos dentro del directorio './spec' que
      describen el comportamiento de Rubinius.

Las especificaciones dentro de ./spec/ruby son una copia de RubySpec
en una revisión en concreto.  Estos se actualizan regularmente desde
el proyecto RubySpec y las especificaciones que terminan en error se
etiquetan para que el proceso de intrgración continua (CI) utilize
siempre un conjunto de especificaciones que se sabe son exitosas. Esto
permite comprobar rapidamente que los cambios hechos al código de
Rubinius no causan regresiones.

Se puede encontrar más información sobre la organización de las
especificaciones y guías para escribirlas en [RubySpec](http://rubyspec.org/).

Utilice el siguiente flujo para agregar especificaciones y código para Rubinius:

   1. Escriba una especificación no exitosa para algun aspecto del
      comportamiento de Ruby. Incluya los archivos correspondientes
      dentro de ./spec/ruby creando un commit para ellos.
   2. Agregue código a Rubinius para hacer pasar las especificaciones. Una vez
      más, haga commit de estos cambios por separado de las archivos
      de especificaciones.
   3. Ejecute el comando `rake` para asegurarse que todas las especificaciones
      anteriores y de integración continua siguen funcionando.

Los cambios a los archivos dentro de ./spec/ruby son normalmente
enviados al proyecto RubySpec. De igual manera, los cambios a RubySpec
hechos por participantes de otras implementaciones de Ruby se obtienen
regularmente para actualizar los directorios ./spec/ruby. Cuando las
especificaciones se actualizan desde RubySpec, las etiquetas de
integración continua (CI) se actualizan también.
