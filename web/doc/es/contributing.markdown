---
layout: doc_es
title: Aportando a Rubinius
previous: Resolviendo problemas
previous_url: getting-started/troubleshooting
next: Comunicación
next_url: contributing/communication
---

El proyecto Rubinius acepta sus aportaciones. Hay muchas cosas que
hacer en las que puede ayudar. Lo más importante es que usted debe hacer algo que le
interesa. Esa es la mejor manera de tener el entusiasmo y la energía para su
trabajo.

Si usted tiene preguntas sobre Rubinius, la mejor forma de obtener respuestas
es platicar con nosotros en el canal de IRC #rubinius en irc.freenode.net.

A continuación se presentan varias ideas sobre qué hacer en Rubinius.

## Ejecutar su propio código

Su código es a menudo una prueba más rigorosa que las
especificaciones. Ejecute su proyecto favorito en Rubinius y reporte
cualquier anomalía que se presente. Vea [Cómo escribir un reporte de
fallos](/doc/es/how-to/write-a-ticket).


## Pida Ayuda

Todo lo que podamos hacer para ayudarle, lo haremos. Asegúrese de hacer su
propia investigación también, si es posible. Rubinius pretende ser un proyecto
en el que usted pueda fácilmente estudiar, aprender, y contribuir.

Desde luego, se aceptan y aprecian cualquier reporte de errores, pero podemos dar
prioridad a los reportes que incluyen pasos para reproducir el problema.
Incluso mejor son aquellos que incluyen RubySpecs que demuestran el error y un
parche que lo corrige.

## Escribir Especificaciones

  1. Ejecute `bin/mspec tag --list incomplete <dir>` para mostrar las
     especificaciones que se han etiquetado como incompletas. Estas
     especificaciones pueden simplemente necesitar una revisión, o podría
     hacer falta especificaciones para alguna clase en particular.

     NOTA: Se puede especificar el pseudo-directorio ':files' para
     \<dir\>, lo que mostrara las etiquetas para todas las especificaciones
     que deben ejecutarse en Rubinius. O puede especificar cualquier
     subdirectorio de spec/ para listar las etiquetas de las especificaciones
     en ese subdirectorio.

  2. Encuentre conductas no especificadas. Vea [Cómo escribir un Ruby
     Spec](/doc/es/how-to/write-a-ruby-spec).

## Corregir una Especificación erronea

  1. Ejecute `bin/mspec tag --list fails <dir>` para mostrar las especificaciones
     que no son exitosas.

     NOTA: Se puede especificar el pseudo-directorio ':files' para
     \<dir\>, lo que mostrara las etiquetas para todas las especificaciones
     que deben ejecutarse en Rubinius. O puede especificar cualquier
     subdirectorio de spec/ para listar las etiquetas de las especificaciones
     en ese subdirectorio.

  2. Elija una especificación que se vea interesante y vea si puede escribir
     un parche que la haga funcionar exitosamente.

## Escribir Documentación

Estudio cómo funciona Rubinius leyendo el código fuente, y escriba
documentación de alto nivel que ayudará a otros a aprender sobre los
detalles de implementación.

## Mantenimiento a los reportes de error

  * Reviva o cierre algunos reportes de error viejos.
  * Creé casos de prueba mínima en los que se presenten
    anomalias. Luego vea si hay algún RubySpecs para ese caso.
    Si no, considere escribir uno.
