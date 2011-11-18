---
layout: doc_es
title: Cómo - escribir una Ruby spec
previous: Escribir un ticket
previous_url: how-to/write-a-ticket
next: Fix a Failing Spec
next_url: how-to/fix-a-failing-spec
---

Asegúrese de haber leído:

  *  [Primeros pasos](/doc/es/getting-started/)
  *  [Especificaciones](/doc/es/specs/)

A continuación, siga estos pasos para escribir una spec para un método Ruby:

  1. Editar un archivo en `spec/ruby/...`
  2. Ejecute `bin/mspec -tr spec/ruby/some/spec_file.rb`
  3. Repita hasta que la spec pase en MatzRuby
  4. Comitee sus cambios
  7. Use `git format-patch`
  8. Cree un gist con su parche y agregue  el enlace a este en un ticket en el issue tracker en http://github.com/rubinius/rubinius/issues.
