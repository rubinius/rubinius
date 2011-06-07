---
layout: doc_fr
title: Comment faire - Corriger un Test En Echec
previous: Ecrire un Test Ruby
previous_url: how-to/write-a-ruby-spec
next: Ecrire des Benchmarks
next_url: how-to/write-benchmarks
---

Assurez-vous d'avoir lu:

  *  [Démarrer](/doc/fr/getting-started/)
  *  [Tests](/doc/fr/specs/)

Ensuite, suivez ces étapes pour corriger un test en échec:

  1.  Exécutez `rake` pour vous assurer que tous les tests d'intégration continue sont valides.
  2.  Exécutez `bin/mspec spec/some/spec_file.rb` pour confirmer que le test échoue.
  3.  Corriger le problème où qu'il se trouve dans Rubinius (probablement dans le répertoire `kernel`).
  4.  Exécutez `rake build` pour compiler vos changements.
  5.  Exécutez `bin/mspec spec/some/spec_file.rb` pour vous vérifier si votre changement corrige le test en échec.
  6.  Répétez jusqu'à ce que le test passe.
  7.  Exécutez `rake` pour vous assurer qu'il n'y a pas de régressions.
  8.  Rendez-vous dans le répertoire racine de Rubinius (si vous n'y êtes pas déjà).
  9.  Exécutez `git status, git add, git commit`, etc. Tout changement apporté aux fichiers de tests sous le dossier
  spec/ruby doit être dans un commit différent de celui utilisé pour corriger le code source.
  10. Exécutez `git format-patch origin`, ce qui extraira les commits accumulés dans la branche en cours depuis sa dernière mise à jour depuis origin, ou `git format-patch -N', ou N (un entier, 1, 2,...) est le nombre de commits que vous souhaitez extraire afin de génerer le patch
  11. Créez un gist avec votre patch et liez-le dans un ticket sur le gestionnaire de ticket du projet http://github.com/rubinius/rubinius/issues. Vous pouvez ajoutez plusieurs patchs à un seul ticket.

Quand votre patch est accepté par le project Rubinius, vous aurez un
droit d'accès au dépôt Rubinius. Communiquez votre nom d'utilisateur
GitHub à Evan.
