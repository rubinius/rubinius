---
layout: doc_es
title: Cómo - escribir una especificación en Ruby
previous: Escribir un ticket
previous_url: how-to/write-a-ticket
next: Fix a Failing Spec
next_url: how-to/fix-a-failing-spec
translated: true
---

Asegúrese de haber leído:

  *  [Primeros pasos](/doc/es/getting-started/)
  *  [Especificaciones](/doc/es/specs/)

A continuación, siga estos pasos para escribir la especificación de 
un método de Ruby:

  1. Editar un archivo bajo `spec/ruby/...`
  2. Ejecute `bin/mspec -tr spec/ruby/some/spec_file.rb`
  3. Repita hasta que la especificación pase la MatzRuby
  4. Envice sus cambios
  7. Use `git format-patch`
  8. Crea un gist con su parche y enlace a un billete de incidencia en el
     tracker en http://github.com/rubinius/rubinius/issues.
