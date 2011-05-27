---
layout: doc_fr
title: Comment Faire - Ecrire des Benchmarks
previous: Corriger un Test En Echec
previous_url: how-to/fix-a-failing-spec
next: Ecrire un Billet pour le Blog
next_url: how-to/write-a-blog-post
---

Pourquoi benchmarker?

Les Benchmarks sont de bons outils pour comparer Rubinius avec le reste
des runtimes Ruby tels que MRI, JRuby, IronRuby, etc. Il n'est pas
réellement attendu de mesurer les performances de Rubinius lui-même donc
si vous souhaitez contribuer aux benchmarks, suivez ces étapes:

  1. Trouvez les benchmarks existants dans rubinius/benchmarks et
     étudiez leur constructions
  2. Chaque fichier de benchmark doit mesurer un aspect specifique de
     Ruby. Par exemple les différentes façons de supprimer des
     clefs/valeurs dans un tableau associatif (Hash).
  3. Utiliser le framework de benchmark.
  4. Gardez vos benchmarks courts et simples
  5. Les benchmarks ne sont pas censés mesurer Rubinius. Si vous écrivez
     un benchmark pour une classe avec des méthodes avec et sans !, vous
     devrez utiliser une variable dupliqué pour la méthode avec ! mais
     vous n'aurez pas à dupliquer la variable dans la méthode sans !.

Si vous voulez tester un benchmark, vous pouvez exécuter

    bin/benchmark benchmark/core/string/bench_case.rb

ou

    bin/benchmark benchmark/core
