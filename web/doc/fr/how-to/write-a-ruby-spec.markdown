---
layout: doc_fr
title: Comment Faire - Ecrire un Test Ruby
previous: Ecrire un Ticket
previous_url: how-to/write-a-ticket
next: Corriger un Test En Echec
next_url: how-to/fix-a-failing-spec
---

Assurez-vous d'avoir lu:

  *  [Démarrer](/doc/en/getting-started/)
  *  [Tests](/doc/en/specs/)

Ensuite, suivez ces étapes pour écrire le test d'une méthode Ruby:

  1. Editer un fichier dans `spec/ruby/...`
  2. Lancez `bin/mspec -tr spec/ruby/le/fichier/de/test.rb`
  3. Répétez jusqu'à ce que votre test passe avec l'implémentation
     historique MRI (aussi connue sous le nom de MatzRuby)
  4. Committez vos changements
  7. Utilisez `git format-patch`
  8. Créez un gist avec votre patch et faites référence à ce ticket sur notre gestionnaire de ticket <http://github.com/rubinius/rubinius/issues>.
