---
layout: doc_es
title: Contributing
previous: Troubleshooting
previous_url: getting-started/troubleshooting
next: Communication
next_url: contributing/communication
translated: true
---

El proyecto Rubinius la bienvenida a sus contribuciones. Hay muchas cosas que
hacer para ayudar. Lo más importante es que usted debe hacer algo que le
interesa.  Esa es la mejor manera de tener el entusiasmo y la energía para su
trabajo.

Si usted tiene preguntas sobre Rubinius, la mejor forma de obtener respuestas
es para chatear con nosotros en el canal # Rubinius IRC en irc.freenode.net.

A continuación se presentan varias ideas sobre qué hacer en Rubinius.

## Ejecutar el código

Su código es a menudo más viciosos de las especificaciones. Monta tu proyecto
favorito en Rubinius y emite un informe. Mas [Cómo escribir un billete](/doc/en/how-to/write-a-ticket).


# # Pedir Ayuda

Todo lo que podemos hacer para ayudar, lo haremos. Asegúrese de hacer su
propia investigación también, si es posible. Rubinius pretende ser un proyecto
que fácilmente se pueden estudiar, aprender, y espero que se extienden.

Desde luego, se aceptan y aprecian informes simples errores, pero podemos dar
prioridad a las entradas que incluyen pasos para reproducir el problema.
Incluso mejor son entradas que incluyen RubySpecs que demuestran el error y un
parche que la fija.

## Escribir Especificaciones

  1. Ejecutar `bin/mspec tag --list incomplete <dir>` para mostrar
     especificaciones técnicas que se han Etiquetado como incompleta. Estas
     especificaciones pueden simplemente necesitan una revisión, o podría no
     se especificaciones técnicas que faltan para una clase en particular.

     NOTA: Se puede especificar el pseudo-directorio ':files' para <dir>, que
     etiquetas para mostrar el resultado de todas las especificaciones
     técnicas que deben ejecutarse en Rubinius. O puede especificar cualquier
     subdirectorio de spec/ para las etiquetas de lista de especificaciones
     técnicas en ese subdirectorio.

  2. Buscar las conductas no especificadas. Mas [Cómo escribir un Ruby
     Spec](/doc/en/how-to/write-a-ruby-spec).

## Fijar A falta de especificaciones

  1. Ejecutar `bin/mspec tag --list fails <dir>` para mostrar especificaciones
     técnicas etiquetados como no.

     NOTA: Se puede especificar el pseudo-directorio 'files' para <dir>, que
     etiquetas para mostrar el resultado de todas las especificaciones
     técnicas que deben ejecutarse en Rubinius. O puede especificar cualquier
     subdirectorio de spec/ para las etiquetas de lista de especificaciones
     técnicas en ese subdirectorio.

  2. Elija una especificación que se ve interesante y ver si puedes escribir
     un parche que hará pasar.

## Escribir Docs

Estudiar cómo Rubinius obras y escribir documentación alto nivel que ayudarán
a a otros a aprender acerca de los detalles de implementación.

## Limpiar el Código

Búsqueda de etiquetas como TODO, HACK, FIXME en el código y parches para
arreglar presentar ellos. Aquí hay un comando para buscar el código:

    `grep -re "@todo\|TODO\|HACK\|FIXME" .`

Revise el [Guía de estilo] (/doc/es/contributing/style-guide/) para
la codificación de las directrices.

## Triage Tickets

  * Revive o cerrar entradas de edad.
  * Crear casos de prueba mínima que se reproducen los insectos. Luego ver si
    hay ya RubySpecs para la expedición. Si no, considere escribir algunas.
