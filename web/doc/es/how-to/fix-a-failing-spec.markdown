---
layout: doc_es
title: Como - Arreglar una spec que falla
previous: Write a Ruby Spec
previous_url: how-to/write-a-ruby-spec
next: Write a Blog Post
next_url: how-to/write-a-blog-post
---

Asegúrese de haber leido:

  *  [Getting Started](/doc/es/getting-started/)
  *  [Specs](/doc/es/specs/)

Luego, siga estos pasos para arreglar una spec que falla:

  1.  Ejecute `rake` para asegurarse que todas las specs de CI están pasando.
  2.  Ejecute `bin/mspec spec/some/spec_file.rb` para confirmar que la spec falla.
  3.  Edite un archivo en algún lugar en Rubinius (probablemente en el directorio kernel).
  4.  Ejecute `rake build` para construir su cambio.
  5.  Ejecute `bin/mspec spec/some/spec_file.rb` para ver que su cambio hace que las spec pasen.
  6.  Repita hasta que su spec pase.
  7.  Ejecute `rake` para asegurarse que no hay regresiones.
  8.  Cambie al directorio raíz de Rubinius si todavía no está en él.
  9.  Ejecute `git status, git add, git commit`, etc. Cualquier cambio hecho a los archivos de spec en el directorio spec/ruby debe estar en un commit diferente al de los cambios hechos en los demás archivos del código de Rubinius.
  10. Ejecute `git format-patch origin`, el cual extraerá los commits del branch actual acumulados desde el último pull desde origin, o `git format-patch
      -N', donde N es el número (1, 2, etc.) de commits con los que ud. quiere generar los parches.
  11. Cree un gist con su patch y linkeelo en el ticket del issue tracker en http://github.com/rubinius/rubinius/issues. Puede agregar múltiples parches a un ticket.

Cuando su parche es aceptado en el project Rubinius, va a obtener commit bit
para el repositorio de Rubinius. Hágale saber a Evan cual es su usuario de Github.
