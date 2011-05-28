---
layout: doc_fr
title: Comment faire - Traduire la Documentation
previous: Ecrire de la Documentaiton
previous_url: how-to/write-documentation
next: Appendice A - Glossaire
next_url: appendix-a-glossary
---

Il y a deux tâches:

1. Mettre à jour la documentation existante
1. Créer de la nouvelle documentation

Commencez par lire [Comment Faire - Ecrire de la Documentation](/doc/fr/how-to/write-documentation/)


### Mettre à jour la documentation existante

Pour mettre à jour la documentation existante, ouvrez un fichier sous `web/doc/LANG`
et éditez le texte existant ou ajoutez du nouveau contenu.

### Créer de la nouvelle documentation

Pour créer une nouvelle traduction:

1. Copier `web/doc/en` vers `web/doc/LANG` où _LANG_ est le code
   [ISO-639-1](http://en.wikipedia.org/wiki/List_of_ISO_639-2_codes) pour
   la traduction que vous créez.
1. Editer les liens dans la Table des Matières pour pointer vers la cette traduction. (Notez que le `page.base_dir`
n'est pas accessible quand ces fichiers sont rendus via Jekyll. Cela mériterait d'être investigué).
1. Traduire le texte de l'Anglais vers la langue en question.

